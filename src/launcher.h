// Color masking bit logic
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define RMASK 0xff000000
#define GMASK 0x00ff0000
#define BMASK 0x0000ff00
#define AMASK 0x000000ff
#else
#define RMASK 0x000000ff
#define GMASK 0x0000ff00
#define BMASK 0x00ff0000
#define AMASK 0xff000000
#endif
#define COLOR_MASKS RMASK, GMASK, BMASK, AMASK

// Definitions
#define MIN_ICON_SIZE 32
#define MAX_ICON_SIZE 1024
#define MIN_RX_SIZE 0
#define MAX_RX_SIZE 100
#define PERCENT_MAX_CHARS 10
#define GAMEPAD_DEADZONE 10000
#define GAMEPAD_REPEAT_DELAY 500
#define GAMEPAD_REPEAT_INTERVAL 25
#define CLOCK_UPDATE_PERIOD 1000
#define SCROLL_INDICATOR_HEIGHT 0.1F
#define SCREEN_MARGIN 0.06F
#define MAX_CLOCK_MARGIN 0.1F
#define MIN_BUTTON_CENTERLINE 0.25F
#define MAX_BUTTON_CENTERLINE 0.75F
#define MAX_SLIDESHOW_IMAGES 250
#define MIN_SLIDESHOW_IMAGE_DURATION 5000
#define MAX_SLIDESHOW_IMAGE_DURATION 600000
#define MIN_SLIDESHOW_TRANSITION_TIME 0
#define MAX_SLIDESHOW_TRANSITION_TIME 3000
#define MIN_SCREENSAVER_IDLE_TIME 3
#define MAX_SCREENSAVER_IDLE_TIME 900
#define SCREENSAVER_TRANSITION_TIME 1500
#define APPLICATION_WAIT_PERIOD 100

// Modes
typedef int mode;
#define MODE_COLOR 0
#define MODE_IMAGE 1
#define MODE_SLIDESHOW 2
#define MODE_TEXT_TRUNCATE 0
#define MODE_TEXT_SHRINK 1
#define MODE_TEXT_NONE 2
#define DIRECTION_LEFT 0
#define DIRECTION_RIGHT 1
#define MODE_ON_LAUNCH_HIDE 0
#define MODE_ON_LAUNCH_NONE 1
#define MODE_ON_LAUNCH_BLANK 2

// Types
#define TYPE_BUTTON 0
#define TYPE_AXIS_POS 1
#define TYPE_AXIS_NEG 2

// Special commands
#define SCMD_SELECT ":select"
#define SCMD_SUBMENU ":submenu"
#define SCMD_FORK ":fork"
#define SCMD_EXIT ":exit"
#define SCMD_LEFT ":left"
#define SCMD_RIGHT ":right"
#define SCMD_HOME ":home"
#define SCMD_BACK ":back"
#define SCMD_QUIT ":quit"
#define SCMD_SHUTDOWN ":shutdown"
#define SCMD_RESTART ":restart"
#define SCMD_SLEEP ":sleep"

typedef enum {
  ALIGNMENT_LEFT,
  ALIGNMENT_RIGHT,
} launcher_alignment_t;

typedef struct {
  bool slideshow_transition;
  bool slideshow_background_rendering;
  bool slideshow_background_ready;
  bool slideshow_paused;
  bool screensaver_active;
  bool screensaver_transition;
  bool clock_rendering;
  bool clock_ready;
} state_t;

typedef struct {
  Uint32 main;
  Uint32 slideshow_load;
  Uint32 last_input;
  Uint32 clock_update;
  Uint32 application_exited;
} ticks_t;

// Linked list for menu entries
typedef struct entry {
  char         *title;
  char         *icon_path;
  char         *cmd;
  SDL_Texture  *icon;
  SDL_Rect     icon_rect;
  SDL_Texture  *title_texture;
  SDL_Rect     text_rect;
  int          title_offset;
  struct entry *next;
  struct entry *previous;
} entry_t;

// Linked list for menus
typedef struct menu {
  char        *name;
  int         num_entries;
  bool        rendered;
  int         page;
  int         highlight_position;
  entry_t     *first_entry;
  entry_t     *root_entry;
  entry_t     *last_selected_entry;
  struct menu *next;
  struct menu *back;
} menu_t;

typedef struct hotkey {
  SDL_Keycode   keycode;
  char          *cmd;
  struct hotkey *next;
} hotkey_t;

// Struct for the geometry parameters of the onscreen buttons
typedef struct {
  int screen_width;
  int screen_height;
  int screen_margin;
  int font_height;
  int x_margin; // Distance between left edge of screen and x coordinate of root_entry icon
  int y_margin; // Distance between top edge of screen and y coordinate of all entry icons
  int x_advance; // Distance between icon x coordinate of adjacent entries
  int num_buttons; // Number of buttons shown on the screen
} geometry_t;

// Struct for highlight 
typedef struct {
  SDL_Texture *texture;
  SDL_Rect rect;
} highlight_t;

//Struct for scroll indicators
typedef struct {
  SDL_Texture *texture;
  SDL_Rect rect_right;
  SDL_Rect rect_left;
} scroll_t;

// Linked list of gamepad controls
typedef struct gamepad_control {
  int                    type;
  int                    index;
  int                    repeat;
  char                   *label;
  char                   *cmd;
  struct gamepad_control *next;
} gamepad_control_t;

// Slideshow
typedef struct {
  char *images[MAX_SLIDESHOW_IMAGES];
  int order[MAX_SLIDESHOW_IMAGES];
  int i;
  int num_images;
  float transition_alpha;
  float transition_change_rate;
  SDL_Surface *transition_surface;
  SDL_Texture *transition_texture;
} slideshow_t;

// Screensaver
typedef struct {
  float alpha;
  float alpha_end_value;
  float transition_change_rate;
  SDL_Texture *texture;
} screensaver_t;

typedef enum {
  FORMAT_TIME_AUTO,
  FORMAT_TIME_12HR,
  FORMAT_TIME_24HR
} time_format_t;

typedef enum {
  FORMAT_DATE_AUTO,
  FORMAT_DATE_LITTLE,
  FORMAT_DATE_BIG
} date_format_t;

// Configuration settings
typedef struct {
  char *default_menu;
  unsigned int max_buttons;
  mode background_mode; // Defines image or color background mode
  SDL_Color background_color; // Background color
  char *background_image; // Path to background image
  char *slideshow_directory;
  Uint16 icon_size;
  int icon_spacing;
  char icon_spacing_str[PERCENT_MAX_CHARS];
  char *title_font_path; // Path to title TTF font file
  unsigned int title_font_size;
  SDL_Color title_font_color; // Color struct for title text
  int title_font_outline_size;
  SDL_Color title_font_outline_color;
  char title_opacity[PERCENT_MAX_CHARS];
  mode title_oversize_mode; 
  unsigned int title_padding;  
  SDL_Color highlight_color;
  char highlight_opacity[PERCENT_MAX_CHARS];
  unsigned int highlight_rx;
  int highlight_vpadding;
  int highlight_hpadding;
  char button_centerline[PERCENT_MAX_CHARS];
  bool scroll_indicators;
  SDL_Color scroll_indicator_color;
  char scroll_indicator_opacity[PERCENT_MAX_CHARS];
  bool reset_on_back;
  bool mouse_select;
  mode on_launch;
  bool screensaver_enabled;
  Uint32 screensaver_idle_time;
  char screensaver_intensity_str[PERCENT_MAX_CHARS];
  bool screensaver_pause_slideshow;
  bool gamepad_enabled;
  int gamepad_device;
  char *gamepad_mappings_file;
  bool debug;
  char *exe_path;
  menu_t *first_menu;
  int num_menus;
  bool clock_enabled;
  bool clock_show_date;
  launcher_alignment_t clock_alignment;
  char *clock_font_path;
  char clock_margin_str[PERCENT_MAX_CHARS];
  int clock_margin;
  SDL_Color clock_font_color;
  char clock_opacity[PERCENT_MAX_CHARS];
  unsigned int clock_font_size;
  int clock_font_outline_size;
  SDL_Color clock_font_outline_color;
  time_format_t clock_time_format;
  date_format_t clock_date_format;
  bool clock_include_weekday;
  Uint32 slideshow_image_duration;
  Uint32 slideshow_transition_time;
} config_t;

// Function prototypes
static int init_sdl(void);
static int init_ttf(void);
static int load_menu(menu_t *menu, bool set_back_menu, bool reset_position);
static int load_menu_by_name(const char *menu_name, bool set_back_menu, bool reset_position);
static void update_slideshow(void);
static void resume_slideshow(void);
static void update_screensaver(void);
static void update_clock(bool block);
static void init_slideshow(void);
static void init_screensaver(void);
void quit_slideshow(void);
void set_draw_color(void);
static void calculate_button_geometry(entry_t *entry, int buttons);
static void render_buttons(menu_t *menu);
static void draw_buttons(entry_t *entry);
static void move_left(void);
static void move_right(void);
static void load_submenu(const char *submenu);
static void load_back_menu(menu_t *menu);
static void draw_screen(void);
static void handle_keypress(SDL_Keysym *key);
static void execute_command(const char *command);
static void launch_application(char *cmd);
static void poll_gamepad(void);
static void connect_gamepad(int device_index);
static void render_scroll_indicators(void);
void quit(int status);
static void cleanup(void);