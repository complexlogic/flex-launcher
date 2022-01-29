#define MAX_PATH_UTF8_CONVERT 2*sizeof(WCHAR)*MAX_PATH_CHARS
#define BROWSER_CHECK_PERIOD 1000

//Function prototypes
static void convert_utf8_string(LPWSTR w_string, const char *string, int buffer_size);
static void convert_utf16_string(char *string, LPCWSTR w_string, int buffer_size);
static void convert_utf8_alloc(LPWSTR *buffer, const char *string);
static void convert_utf16_alloc(const char **buffer, LPCWSTR w_string);
static void parse_command(char *cmd, LPWSTR w_file, LPWSTR *w_params);
static LPWSTR path_basename(LPCWSTR w_path);
static bool process_running_name(LPCWSTR w_target_process);
static bool w_file_exists(LPCWSTR w_path);
static bool is_browser(LPCWSTR w_exe_basename);
bool process_running(HANDLE process);
