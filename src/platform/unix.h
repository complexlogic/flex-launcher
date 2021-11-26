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

bool file_exists(char *path);
int parse_desktop_file(char *command, char **exec);