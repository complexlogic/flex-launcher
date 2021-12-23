typedef enum {
  LOGLEVEL_DEBUG = 0,
  LOGLEVEL_ERROR,
  LOGLEVEL_FATAL
} log_level_t;

#define NO_ERROR 0
#define NO_ERROR_QUIT 1
#define ERROR_QUIT 2 
#define MAX_LOG_LINE_BYTES 501
#define MAX_PATH_LENGTH 512
#define MAX_PATH_BYTES 1001 //250 wide characters

int handle_arguments(int argc, char *argv[], char **config_file_path);
int config_handler(void* user, const char* section, const char* name, const char* value);
int convert_percent(char *string, int max_value);
int utf8_length(char *string);
int init_log();
bool hex_to_color(char *text, SDL_Color *color);
bool convert_bool(char *string, bool default_setting);
char *join_paths(char *buffer, int num_paths, ...);
char *find_file(char *file, int num_prefixes, char **prefixes);
void copy_string(char *string, char **ptr);
void utf8_truncate(char *string, int width, int max_width);
void add_gamepad_control(int type, int index, char *label, char *cmd);
void print_usage();
void print_version();
void debug_settings();
void debug_menu_entries(menu_t *first_menu, int num_menus);
void debug_button_positions(entry_t *entry, menu_t *current_menu, geometry_t *geo);
void clean_path(char *path);
void output_log(log_level_t log_level, const char *format, ...);
menu_t *get_menu(char *menu_name, menu_t *first_menu);
menu_t *create_menu(char *menu_name, int *num_menus);