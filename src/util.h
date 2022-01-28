#define NO_ERROR 0
#define NO_ERROR_QUIT 1
#define ERROR_QUIT 2 
#define MAX_LOG_LINE_BYTES 501
#define MAX_PATH_CHARS 1001 //250 wide characters

#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

int handle_arguments(int argc, char *argv[], char **config_file_path);
int config_handler(void *user, const char *section, const char *name, const char *value);
int convert_percent(const char *string, int max_value);
int utf8_length(const char *string);
unsigned int calculate_width(int buttons, int icon_spacing, int icon_size, int highlight_padding);
bool hex_to_color(const char *string, SDL_Color *color);
bool convert_bool(const char *string, bool default_setting);
char *join_paths(char *buffer, int num_paths, ...);
char *find_file(const char *file, int num_prefixes, const char **prefixes);
void copy_string(char **dest, const char *string);
void utf8_truncate(char *string, int width, int max_width);
void add_hotkey(hotkey_t **first_hotkey, const char *keycode, const char *cmd);
void add_gamepad_control(int type, int index, const char *label, const char *cmd);
void random_array(int *array, int array_size);
void print_usage(void);
void print_version(void);
void clean_path(char *path);
void validate_settings(geometry_t *geo);
menu_t *get_menu(char *menu_name, menu_t *first_menu);
menu_t *create_menu(char *menu_name, int *num_menus);
entry_t *advance_entries(entry_t *entry, int spaces, mode direction);
time_format_t get_time_format(const char *region);