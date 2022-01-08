#define CMD_SHUTDOWN "systemctl poweroff"
#define CMD_RESTART "systemctl reboot"
#define CMD_SLEEP "systemctl suspend"
#define DESKTOP_SUCCESS 0
#define DESKTOP_ERROR 1
#define DESKTOP_NOT_FOUND 2
#define EXT_DESKTOP ".desktop"
#define EXT_DESKTOP_LENGTH 8
#define DELIMITER_ACTION "|"
#define DESKTOP_SECTION_HEADER "Desktop Entry"
#define DESKTOP_SECTION_HEADER_LENGTH 13
#define DESKTOP_SECTION_HEADER_ACTION "Desktop Action %s"
#define DESKTOP_SECTION_HEADER_ACTION_LENGTH 15
#define EXEC "Exec"

typedef struct {
  char *section;
  char **exec;
} desktop_t;


bool file_exists(const char *path);
bool directory_exists(const char *path);
int parse_desktop_file(char *command, char **exec);
static int desktop_handler(void *user, const char *section, const char *name, const char *value);
void make_directory(const char *directory);
static void strip_field_codes(char *cmd);
void launch_application(char *cmd);
static int image_filter(struct dirent *file);
int scan_slideshow_directory(slideshow_t *slideshow, const char *directory);