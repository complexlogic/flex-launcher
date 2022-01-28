typedef enum {
	LOGLEVEL_DEBUG = 0,
	LOGLEVEL_ERROR,
	LOGLEVEL_FATAL
} log_level_t;

static int init_log(void);
void output_log(log_level_t log_level, const char *format, ...);
void debug_video(SDL_Renderer *renderer, SDL_DisplayMode *display_mode);
void debug_settings(void);
void debug_hotkeys(hotkey_t *hotkeys);
void debug_menu_entries(menu_t *first_menu, int num_menus);
void debug_slideshow(slideshow_t *slideshow);
void debug_button_positions(entry_t *entry, menu_t *current_menu, geometry_t *geo);
