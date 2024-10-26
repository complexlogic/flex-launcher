#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <SDL.h>
#include <ini.h>
#include "../launcher.h"
#include <launcher_config.h>
#include "unix.h"
#include "../util.h"
#include "../debug.h"
#include "platform.h"
#include "slideshow.h"

static int desktop_handler(void *user, const char *section, const char *name, const char *value);
static void strip_field_codes(char *cmd);
static bool ends_with(const char *string, const char *phrase);
static int image_filter(const struct dirent *file);

// A function to handle .desktop lines
static int desktop_handler(void *user, const char *section, const char *name, const char *value)
{
    Desktop *pdesktop = (Desktop*) user;
    if (!strcmp(pdesktop->section, section) && !strcmp(name, KEY_EXEC))
        pdesktop->exec = strdup(value);
    return 0;
}

// A function to determine if a file exists in the filesystem
bool file_exists(const char *path)
{
    return access(path, R_OK) ? false : true;
}

// A function to determine if a directory exists in the filesystem
bool directory_exists(const char *path)
{
    struct stat directory;
    return stat(path, &directory) == 0 && S_ISDIR(directory.st_mode) ? true : false;
}

// A function to remove field codes from .desktop file Exec line
static void strip_field_codes(char *cmd)
{
    size_t start = 0;
    for (size_t i = 0; i < strlen(cmd); i++) {
        if (cmd[i] == '%' && i > 0 && cmd[i - 1] == ' ')
            start = i;
        else if (start && i > start + 2 && cmd[i] != ' ') {
            strcpy(cmd + start, cmd + i);
            start = 0;
        }
    }
    if (start)
        cmd[start - 1] ='\0'; 
}

// A function to make a directory, including any intermediate
// directories if necessary
void make_directory(const char *directory) 
{
    char buffer[MAX_PATH_CHARS + 1];
    char *i = NULL;
    size_t length;
    snprintf(buffer, sizeof(buffer), "%s", directory);
    length = strlen(buffer);
    if (buffer[length - 1] == '/')
        buffer[length - 1] = '\0';
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
    size_t len_string = strlen(string);
    size_t len_phrase = strlen(phrase);
    if (len_phrase > len_string)
        return false;
    char *p = (char*) string + len_string - len_phrase;
    return strcmp(p, phrase) ? false : true;
}

// A function to launch an external application
bool start_process(char *cmd, bool application)
{
    // Check if the command is an XDG .desktop file
    char *exec = NULL;
    char *tmp = strdup(cmd);
    char *file = strtok(tmp, DELIMITER_ACTION);
    if (ends_with(file, EXT_DESKTOP)) {
        Desktop desktop;
        desktop.exec = NULL;

        // Parse the desktop action from the command (if any)
        const char* const action = strtok(NULL, DELIMITER_ACTION);
        if (action == NULL)
            copy_string(desktop.section, DESKTOP_SECTION_HEADER, sizeof(desktop.section));
        else
            snprintf(desktop.section, sizeof(desktop.section), DESKTOP_SECTION_HEADER_ACTION, action);

        // Parse the .desktop file for the Exec line value
        int error = ini_parse(file, desktop_handler, &desktop);
        if (error < 0) {
            log_error("Desktop file '%s' not found", file);
            free(tmp);
            return false;
        }
        if (desktop.exec == NULL) {
            log_debug("No Exec line found in desktop file '%s'", cmd);
            free(tmp);
            return false;
        }
        exec = desktop.exec;
        strip_field_codes(exec);
        cmd = exec;
    }
    free(tmp);

    // Fork new system shell process
    pid_t child_pid = fork();
    switch(child_pid) {
        case -1:
            log_error("Could not fork new process for application");
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
            execvp(file, (char* const*) args);
            break;

        // Parent process
        default:
            if (!application) 
                return true;
            int status;

            // Check to see if the shell successfully launched
            SDL_Delay(10);
            waitpid(child_pid, &status, WNOHANG);
            if (WIFEXITED(status) && WEXITSTATUS(status) > 126) {
                log_error("Application failed to launch");
                return false;
            }
            log_debug("Application launched successfully");
            break;
    }
    free(exec);
    return true;
}

// A function to determine if a file is an image file
int image_filter(const struct dirent *file)
{
    size_t len_file = strlen(file->d_name);
    size_t len_extension;
    for (size_t i = 0; i < NUM_IMAGE_EXTENSIONS; i++) {
        len_extension = strlen(extensions[i]);
        if (len_file > len_extension && 
        !strcmp(file->d_name + len_file - len_extension, extensions[i]))
            return 1;
    }
    return 0;
}

// A function to scan a directory for images
void scan_slideshow_directory(Slideshow *slideshow, const char *directory)
{
    struct dirent **files;
    slideshow->num_images = scandir(directory, &files, image_filter, NULL);
    slideshow->images = malloc((size_t) slideshow->num_images * sizeof(char*));
    char file_path[MAX_PATH_CHARS + 1];
    for (int i = 0; i < slideshow->num_images; i++) {
        join_paths(file_path, sizeof(file_path), 2, directory, files[i]->d_name);
        slideshow->images[i] = strdup(file_path);
        free(files[i]);
    }
    free(files);
}

void get_region(char *buffer)
{
    char *lang = getenv("LANG");
    char *token = strtok(lang, "_");
    if (token == NULL)
        return;
    token = strtok(NULL, ".");
    if (token != NULL && strlen(token) == 2)
        copy_string(buffer, token, 3);
}

// A function to shutdown the computer
void scmd_shutdown()
{
    start_process(CMD_SHUTDOWN, false);
}

// A function to restart the computer
void scmd_restart()
{
    start_process(CMD_RESTART, false);
}

// A function to put the computer to sleep
void scmd_sleep()
{
    start_process(CMD_SLEEP, false);
}

// A function to print usage to the command line
void print_usage()
{
    printf("Usage: " EXECUTABLE_TITLE " [OPTIONS]\n");
    printf("  -c p, --config=p   Load config file from path p.\n");
    printf("  -d,   --debug      Enable debug messages.\n");
    printf("  -h,   --help       Show this help message.\n");
    printf("  -v,   --version    Print version information.\n");
}
