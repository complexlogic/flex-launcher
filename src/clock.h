#define MAX_CLOCK_CHARS 20
#define CLOCK_SPACING_FACTOR 0.4F

// Clock
typedef struct {
    SDL_Surface *time_surface;
    SDL_Surface *date_surface;
    SDL_Texture *time_texture;
    SDL_Texture *date_texture;
    SDL_Rect time_rect;
    SDL_Rect date_rect;
    text_info_t text_info;
    time_t current_time;
    struct tm *time_info;
    int x_offset_time;
    int x_offset_date;
    int y_offset;
    int y_advance;
    char time_string[MAX_CLOCK_CHARS + 1];
    char date_string[MAX_CLOCK_CHARS + 1];
    time_format_t time_format;
    date_format_t date_format;
    bool render_time;
    bool render_date;
} launcher_clock_t;

static void calculate_text_metrics(TTF_Font *font, const char *text, int *h, int *x_offset);
static void calculate_clock_geometry(launcher_clock_t *launcher_clock);
static void format_time(launcher_clock_t *launcher_clock);
static void format_date(launcher_clock_t *launcher_clock);
static void calculate_clock_positioning(launcher_clock_t *launcher_clock);
void init_clock(launcher_clock_t *launcher_clock);
void get_time(launcher_clock_t *launcher_clock);
void render_clock(launcher_clock_t *launcher_clock);
int render_clock_async(void *data);
time_format_t get_time_format(const char *region);
date_format_t get_date_format(const char *region);
