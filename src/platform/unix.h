#define EXT_DESKTOP ".desktop"
#define DELIMITER_ACTION ";"
#define DESKTOP_SECTION_HEADER "Desktop Entry"
#define DESKTOP_SECTION_HEADER_ACTION "Desktop Action %s"
#define KEY_EXEC "Exec"
#define MAX_INI_SECTION 100

typedef struct {
  char section[MAX_INI_SECTION + 1];
  char *exec;
} desktop_t;

static int desktop_handler(void *user, const char *section, const char *name, const char *value);
static void strip_field_codes(char *cmd);
static bool ends_with(const char *string, const char *phrase);
static int image_filter(struct dirent *file);
