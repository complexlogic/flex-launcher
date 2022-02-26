#ifdef _WIN32
#define CMD_SHUTDOWN "\"shutdown /s /f /t 0 \""
#define CMD_RESTART "\"shutdown /r /f /t 0 \""
#define CMD_SLEEP "\"rundll32 powrprof.dll,SetSuspendState 0,1,0 \""
#define FILE_MODE_WRITE "wt"
#else
#define CMD_SHUTDOWN "systemctl poweroff"
#define CMD_RESTART "systemctl reboot"
#define CMD_SLEEP "systemctl suspend"
#define FILE_MODE_WRITE "w"
#endif

// Abstracted platform function prototypes
bool file_exists(const char *path);
bool directory_exists(const char *path);
void get_region(char *buffer);
int scan_slideshow_directory(slideshow_t *slideshow, const char *directory);
bool start_process(char *cmd, bool application);
bool process_running();

// Linux-specific function prototypes
#ifdef __unix__
void make_directory(const char *directory);
void print_usage(void);
void print_version(void);
#endif

// Windows-specific function prototypes
#ifdef _WIN32
bool has_exit_hotkey(void);
void set_exit_hotkey(SDL_Keycode keycode);
void register_exit_hotkey(void);
void check_exit_hotkey(SDL_SysWMmsg* msg);
#endif