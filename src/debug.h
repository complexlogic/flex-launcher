typedef enum {
    LOGLEVEL_DEBUG = 0,
    LOGLEVEL_ERROR,
    LOGLEVEL_FATAL
} LogLevel;

static int init_log(void);
void output_log(LogLevel log_level, const char *format, ...);
void print_compiler_info(FILE *stream);
void debug_video(SDL_Renderer *renderer, SDL_DisplayMode *display_mode);
void debug_settings(void);
void debug_gamepad(GamepadControl *gamepad_controls);
void debug_hotkeys(Hotkey *hotkeys);
void debug_menu_entries(Menu *first_menu, int num_menus);
void debug_slideshow(Slideshow *slideshow);
void debug_button_positions(Entry *entry, Menu *current_menu, Geometry *geo);

#ifdef _WIN32
#define endline "\r\n"
#else
#define endline "\n"
#endif

#define log_debug(msg, ...) output_log(LOGLEVEL_DEBUG, msg endline, ##__VA_ARGS__)
#define log_error(msg, ...) output_log(LOGLEVEL_ERROR, "Error: " msg endline, ##__VA_ARGS__)
#define log_fatal(msg, ...) output_log(LOGLEVEL_FATAL, "Fatal Error: " msg endline, ##__VA_ARGS__)
