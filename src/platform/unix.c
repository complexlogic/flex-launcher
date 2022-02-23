#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <SDL.h>
#include "../external/ini.h"
#include "../launcher.h"
#include <launcher_config.h>
#include "unix.h"
#include "../util.h"
#include "../debug.h"
#include "slideshow.h"

pid_t child_pid;

// A function to handle .desktop lines
static int desktop_handler(void *user, const char *section, const char *name, const char *value)
{
  desktop_t *pdesktop = (desktop_t*) user;
  if (!strcmp(pdesktop->section, section) && !strcmp(name, KEY_EXEC)) {
    copy_string(&pdesktop->exec, value);
  }
}

// A function to determine if a file exists in the filesystem
bool file_exists(const char *path)
{
  if(access(path, R_OK) == 0) {
    return true;
  }
  else {
    return false;  
  }
}

// A function to determine if a directory exists in the filesystem
bool directory_exists(const char *path)
{
  struct stat directory;
  if (stat(path, &directory) == 0 && S_ISDIR(directory.st_mode)) {
    return true;
  }
  else {
    return false;
  }
}

// A function to remove field codes from .desktop file Exec line
static void strip_field_codes(char *cmd)
{
  int start = 0;
  for (int i = 0; i < strlen(cmd); i++) {
    if (cmd[i] == '%' && i > 0 && cmd[i - 1] == ' ') {
      start = i;
    }
    else if (start && i > start + 2 && cmd[i] != ' ') {
      strcpy(cmd + start, cmd + i);
      start = 0;
    }
  }
  if (start) {
    cmd[start - 1] ='\0'; 
  }
}

// A function to make a directory, including any intermediate
// directories if necessary
void make_directory(const char *directory) 
{
  char buffer[MAX_PATH_CHARS + 1];
  char *i = NULL;
  int length;
  snprintf(buffer, sizeof(buffer), "%s", directory);
  length = strlen(buffer);
  if (buffer[length - 1] == '/') {
    buffer[length - 1] = '\0';
  }   
  for (i = buffer + 1; *i != '\0'; i++) {
    if (*i == '/') {
      *i = '\0';
      mkdir(buffer, S_IRWXU);
      *i = '/';
    }
  }
  mkdir(buffer, S_IRWXU);
}

// A function to determine if a string ends with a phrase
static bool ends_with(const char *string, const char *phrase)
{
  int len_string = strlen(string);
  int len_phrase = strlen(phrase);
  if (len_phrase > len_string) {
    return false;
  }
  char *p = string + len_string - len_phrase;
  if (!strcmp(p, phrase)) {
    return true;
  }
  else {
    return false;
  }
}

// A function to launch an external application
bool start_process(char *cmd, bool check_result)
{
  // Check if the command is an XDG .desktop file
  char *tmp = NULL;
  char *exec = NULL;
  copy_string(&tmp, cmd);
  char *file = strtok(tmp, DELIMITER_ACTION);
  if (ends_with(file, EXT_DESKTOP)) {
    desktop_t desktop;
    desktop.exec = NULL;

    // Parse the desktop action from the command (if any)
    char *action = strtok(NULL, DELIMITER_ACTION);
    if (action == NULL) {
      strncpy(desktop.section, DESKTOP_SECTION_HEADER, sizeof(desktop.section));
    }
    else {
      snprintf(desktop.section, sizeof(desktop.section), DESKTOP_SECTION_HEADER_ACTION, action);
    }

    // Parse the .desktop file for the Exec line value
    int error = ini_parse(file, desktop_handler, &desktop);
    if (error < 0) {
      output_log(LOGLEVEL_ERROR, "Error: Desktop file \"%s\" not found\n", file);
      free(tmp);
      return false;
    }
    if (desktop.exec == NULL) {
      output_log(LOGLEVEL_DEBUG, "No Exec line found in desktop file \"%s\"\n", cmd);
      free(tmp);
      return false;
    }
    exec = desktop.exec;
    strip_field_codes(exec);
    cmd = exec;
  }
  free(tmp);

  // Launch application in system shell
  child_pid = fork();
  switch(child_pid) {
    case -1:
      output_log(LOGLEVEL_ERROR, "Error: Could not fork new process for application\n");
      free(exec);
      return false;

    // Child process
    case 0:
      setpgid(0, 0);
      const char *file = "/bin/sh";
      const char *args[] = {
        "sh",
        "-c", 
        cmd, 
        NULL
      };
      execvp(file, args);
      break;

    // Parent process
    default:
      if (!check_result) return true;
      int status;

      // Check to see if the shell successfully launched
      SDL_Delay(10);
      pid_t pid = waitpid(child_pid, &status, WNOHANG);
      if (WIFEXITED(status) && WEXITSTATUS(status) > 126) {
        output_log(LOGLEVEL_ERROR, "Error: Application failed to launch\n");
        return false;
      }
      output_log(LOGLEVEL_DEBUG, "Application launched successfully\n");
      break;
  }
  free(exec);
  return true;
}

// A function to check if a child process is still running
bool process_running()
{
  pid_t pid = waitpid(-1*child_pid, NULL, WNOHANG);
  if (pid > 0) {
    if (waitpid(-1*child_pid, NULL, WNOHANG) == -1) {
      return false;
    }
  } 
  else if (pid == -1) {
    return false;
  }
  return true;
}

// A function to determine if a file is an image file
int image_filter(struct dirent *file)
{
  int len_file = strlen(file->d_name);
  int len_extension;
  for (int i = 0; i < NUM_IMAGE_EXTENSIONS; i++) {
    len_extension = strlen(extensions[i]);
    if (len_file > len_extension && 
    !strcmp(file->d_name + len_file - len_extension, extensions[i])) {
      return 1;
    }
  }
  return 0;
}

// A function to scan a directory for images
int scan_slideshow_directory(slideshow_t *slideshow, const char *directory)
{
  struct dirent **files;
  int n = scandir(directory, &files, image_filter, NULL);
  char file_path[MAX_PATH_CHARS + 1];
  for (int i = 0; i < n; i++) {
    if (i < MAX_SLIDESHOW_IMAGES) {
      join_paths(file_path, 2, directory, files[i]->d_name);
      copy_string(&slideshow->images[i], file_path);
    }
    free(files[i]);
  }
  free(files);
  if (n <= MAX_SLIDESHOW_IMAGES) {
    slideshow->num_images = n;
  }
  else {
    slideshow->num_images = MAX_SLIDESHOW_IMAGES;
  }
  return slideshow->num_images;
}

void get_region(char *buffer)
{
  char *lang = getenv("LANG");
  char *token = strtok(lang, "_");
  if (token == NULL) {
    return;
  }
  token = strtok(NULL, ".");
  if (token != NULL && strlen(token) == 2) {
    strcpy(buffer, token);
  }
}

// A function to print usage to the command line
void print_usage()
{
  printf("Usage: %s [OPTIONS]\n", EXECUTABLE_TITLE);
  printf("-c, --config             Path to config file.\n");
  printf("-d, --debug              Enable debug messages.\n");
  printf("-h, --help               Show this help message.\n");
  printf("-v, --version            Print version information.\n");
}

// A function to print the version and other info to command line
void print_version()
{
  #if (PROJECT_VERSION_PATCH + 0)
  printf("%s version %i.%i.%i\n", PROJECT_NAME,
                                  PROJECT_VERSION_MAJOR,
                                  PROJECT_VERSION_MINOR,
                                  PROJECT_VERSION_PATCH);
  #else
  printf("%s version %i.%i\n", PROJECT_NAME, PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR);
  #endif
}