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

// Launcher parameters
#define MIN_FPS_LIMIT 10
#define MIN_ICON_SIZE 32
#define MAX_ICON_SIZE 1024
#define MIN_RX_SIZE 0
#define MAX_RX_SIZE 100
#define PERCENT_MAX_CHARS 10
#define GAMEPAD_DEADZONE 10000
#define GAMEPAD_REPEAT_DELAY 500
#define GAMEPAD_REPEAT_INTERVAL 25
#define CLOCK_UPDATE_PERIOD 1000
#define SCROLL_INDICATOR_HEIGHT 0.11F
#define MAX_SCROLL_INDICATOR_OUTLINE 0.01F
#define SCREEN_MARGIN 0.05F
#define MAX_CLOCK_MARGIN 0.1F
#define MIN_VCENTER 0.25F
#define MAX_VCENTER 0.75F
#define MIN_SLIDESHOW_IMAGE_DURATION 5000
#define MAX_SLIDESHOW_IMAGE_DURATION 3600000
#define MAX_SLIDESHOW_TRANSITION_TIME 3000
#define MIN_SCREENSAVER_IDLE_TIME 3
#define MAX_SCREENSAVER_IDLE_TIME 900
#define SCREENSAVER_TRANSITION_TIME 1500
#define APPLICATION_WAIT_PERIOD 100
#define MIN_APPLICATION_TIMEOUT 3
#define MAX_APPLICATION_TIMEOUT 30

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
    MODE_SETTING_BACKGROUND,
    MODE_SETTING_ON_LAUNCH,
    MODE_SETTING_OVERSIZE,
    MODE_SETTING_ALIGNMENT,
    MODE_SETTING_TIME_FORMAT,
    MODE_SETTING_DATE_FORMAT
} ModeSettingType;

typedef enum {
    BACKGROUND_COLOR,
    BACKGROUND_IMAGE,
    BACKGROUND_SLIDESHOW,
    BACKGROUND_TRANSPARENT
} ModeBackground;

typedef enum {
    ON_LAUNCH_BLANK,
    ON_LAUNCH_NONE,
    ON_LAUNCH_QUIT
} ModeOnLaunch;

typedef enum {
    OVERSIZE_TRUNCATE,
    OVERSIZE_SHRINK,
    OVERSIZE_NONE
} ModeOversize;

typedef enum {
    ALIGNMENT_LEFT,
    ALIGNMENT_RIGHT,
} Alignment;

typedef enum {
    FORMAT_TIME_24HR,
    FORMAT_TIME_12HR,
    FORMAT_TIME_AUTO
} TimeFormat;

typedef enum {
    FORMAT_DATE_BIG,
    FORMAT_DATE_LITTLE,
    FORMAT_DATE_AUTO
} DateFormat;

typedef enum {
    TYPE_BUTTON,
    TYPE_AXIS_POS,
    TYPE_AXIS_NEG,
} ControlType;

typedef enum {
    DIRECTION_LEFT,
    DIRECTION_RIGHT,
} Direction;

// Program states
typedef struct {
    bool application_launching;
    bool application_running;
    bool has_focus;
    bool slideshow_transition;
    bool slideshow_background_rendering;
    bool slideshow_background_ready;
    bool slideshow_paused;
    bool screensaver_active;
    bool screensaver_transition;
    bool clock_rendering;
    bool clock_ready;
} State;

// Timing information
typedef struct {
    Uint32 main;
    Uint32 program_start;
    Uint32 application_launched;
    Uint32 slideshow_load;
    Uint32 last_input;
    Uint32 clock_update;
    Uint32 application_exited;
} Ticks;

// Linked list for menu entries
typedef struct entry {
    char           *title;
    char           *icon_path;
    char           *icon_selected_path;
    char           *cmd;
    SDL_Texture    *icon;
    SDL_Texture    *icon_selected;
    SDL_Rect       icon_rect;
    SDL_Texture    *title_texture;
    SDL_Rect       text_rect;
    int            title_offset;
    struct entry   *next;
    struct entry   *previous;
} Entry;

// Linked list for menus
typedef struct menu {
    char         *name;
    unsigned int num_entries;
    bool         rendered;
    unsigned int page;
    unsigned int highlight_position;
    Entry        *first_entry;
    Entry        *root_entry;
    Entry        *last_selected_entry;
    struct menu  *next;
    struct menu  *back;
} Menu;

typedef struct gamepad {
    SDL_GameController *controller;
    int device_index;
    int id;
    struct gamepad *previous;
    struct gamepad *next;
} Gamepad;

// Linked list of gamepad controls
typedef struct gamepad_control {
    ControlType            type;
    int                    index;
    Uint32                 repeat;
    const char             *label;
    char                   *cmd;
    struct gamepad_control *next;
} GamepadControl;

// Linked list of hotkeys
typedef struct hotkey {
    SDL_Keycode   keycode;
    char          *cmd;
    struct hotkey *next;
} Hotkey;

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
} Geometry;

// Struct for highlight 
typedef struct {
    SDL_Texture *texture;
    SDL_Rect rect;
} Highlight;

//Struct for scroll indicators
typedef struct {
    SDL_Texture *texture;
    SDL_Rect rect_right;
    SDL_Rect rect_left;
} Scroll;

// Slideshow
typedef struct {
    char **images;
    int *order;
    int i;
    int num_images;
    float transition_alpha;
    float transition_change_rate;
    SDL_Surface *transition_surface;
    SDL_Texture *transition_texture;
} Slideshow;

// Screensaver
typedef struct {
    float alpha;
    float alpha_end_value;
    float transition_change_rate;
    SDL_Texture *texture;
} Screensaver;

// Configuration settings
typedef struct {
    char *default_menu;
    unsigned int max_buttons;
    bool vsync;
    int fps_limit;
    Uint32 application_timeout;
    ModeBackground background_mode; // Defines image or color background mode
    SDL_Color background_color; // Background color
    SDL_Color chroma_key_color;
    char *background_image; // Path to background image
    char *slideshow_directory;
    bool background_overlay;
    SDL_Color background_overlay_color;
    char background_overlay_opacity[PERCENT_MAX_CHARS];
    Uint16 icon_size;
    int icon_spacing;
    char icon_spacing_str[PERCENT_MAX_CHARS];
    bool titles_enabled;
    char *title_font_path; // Path to title TTF font file
    unsigned int title_font_size;
    SDL_Color title_font_color; // Color struct for title text
    bool title_shadows;
    SDL_Color title_shadow_color;
    char title_opacity[PERCENT_MAX_CHARS];
    ModeOversize title_oversize_mode; 
    int title_padding;
    bool highlight;
    SDL_Color highlight_fill_color;
    SDL_Color highlight_outline_color;
    int highlight_outline_size;
    char highlight_fill_opacity[PERCENT_MAX_CHARS];
    char highlight_outline_opacity[PERCENT_MAX_CHARS];
    unsigned int highlight_rx;
    int highlight_vpadding;
    int highlight_hpadding;
    char vcenter[PERCENT_MAX_CHARS];
    bool scroll_indicators;
    SDL_Color scroll_indicator_fill_color;
    int scroll_indicator_outline_size;
    SDL_Color scroll_indicator_outline_color;
    char scroll_indicator_opacity[PERCENT_MAX_CHARS];
    bool wrap_entries;
    bool reset_on_back;
    bool mouse_select;
    bool inhibit_os_screensaver;
    char *startup_cmd;
    char *quit_cmd;
    ModeOnLaunch on_launch;
    bool screensaver_enabled;
    Uint32 screensaver_idle_time;
    char screensaver_intensity_str[PERCENT_MAX_CHARS];
    bool screensaver_pause_slideshow;
    bool gamepad_enabled;
    int gamepad_device;
    char *gamepad_mappings_file;
    bool debug;
    char *exe_path;
    Menu *first_menu;
    size_t num_menus;
    bool clock_enabled;
    bool clock_show_date;
    Alignment clock_alignment;
    char *clock_font_path;
    char clock_margin_str[PERCENT_MAX_CHARS];
    int clock_margin;
    SDL_Color clock_font_color;
    char clock_opacity[PERCENT_MAX_CHARS];
    unsigned int clock_font_size;
    bool clock_shadows;
    SDL_Color clock_shadow_color;
    TimeFormat clock_time_format;
    DateFormat clock_date_format;
    bool clock_include_weekday;
    Uint32 slideshow_image_duration;
    Uint32 slideshow_transition_time;
} Config;

void quit_slideshow(void);
void set_draw_color(void);
void quit(int status);
void print_version(FILE *stream);
