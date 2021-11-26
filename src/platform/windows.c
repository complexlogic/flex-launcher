#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool file_exists(char *path)
{
  if(_access(path,4) == 0) {
    return true;
  }
  else {
    return false;  
  }
}

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