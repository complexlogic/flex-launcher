#define EXT_DESKTOP ".desktop"
#define MAX_INI_SECTION 100
#define DELIMITER_ACTION ";"
#define DESKTOP_SECTION_HEADER "Desktop Entry"
#define DESKTOP_SECTION_HEADER_ACTION "Desktop Action %s"
#define KEY_EXEC "Exec"

typedef struct {
  char section[MAX_INI_SECTION + 1];
  char *exec;
} desktop_t;

static int desktop_handler(void *user, const char *section, const char *name, const char *value);
void make_directory(const char *directory);
static void strip_field_codes(char *cmd);
static int image_filter(struct dirent *file);
