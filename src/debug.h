typedef enum {
    LOGLEVEL_DEBUG = 0,
    LOGLEVEL_ERROR,
    LOGLEVEL_FATAL
} LogLevel;

void output_log(LogLevel log_level, const char *format, ...);
void print_compiler_info(FILE *stream);
void debug_video(SDL_Renderer *renderer, SDL_DisplayMode *display_mode);
void debug_settings(void);
void debug_gamepad(GamepadControl *gamepad_controls);
void debug_hotkeys(Hotkey *hotkeys);
void debug_menu_entries(Menu *first_menu, size_t num_menus);
void debug_slideshow(Slideshow *slideshow);
void debug_button_positions(Entry *entry, Menu *current_menu, Geometry *geo);

#ifdef _WIN32
#define endline "\r\n"
#else
#define endline "\n"
#endif

#define log_debug(msg, ...) output_log(LOGLEVEL_DEBUG, msg endline, ##__VA_ARGS__)
#define log_error(msg, ...) output_log(LOGLEVEL_ERROR, "" msg endline, ##__VA_ARGS__)
#define log_fatal(msg, ...) output_log(LOGLEVEL_FATAL, "" msg endline, ##__VA_ARGS__)

#define DEBUG_COLOR(setting_name, color) log_debug("%-25s #%.2X%.2X%.2X%.2X", setting_name ":", color.r, color.g, color.b, color.a)
#define DEBUG_MODE(setting_name, type, value) log_debug("%-25s %s", setting_name ":", get_mode_setting(type, value))
#define DEBUG_BOOL(setting_name, value) log_debug("%-25s %s", setting_name ":", value ? "true" : "false");
#define DEBUG_STR(setting_name, value) log_debug("%-25s %s", setting_name ":", value != NULL ? value : "(null)")
#define DEBUG_INT(setting_name, value) log_debug("%-25s %i", setting_name ":", value)
#define DEBUG_FLOAT(setting_name, value) log_debug("%-25s %.2f", setting_name ":", value)
