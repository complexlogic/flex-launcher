#define MAX_CLOCK_CHARS 20
#define CLOCK_SPACING_FACTOR 0.5F

// Clock
typedef struct {
    SDL_Surface *time_surface;
    SDL_Surface *date_surface;
    SDL_Texture *time_texture;
    SDL_Texture *date_texture;
    SDL_Rect time_rect;
    SDL_Rect date_rect;
    TextInfo text_info;
    time_t current_time;
    struct tm *time_info;
    int x_offset_time;
    int x_offset_date;
    int y_offset;
    int y_advance;
    char time_string[MAX_CLOCK_CHARS + 1];
    char date_string[MAX_CLOCK_CHARS + 1];
    TimeFormat time_format;
    DateFormat date_format;
    bool render_time;
    bool render_date;
} Clock;

static void calculate_text_metrics(TTF_Font *font, const char *text, int *h, int *x_offset);
static void calculate_clock_geometry(Clock *clk);
static void format_time(Clock *clk);
static void format_date(Clock *clk);
static void calculate_clock_positioning(Clock *clk);
void init_clock(Clock *clk);
void get_time(Clock *clk);
void render_clock(Clock *clk);
int render_clock_async(void *data);
TimeFormat get_time_format(const char *region);
DateFormat get_date_format(const char *region);
