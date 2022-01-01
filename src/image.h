// Dynamic SVG generation for highlight
#define SVG_HIGHLIGHT "<svg viewBox=\"0 0 %i %i\"><rect x=\"0\" width=\"%i\" height=\"%i\" rx=\"%i\" fill=\"white\" /></svg>"

int init_svg(void);
void quit_svg(void);
SDL_Surface *load_next_slideshow_background(slideshow_t *slideshow, bool transition);
int load_next_slideshow_background_async(void *data);
SDL_Texture *load_texture(char *path, SDL_Surface *surface);
SDL_Texture *rasterize_svg(char *filename, char *xml, int w, int h);
SDL_Texture *render_highlight(int width, int height, int rx);
SDL_Texture *render_text(entry_t *entry, geometry_t *geo);