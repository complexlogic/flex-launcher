#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "launcher.h"
#include <launcher_config.h>
#include "image.h"
#include "util.h"
#include "debug.h"
#include "external/ini.h"
#define NANOSVG_IMPLEMENTATION
#include "external/nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "external/nanosvgrast.h"

extern config_t config;
extern SDL_Renderer *renderer;
extern SDL_Texture *background_texture;
extern TTF_Font *title_font;
NSVGrasterizer *rasterizer = NULL;


// A function to initalize SVG rasterization
int init_svg()
{
  rasterizer = nsvgCreateRasterizer();
  if (rasterizer == NULL) {
    output_log(LOGLEVEL_FATAL, "Fatal Error: Could not initialize SVG rasterizer.\n");
    return 1;
  }
  return 0;
}

// A function to quit the SVG subsystem
void quit_svg()
{
  nsvgDeleteRasterizer(rasterizer);
}

// A function to load the next slideshow background from the struct
SDL_Texture *load_next_slideshow_background(slideshow_t *slideshow, bool transition)
{
  SDL_Surface *surface = NULL;
  SDL_Texture *texture = NULL;
  int initial_index = slideshow->i;
  int attempts = 0;
  do {
    // Increment slideshow background index and load background
    (slideshow->i)++;
    if (slideshow->i >= slideshow->num_images) {
      slideshow->i = 0;
    }
    surface = IMG_Load(slideshow->images[slideshow->order[slideshow->i]]);
    
    // If the loaded image has no alpha channel (e.g. JPEG), create one 
    // so that we can have transparency for the background transition
    if (surface != NULL && surface->format->format == SDL_PIXELFORMAT_RGB24 && transition) {
      SDL_Surface *tmp = SDL_CreateRGBSurfaceWithFormat(0,
        surface->w,
        surface->h,
        32,
        SDL_PIXELFORMAT_ARGB8888);
      Uint32 color = SDL_MapRGBA(tmp->format, 0, 0, 0, 0xFF);
      SDL_FillRect(tmp, NULL, color);
      SDL_BlitSurface(surface, NULL, tmp, NULL);
      SDL_FreeSurface(surface);
      surface = tmp;
      attempts++;
    } 
  } while (surface == NULL && slideshow->i != initial_index && attempts < slideshow->num_images);
  
  // Switch to color background mode if we failed to load any image from the array
  if (surface == NULL) {
    output_log(LOGLEVEL_ERROR, 
      "Error: Could not load any image from slideshow directory %s\n"
      "Changing background to color mode\n", 
      config.slideshow_directory);
    quit_slideshow();
    config.background_mode = MODE_COLOR;
    set_draw_color();
  }

  // If only one image in the entire slideshow array was valid, switch to
  // single image background mode
  else if (slideshow->i == initial_index && surface != NULL) {
    output_log(LOGLEVEL_ERROR, 
      "Error: Could only load one image from slideshow directory %s\n"
      "Changing background to single image mode\n",
      config.slideshow_directory);
    background_texture = SDL_CreateTextureFromSurface(renderer, surface);
    config.background_mode = MODE_IMAGE;
  }

  // Loading was successful, convert to texture
  else {
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    // Start as transparent, except first image
    if (transition) {
      SDL_SetTextureAlphaMod(texture, 0);
    }
  }
  return texture;
}

// A function to load a texture from a file OR existing SDL surface
SDL_Texture *load_texture(char *path, SDL_Surface *surface)
{
  SDL_Texture *texture = NULL;
  SDL_Surface *loaded_surface = NULL;

    if (surface == NULL) {
      loaded_surface = IMG_Load(path);
    }
    else {
      loaded_surface = surface;
    }

    if (loaded_surface == NULL) {
        output_log(LOGLEVEL_ERROR, 
                  "Error: Could not load image %s\n%s\n", 
                  path, 
                  IMG_GetError());
    }
    else {
        //Convert surface to screen format
        texture = SDL_CreateTextureFromSurface(renderer, loaded_surface);
        if (texture == NULL) {
            output_log(LOGLEVEL_ERROR, "Error: Could not create texture from %s\n%s", 
                       path, 
                       SDL_GetError());
        }

        //Get rid of old loaded surface
        SDL_FreeSurface(loaded_surface);
    }
    return texture;
}

// A function to rasterize an SVG from a file OR from xml text buffer
SDL_Texture *rasterize_svg(char *filename, char *xml, int w, int h)
{
  NSVGimage *image = NULL;
  unsigned char *pixel_buffer = NULL;
  int width, height, pitch;
  float scale;

  // Parse SVG to NSVGimage struct
  if (filename == NULL) {
    image = nsvgParse(xml, "px", 96.0f);
  }
  else {
    image = nsvgParseFromFile(filename, "px", 96.0f);
  }
  if (image == NULL) {
    output_log(LOGLEVEL_ERROR, "Error: could not open SVG image.\n");
    return NULL;
  }

  // Calculate scaling
  if (w == -1 && h == -1) {
    width = (int) image->width;
    height = (int) image->height;
    scale = 1.0f;
  }
  else {
    width = w;
    height = h;
    scale = (float) w / (float) image->width; // Assuming aspect ratio is conserved
  }

  // Allocate memory
  pitch = 4*width;
  pixel_buffer = malloc(4*width*height);
  if (pixel_buffer == NULL) {
    output_log(LOGLEVEL_ERROR, "Error: Could not alloc SVG pixel buffer.\n");
    return NULL;
  }

  // Rasterize image
  nsvgRasterize(rasterizer, image, 0, 0, scale, pixel_buffer, width, height, pitch);
  SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(pixel_buffer,
                                                  width,
                                                  height,
                                                  32,
                                                  pitch,
                                                  COLOR_MASKS);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  free(pixel_buffer);
  SDL_FreeSurface(surface);
  nsvgDelete(image);
  return texture;
}

// A function to render the highlight for the buttons
SDL_Texture *render_highlight(int width, int height, int rx)
{
  // Insert user config variables into SVG-formatted text buffer
  char buf[1024];
  sprintf(buf, SVG_HIGHLIGHT, width, height, width, height, rx);
  
  // Rasterize the SVG
  SDL_Texture *texture = rasterize_svg(NULL, buf, -1, -1);
  
  // Set color
  SDL_SetTextureColorMod(texture,
                         config.highlight_color.r,
                         config.highlight_color.g,
                         config.highlight_color.b);
  SDL_SetTextureAlphaMod(texture,config.highlight_color.a);
  return texture;
}

// A function to render title text for an entry
SDL_Texture *render_text(entry_t *entry, geometry_t *geo)
{
  TTF_Font *output_font = NULL;
  TTF_Font *reduced_font = NULL; // Font for Shrink text oversize mode
  int max_width = config.icon_size;
  int w, h;

  // Copy entry title to new buffer buffer
  char *title;
  copy_string(&title, entry->title);

  // Calculate size of the rendered title
  int title_length = strlen(title);
  TTF_SizeUTF8(title_font,title,&w,&h);

  // If title is too large to fit
  if (w > max_width) {

    // Truncate mode:
    if (config.title_oversize_mode == MODE_TEXT_TRUNCATE) {
      utf8_truncate(title, w, max_width);
      TTF_SizeUTF8(title_font,title,&w,&h);
    }

    // Shrink mode:
    else if (config.title_oversize_mode == MODE_TEXT_SHRINK) {
      int font_size = config.font_size - 1;
      reduced_font = TTF_OpenFont(config.title_font_path, font_size);
      TTF_SizeUTF8(reduced_font,title,&w,&h);

      // Keep trying smaller font until it fits
      while (w > max_width && font_size > 0) {
        TTF_CloseFont(reduced_font);
        reduced_font = NULL;
        font_size--;
        reduced_font = TTF_OpenFont(config.title_font_path, font_size);
        TTF_SizeUTF8(reduced_font,title,&w,&h);
      }

      // Set vertical offset so reduced font title remains vertically centered
      // with other titles
      if (font_size) {
        output_font = reduced_font;
        entry->title_offset = (geo->font_height - h) / 2;
      }
      else {
        reduced_font = NULL;
      }
    }
  }
  
  // Set geometry
  entry->text_rect.w = w;
  entry->text_rect.h = h;
  if (reduced_font == NULL) {
    output_font = title_font;
  }

  // Render texture
  SDL_Surface *text_surface = TTF_RenderUTF8_Blended(output_font,
                                                    title,
                                                    config.title_color);
  SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer,text_surface);
  SDL_FreeSurface(text_surface);
  if (reduced_font != NULL) {
    TTF_CloseFont(reduced_font);
  }
  free(title);
  return text_texture;
}