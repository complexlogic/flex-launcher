bool file_exists(char *path);
char* convert_cmd(char *cmd);

#define CMD_SHUTDOWN "\"shutdown /s /f /t 0 \""
#define CMD_RESTART "\"shutdown /r /f /t 0 \""
#define CMD_SLEEP "\"rundll32 powrprof.dll,SetSuspendState 0,1,0 \""