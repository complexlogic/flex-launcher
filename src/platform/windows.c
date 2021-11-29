#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define _WIN32_WINNT 0x0601
#include <windows.h>

// A function to determine if a file exists in the filesystem
bool file_exists(char *path)
{
  if(_access(path,4) == 0) {
    return true;
  }
  else {
    return false;  
  }
}

// A function to put double quotes around the command for the system call
char* convert_cmd(char *cmd)
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