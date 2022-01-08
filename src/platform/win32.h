bool file_exists(const char *path);
bool directory_exists(const char *path);
void launch_application(char *cmd);
int scan_slideshow_directory(slideshow_t *slideshow, const char *directory);

#define MAX_PATH_UTF8_CONVERT 2*sizeof(WCHAR)*MAX_PATH_CHARS
#define NUM_BROWSERS 3
#define BROWSER_CHECK_PERIOD 1000
#define CMD_SHUTDOWN "\"shutdown /s /f /t 0 \""
#define CMD_RESTART "\"shutdown /r /f /t 0 \""
#define CMD_SLEEP "\"rundll32 powrprof.dll,SetSuspendState 0,1,0 \""