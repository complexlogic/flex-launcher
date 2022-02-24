#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>
#include <psapi.h>
#include <SDL.h>
#include <SDL_syswm.h>
#include "../launcher.h"
#include <launcher_config.h>
#include "platform.h"
#include "win32.h"
#include "../util.h"
#include "../debug.h"
#include "slideshow.h"

extern config_t config;
extern SDL_SysWMinfo wm_info;
bool web_browser;
LPWSTR w_process_basename     = NULL;
HANDLE child_process          = NULL;

// A function to get the basename of a file
static LPWSTR path_basename(LPCWSTR w_path)
{
  size_t length = wcslen(w_path);
  if (length) {
    // Find first path separator, count back from end of string
    LPWSTR p = w_path + length - 1;
    while (*p != L'\\' && *p != L'/' && p > w_path) {
      p -= 1;
    } 
    return p + 1;
  }
  return NULL;
}

// A function to determine if a process name is a web browser
static bool is_browser(LPCWSTR w_exe_basename)
{
  LPCWSTR *browsers[] = {
    L"chrome.exe",
    L"msedge.exe",
    L"firefox.exe"
  };
  for (int i = 0; i < sizeof(browsers) / sizeof(browsers[0]); i++) {
    if (!wcscmp(w_exe_basename, browsers[i])) {
      return true;
    }
  }
  return false;
}

// A function to convert a UTF-8 string to UTF-16
static void convert_utf8_string(LPWSTR w_string, const char *string, int buffer_size)
{
  MultiByteToWideChar(CP_UTF8,
    0,
    string,
    -1,
    w_string,
    buffer_size
  );
}

// A function to convert a UTF-16 string to UTF-8
static void convert_utf16_string(char *string, LPCWSTR w_string, int buffer_size)
{
  WideCharToMultiByte(CP_UTF8,
    0,
    w_string,
    -1,
    string,
    buffer_size,
    NULL,
    NULL
  );
}

// A function to convert a UTF-8 string to UTF-16, and allocate memory for it
static void convert_utf8_alloc(LPWSTR *buffer, const char *string)
{
  int length = strlen(string);
  int buffer_size = sizeof(WCHAR)*(length + 1);
  *buffer = malloc(buffer_size);
  convert_utf8_string(*buffer, string, buffer_size);
}

static void convert_utf16_alloc(const char **buffer, LPCWSTR w_string)
{
  size_t length = wcslen(w_string);
  int buffer_size = 2*sizeof(WCHAR)*(length + 1);
  *buffer = malloc(buffer_size);
  convert_utf16_string(*buffer, w_string, buffer_size);
}

// A function to determine if a file with wide char path exists on the filesystem
static bool w_file_exists(LPCWSTR w_path)
{
  if (_waccess(w_path, 4) == 0) {
    return true;
  }
  else {
    return false;
  }
}

// A function to determine if a file exists on the filesystem
bool file_exists(const char *path)
{
  WCHAR w_path[MAX_PATH_CHARS + 1];
  convert_utf8_string(w_path, path, sizeof(w_path));
  return w_file_exists(w_path);
}

// A function to determine if a directory exists on the filesystem
bool directory_exists(const char *path)
{
  WCHAR w_path[MAX_PATH_CHARS + 1];
  convert_utf8_string(w_path, path, sizeof(w_path));
  if (!w_file_exists(w_path)) {
    return false;
  }
  else {
    DWORD attributes = GetFileAttributesW(w_path);
    if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
      return true;
    }
    else {
      return false;
    }
  }
}

// A function that parses the command string into a file and parameters
static void parse_command(char *cmd, LPWSTR w_file, LPWSTR *w_params)
{
  char *start = NULL;
  char *quote_begin = NULL;
  char *quote_end = NULL;
  char *p = cmd;
  w_file[0] = L'\0';

  // Skip any whitespace at beginning of command
  while (*p == ' ') {
    p++;
  }
  start = p;

  // Check for quote, in which case ignore spaces until the end quote is detecetd
  if (*p == '"') {
    quote_begin = p;
  }

  while (*p != '\0') {
    // If the quote is complete, copy the file
    if (*p == '"' && p != quote_begin) {
      quote_end = p;
      *p = '\0';
      convert_utf8_string(w_file, quote_begin + 1, sizeof(WCHAR)*(MAX_PATH_CHARS + 1));
    }

    else if (*p == ' ') {
      // If a space was detected but there hasn't been a quote detected yet, this is the end of the file
      if (!quote_begin) {
        *p = '\0';
        convert_utf8_string(w_file, start, sizeof(WCHAR)*(MAX_PATH_CHARS + 1));
        p++;

        // Skip any preceding white space for parameters
        while (*p == ' ') {
          p++;
        }

        // Copy parameters
        if (*p != '\0') {
          convert_utf8_alloc(w_params, p);
          break;
        }
      }

      // If a space was detected after the quote
      else if (quote_begin && quote_end) {
        // Skip any preceding white space for parameters
        while (*p == ' ') {
          p++;
        }

        // Copy rest of command as parameters
        if (*p != '\0') {
          convert_utf8_alloc(w_params, p);
        }
        break;
      }
    }
    p++;
  }

  // If there were no quotes or spaces, copy whole command into file buffer
  if (start && w_file[0] == L'\0') {
    convert_utf8_string(w_file, start, sizeof(WCHAR)*(MAX_PATH_CHARS + 1));
  }
}

// A function to determine if a process of a given name is running on the system
static bool process_running_name(LPCWSTR w_target_process)
{
  static Uint32 ticks = 0;
  Uint32 current_ticks = SDL_GetTicks();

  // Only check the process if we've exceeded the process check interval
  if (current_ticks - ticks < BROWSER_CHECK_PERIOD) {
    return true;
  }
  else {
    ticks = current_ticks;

    // Generate an array of process IDs
    DWORD processes[1024], needed, num_processes;
    if (!EnumProcesses(processes, sizeof(processes), &needed)) {
      return false;
    }
    num_processes = needed / sizeof(DWORD);
    WCHAR w_current_process[MAX_PATH + 1];
    HANDLE process = NULL;
    HMODULE module = NULL;
    DWORD flags = PROCESS_QUERY_INFORMATION | PROCESS_VM_READ;

    // Get the image name of each running process
    for (int i = 0; i < num_processes; i++) {
      process = OpenProcess(flags, FALSE, processes[i]);
      if (process != NULL && 
      EnumProcessModulesEx(process, &module, sizeof(module), &needed, LIST_MODULES_ALL)) {
        GetModuleBaseNameW(process, module, w_current_process, sizeof(w_current_process) / sizeof(WCHAR));
        
        // Check if the target process's name is the same as the current process
        if (!wcscmp(w_current_process, w_target_process)) {
          CloseHandle(process);
          return true;
        }
        CloseHandle(process);
      }
    }
  return false;
  }
}

// A function to determine if the previously launched process is still running
bool process_running()
{
  if (web_browser) {
    return process_running_name(w_process_basename);
  }
  else {
    DWORD status = WaitForSingleObject(child_process, 0);
    if (status == WAIT_OBJECT_0) {
      return false;
    }
    else {
      return true;
    }
  }
}

// A function to launch an application
bool start_process(char *cmd, bool check_result)
{
  WCHAR w_file[MAX_PATH_CHARS + 1];
  LPWSTR w_params = NULL;
  
  // Parse command into file and parameters strings
  parse_command(cmd, w_file, &w_params);

  // Set up info struct
  SHELLEXECUTEINFOW info = {
    .cbSize = sizeof(SHELLEXECUTEINFOW),
    .fMask = SEE_MASK_NOCLOSEPROCESS,
    .hwnd = NULL,
    .lpVerb = L"open",
    .lpFile = w_file,
    .lpParameters = w_params,
    .lpDirectory = NULL,
    .nShow = SW_SHOWMAXIMIZED,
    .lpIDList = NULL,
    .lpClass = NULL,  
  };

  BOOL successful = ShellExecuteExW(&info);
  if (!check_result) return true;
  
  if (successful) {
    child_process = info.hProcess;

    // Check if launched application is a web browser
    WCHAR w_process_name[MAX_PATH_CHARS + 1];
    DWORD ret = GetProcessImageFileNameW(child_process, w_process_name, sizeof(w_process_name));
    if (ret) {
      w_process_basename = path_basename(w_process_name);
      web_browser = is_browser(w_process_basename);
    }

    // Go down in the window stack so the launched application can take focus
    if (config.on_launch == MODE_ON_LAUNCH_NONE || config.on_launch == MODE_ON_LAUNCH_BLANK) {
      HWND hwnd = wm_info.info.win.window;
      SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOREDRAW | SWP_NOSIZE | SWP_NOMOVE);
    }
  }
  else {
    output_log(LOGLEVEL_DEBUG, "Failed to launch command\n");
    free(w_params);
    return false;
  }
  free(w_params);
  return true;
}

// A function to scan the slideshow directory for image files
int scan_slideshow_directory(slideshow_t *slideshow, const char *directory)
{
  WIN32_FIND_DATAW data;
  HANDLE handle;
  char file_search[MAX_PATH_CHARS + 1];
  WCHAR w_file_search[MAX_PATH_CHARS + 1];
  char file_result[MAX_PATH_UTF8_CONVERT + 1];
  char file_output[sizeof(file_result)];
  char extension[10];
  int num_images = 0;

  // Generate a UTF-16 wildcard file search string for all supported image file extensions
  for (int i = 0; i < NUM_IMAGE_EXTENSIONS && num_images < MAX_SLIDESHOW_IMAGES; i++) {
    copy_string(extension, "*", sizeof(extension));
    strcat(extension, extensions[i]);
    join_paths(file_search, sizeof(file_search), 2, directory, extension);
    convert_utf8_string(w_file_search, file_search, sizeof(w_file_search));

    // Convert every search result to back to UTF-8, store in slideshow struct
    handle = FindFirstFileW(w_file_search, &data);
    if (handle != INVALID_HANDLE_VALUE) {
      do {
        convert_utf16_string(file_result, data.cFileName, sizeof(file_result));
        join_paths(file_output, sizeof(file_output), 2, directory, file_result);
        copy_string_alloc(&slideshow->images[num_images], file_output);
        num_images++;
      } while (FindNextFileW(handle, &data) != 0 && num_images < MAX_SLIDESHOW_IMAGES);
    }
  }
  slideshow->num_images = num_images;
  return num_images;
}

// A function to get the 2 letter region code
void get_region(char *buffer)
{
  WCHAR w_region[3];
  GetUserDefaultGeoName(w_region, sizeof(w_region));
  convert_utf16_string(buffer, w_region, 3);
}

// A function to convert command line arguments from UTF-16 to UTF-8
void convert_args(int *argc, char **argv[])
{
  LPWSTR command_line = GetCommandLineW();
  LPWSTR  *w_argv = CommandLineToArgvW(command_line, argc);
  int array_length = *argc;
  char **arg_list = malloc(sizeof(char*)*array_length);
  for (int i = 0; i < array_length; i++) {
    convert_utf16_alloc(arg_list + i, w_argv[i]);
  }
  LocalFree(w_argv);
  *argv = arg_list;
}

// A function to free the heap-allocated command line arguments
void cleanup_args(int argc, char *argv[])
{
  for (int i = 0; i < argc; i++) {
    free(argv[i]);
  }
  free(argv);
}