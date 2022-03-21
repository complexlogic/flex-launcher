// Dynamic SVG generation for highlight
#define HIGHLIGHT_OUTLINE_FORMAT " stroke-width=\"%i\" stroke=\"#%02X%02X%02X\" stroke-opacity=\"%.2f\""
#define HIGHLIGHT_FORMAT "<svg viewBox=\"0 0 %i %i\"><rect x=\"0\" width=\"%i\" height=\"%i\" rx=\"%i\" fill=\"#%02X%02X%02X\" fill-opacity=\"%.2f\"%s/></svg>"
#define SHADOW_OPACITY_MULTIPLIER 0.75F

// Macro functions
#define format_highlight_outline(buffer, outline_size, outline_color, outline_opacity) sprintf_alloc(buffer, HIGHLIGHT_OUTLINE_FORMAT, outline_size, outline_color.r, outline_color.g, outline_color.b, outline_opacity)
#define format_highlight(buffer, width, height, corner_radius, fill_color, fill_opacity, outline_buffer) sprintf_alloc(buffer, HIGHLIGHT_FORMAT, width, height, width, height, corner_radius, fill_color.r, fill_color.g, fill_color.b, fill_opacity, outline_buffer)
#define calculate_shadow_alpha(x) x.shadow_color->a = (Uint8) (SHADOW_OPACITY_MULTIPLIER * (float) x.color->a)

typedef struct {
  TTF_Font *font;
  int font_size;
  const char **font_path;
  SDL_Color *color;
  bool shadow;
  SDL_Color *shadow_color;
  int max_width;
  launcher_mode_t oversize_mode;
} text_info_t;

int init_svg(void);
int load_font(text_info_t *info, const char *default_font);
void quit_svg(void);
SDL_Surface *load_next_slideshow_background(slideshow_t *slideshow, bool transition);
int load_next_slideshow_background_async(void *data);
SDL_Texture *load_texture(SDL_Surface *surface);
SDL_Texture *load_texture_from_file(const char *path);
SDL_Texture *rasterize_svg(const char *buffer, int w, int h, SDL_Rect *rect);
SDL_Texture *rasterize_svg_from_file(const char *path, int w, int h, SDL_Rect *rect);
SDL_Texture *render_highlight(int width, int height, unsigned int rx, SDL_Rect *rect);
SDL_Surface *render_text(const char *text, text_info_t *info, SDL_Rect *rect, int *text_height);
SDL_Texture *render_text_texture(const char *text, text_info_t *info, SDL_Rect *rect, int *text_height);