bool file_exists(const char *path);
bool file_exists_w(wchar_t *path);
char *convert_cmd(char *cmd);
void hide_console(void);
void restore_console(void);
int scan_slideshow_directory(slideshow_t* slideshow, const char* directory);

#define CMD_SHUTDOWN "\"shutdown /s /f /t 0 \""
#define CMD_RESTART "\"shutdown /r /f /t 0 \""
#define CMD_SLEEP "\"rundll32 powrprof.dll,SetSuspendState 0,1,0 \""