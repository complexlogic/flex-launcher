#include <dirent.h>

#define CMD_SHUTDOWN "systemctl poweroff"
#define CMD_RESTART "systemctl reboot"
#define CMD_SLEEP "systemctl suspend"
#define EXT_DESKTOP ".desktop"
#define DELIMITER_ACTION ";"
#define DESKTOP_SECTION_HEADER "Desktop Entry"
#define DESKTOP_SECTION_HEADER_ACTION "Desktop Action %s"
#define KEY_EXEC "Exec"
#define MAX_INI_SECTION 100

typedef struct {
    char section[MAX_INI_SECTION + 1];
    char *exec;
} Desktop;
