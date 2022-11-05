#define MAX_LOG_LINE_BYTES 501
#define MAX_PATH_CHARS 1001 //250 wide characters
#define INVALID_PERCENT_VALUE -1

#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

#define SELECTED_SUFFIX "_selected"
#define LEN(x) ((sizeof(x)/sizeof(x[0])) - sizeof(x[0]))
#define MATCH(x, y) !strcmp(x, y)

struct gamepad_info {
    const char *label;
    int type;
    int index;
};

int config_handler(void *user, const char *section, const char *name, const char *value);
int convert_percent(const char *string, int max_value);
int utf8_length(const char *string);
unsigned int calculate_width(int buttons, int icon_spacing, int icon_size, int highlight_padding);
bool hex_to_color(const char *string, SDL_Color *color);
bool convert_bool(const char *string, bool *setting);
bool is_percent(const char *string);
static bool ends_with(const char *string, const char *phrase);
char *selected_path(const char *path);
char *join_paths(char *buffer, size_t bytes, int num_paths, ...);
char *find_file(const char *file, int num_prefixes, const char **prefixes);
void handle_arguments(int argc, char *argv[], char **config_file_path);
void copy_string(char* dest, const char* string, size_t size);
void copy_string_alloc(char **dest, const char *string);
void utf8_truncate(char *string, int width, int max_width);
void convert_percent_to_int(char *string, int *result, int max_value);
void add_hotkey(const char *keycode, const char *cmd);
static void add_gamepad_control(const char *label, const char *cmd);
void random_array(int *array, int array_size);
void clean_path(char *path);
void validate_settings(Geometry *geo);
void parse_config_file(const char *config_file_path);
void read_file(const char *path, char **buffer);
void sprintf_alloc(char **buffer, const char *format, ...);
Uint16 get_unicode_code_point(const char *p, int *bytes);
Menu *get_menu(char *menu_name);
Menu *create_menu(char *menu_name, int *num_menus);
Entry *advance_entries(Entry *entry, int spaces, Direction direction);