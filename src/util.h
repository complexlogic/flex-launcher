int handle_arguments(int argc, char *argv[], char **config_file_path, config_t *config);
int config_handler(void* user, const char* section, const char* name, const char* value);
int convert_percent(char *string, int max_value);
int utf8_length(char *string);
bool hex_to_color(char *text, SDL_Color *color, int bits);
bool convert_bool(char *string, bool default_setting);
char *join_path(int num_paths, ...);
char *find_file(char *file, int num_prefixes, char **prefixes);
void copy_string(char *string, char **ptr);
void utf8_truncate(char *string, int width, int max_width);
void add_gamepad_control(int type, int index, char *label, char *cmd, config_t *config);
void print_usage();
void print_version();
void debug_settings(config_t *config);
void debug_menu_entries(menu_t *first_menu, int num_menus);
void debug_button_positions(entry_t *entry, menu_t *current_menu, geometry_t *geo);
void clean_path(char *path);
menu_t *get_menu(char *menu_name, menu_t *first_menu);
menu_t *create_menu(char *menu_name, int *num_menus);

#define NO_ERROR 0
#define NO_ERROR_QUIT 1
#define ERROR_QUIT 2 