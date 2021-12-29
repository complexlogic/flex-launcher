#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define _WIN32_WINNT 0x0601
#include <windows.h>
#include <stringapiset.h>
#include <fileapi.h>
#include <SDL.h>
#include <launcher.h>
#include "win32.h"
#include "../util.h"

#define NUM_EXTENSIONS 4
const char* extensions[NUM_EXTENSIONS] = { ".jpg", ".jpeg", ".png", ".webp" };

// A function to determine if a file exists in the filesystem

bool file_exists_w(wchar_t *path)
{
  if (_waccess(path, 4) == 0) {
    return true;
  }
  else {
    return false;
  }
}

bool file_exists(const char *path)
{
  WCHAR w_path[2*MAX_PATH_BYTES + 1];
  MultiByteToWideChar(CP_UTF8,
    0,
    path,
    -1,
    w_path,
    sizeof(w_path));
  return file_exists_w(w_path);
}

bool directory_exists(const char *path)
{
  WCHAR w_path[2*MAX_PATH_BYTES + 1];
  MultiByteToWideChar(CP_UTF8,
    0,
    path,
    -1,
    w_path,
    sizeof(w_path));
  if (!file_exists_w(w_path)) {
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

// A function to put double quotes around the command for the system call
char *convert_cmd(char *cmd)
{
  int cmd_length = strlen(cmd);
  char *cmd_output = malloc(sizeof(char) * cmd_length + 3);
  *cmd_output = '"';
  strcpy(cmd_output + 1, cmd);
  *(cmd_output + cmd_length + 1) = '"';
  *(cmd_output + cmd_length + 2) = '\0';
  free(cmd);
  return cmd_output;
}

// A function to hide the console window
void hide_console()
{
  HWND console = GetConsoleWindow();
  ShowWindow(console, SW_SHOWMINNOACTIVE);
}

// A function to restore the console window
void restore_console()
{
  HWND console = GetConsoleWindow();
  ShowWindow(console, SW_RESTORE);
}

int scan_slideshow_directory(slideshow_t* slideshow, const char* directory)
{
  WIN32_FIND_DATAW data;
  HANDLE handle;
  char file_search_utf8[MAX_PATH_BYTES];
  WCHAR file_search[2*MAX_PATH_BYTES + 1];
  char file_result_utf8[2*sizeof(file_search)];
  char file_output[sizeof(file_result_utf8)];
  char extension[10];
  int num_images = 0;

  // Generate a UTF-16 wildcard file search string for all supported image file extensions
  for (int i = 0; i < NUM_EXTENSIONS && num_images < MAX_SLIDESHOW_IMAGES; i++) {
    strcpy(extension, "*");
    strcat(extension, extensions[i]);
    join_paths(file_search_utf8, 2, directory, extension);
    MultiByteToWideChar(CP_UTF8, 
      0, 
      file_search_utf8, 
      -1, 
      file_search, 
      sizeof(file_search));

    // Convert every search result to back to UTF-8, store in slideshow struct
    handle = FindFirstFileW(file_search, &data);
    if (handle != INVALID_HANDLE_VALUE) {
      do {
        WideCharToMultiByte(CP_UTF8, 
          0, 
          data.cFileName, 
          -1, 
          file_result_utf8, 
          sizeof(file_result_utf8), 
          NULL, 
          NULL);
        join_paths(file_output, 2, directory, file_result_utf8);
        copy_string(&slideshow->images[num_images], file_output);
        num_images++;
      } while (FindNextFileW(handle, &data) != 0 && num_images < MAX_SLIDESHOW_IMAGES);
    }
  }
  slideshow->num_images = num_images;
  return num_images;
}