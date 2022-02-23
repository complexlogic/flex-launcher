#ifdef _WIN32
#define CMD_SHUTDOWN "\"shutdown /s /f /t 0 \""
#define CMD_RESTART "\"shutdown /r /f /t 0 \""
#define CMD_SLEEP "\"rundll32 powrprof.dll,SetSuspendState 0,1,0 \""
#else
#define CMD_SHUTDOWN "systemctl poweroff"
#define CMD_RESTART "systemctl reboot"
#define CMD_SLEEP "systemctl suspend"
#endif

// Abstracted platform function prototypes
bool file_exists(const char *path);
bool directory_exists(const char *path);
void get_region(char *buffer);
int scan_slideshow_directory(slideshow_t *slideshow, const char *directory);
bool start_process(char *cmd);
bool process_running();


// Windows-specfic function prototypes
#ifdef _WIN32
void convert_args(int *argc, char **argv[]);
void cleanup_args(int argc, char *argv[]);
#endif

// Linux-specific function prototypes
#ifdef __unix__
void print_usage(void);
void print_version(void);
#endif