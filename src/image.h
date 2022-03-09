// Dynamic SVG generation for highlight
#define SVG_HIGHLIGHT "<svg viewBox=\"0 0 %i %i\"><rect x=\"0\" width=\"%i\" height=\"%i\" rx=\"%i\" fill=\"white\" /></svg>"

typedef struct {
  TTF_Font *font;
  int font_size;
  int outline_size;
  const char **font_path;
  SDL_Color *color;
  SDL_Color *outline_color;
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