#define MAX_PATH_UTF8_CONVERT 2*sizeof(WCHAR)*MAX_PATH_CHARS
#define BROWSER_CHECK_PERIOD 1000

//Function prototypes

static void parse_command(char *cmd, char *file, size_t file_size, char **params);
static char *path_basename(const char *path);
static bool process_running_name(const char *target_process);
static bool is_browser(const char *exe_basename);
