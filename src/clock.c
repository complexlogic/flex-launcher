#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_thread.h>
#include "launcher.h"
#include <launcher_config.h>
#include "util.h"
#include "image.h"
#include "clock.h"
#include "debug.h"

extern config_t config;
extern state_t state;
extern geometry_t geo;

// A function to calculate height and x offset of text
static void calculate_text_metrics(TTF_Font *font, const char *text, int *h, int *x_offset)
{
  int ymin = 0;
  int ymax = 0; 
  int xmin, xmax, xadvance;
  int current_ymin, current_ymax;
  char *p = text;
  Uint16 code_point;
  int bytes;

  // Get text height, x offset
  while (*p != '\0') {
    code_point = get_unicode_code_point(p, &bytes);
    TTF_GlyphMetrics(font, 
      code_point, 
      &xmin, 
      &xmax, 
      &current_ymin, 
      &current_ymax,
      &xadvance
    );
    if (current_ymax > ymax) {
      ymax = current_ymax;
    }
    if (current_ymin < ymin) {
      ymin = current_ymin;
    }
    if (p == text && config.clock_alignment == ALIGNMENT_LEFT) {
      *x_offset = xmin;
    }
  p += bytes;
  }
  if (config.clock_alignment == ALIGNMENT_RIGHT) {
    *x_offset = xadvance - xmax;
  }
  *h = ymax - ymin;
}

// A function to calculate the spacing and offsets of the clock text
static void calculate_clock_geometry(launcher_clock_t *launcher_clock)
{
  int line_skip = TTF_FontLineSkip(launcher_clock->text_info.font);
  int h_time, h_date;

  // Calculate text height and x offset
  calculate_text_metrics(launcher_clock->text_info.font, 
    launcher_clock->time_string,
    &h_time,
    &launcher_clock->x_offset_time
  );

  if (config.clock_show_date) {
    calculate_text_metrics(launcher_clock->text_info.font, 
      launcher_clock->date_string,
      &h_date,
      &launcher_clock->x_offset_date
    );
    int h_avg = (h_time + h_date) / 2;
    int spacing = (int) (CLOCK_SPACING_FACTOR * (float) h_avg);
    launcher_clock->y_advance = spacing + h_avg;
  }

  // Calculate y offset from margin
  launcher_clock->y_offset = (line_skip - h_time) / 2;
}

// A function to get the current time from the operating system
void get_time(launcher_clock_t *launcher_clock)
{
  int previous_min = 0;
  int previous_day = 0;
  if (launcher_clock->time_info != NULL) {
    previous_min = launcher_clock->time_info->tm_min;
    previous_day = launcher_clock->time_info->tm_mday;
  }

  // Get current time
  time(&launcher_clock->current_time);
  launcher_clock->time_info = localtime(&launcher_clock->current_time);
  
  // Set render flags if time and/or date changed
  if (launcher_clock->time_info == NULL || previous_min != launcher_clock->time_info->tm_min) {
    launcher_clock->render_time = true;
    if (launcher_clock->time_info == NULL || previous_day != launcher_clock->time_info->tm_mday) {
      launcher_clock->render_date = true;
    }
  }
}

// A function to format the current time according to user settings
static void format_time(launcher_clock_t *launcher_clock)
{
  char *format = NULL;
  if (launcher_clock->time_format == FORMAT_TIME_24HR) {
    format = TIME_STRING_24HR;
  }
  else {
    format = TIME_STRING_12HR;
  }
  strftime(launcher_clock->time_string, 
    sizeof(launcher_clock->time_string), 
    format, 
    launcher_clock->time_info
  );
}

// A function to format the current date according to user settings
static void format_date(launcher_clock_t *launcher_clock)
{
  char *format = NULL;
 
  // Get date format
  if (launcher_clock->date_format == FORMAT_DATE_LITTLE) {
    format = DATE_STRING_LITTLE;
  }
  else {
    format = DATE_STRING_BIG;
  } 
  char weekday[MAX_CLOCK_CHARS + 1];
  char date[MAX_CLOCK_CHARS + 1];
  unsigned int bytes = sizeof(launcher_clock->date_string);

  // Get weekday name
  if (config.clock_include_weekday) {
    strftime(weekday, 
      sizeof(weekday), 
      "%a ", 
      launcher_clock->time_info
    );
  }
  else {
    weekday[0] = '\0';
  }
  copy_string(launcher_clock->date_string, 
    weekday, 
    sizeof(launcher_clock->date_string)
  );
  bytes -= strlen(weekday);
  if (bytes) {
    // Get date
    strftime(date, 
      sizeof(date), 
      format, 
      launcher_clock->time_info
    );
    bytes -= strlen(date);
    strncat(launcher_clock->date_string, 
      date,
      bytes
    );
  }
}

// A function to calculate the x and y coordinates of the clock text
static void calculate_clock_positioning(launcher_clock_t *launcher_clock)
{
  if (config.clock_alignment == ALIGNMENT_LEFT) {
    launcher_clock->time_rect.x = config.clock_margin - launcher_clock->x_offset_time;
    if (config.clock_show_date) {
      launcher_clock->date_rect.x = config.clock_margin - launcher_clock->x_offset_date;
    }
  }
  else {
    launcher_clock->time_rect.x = geo.screen_width - config.clock_margin - launcher_clock->time_rect.w + launcher_clock->x_offset_time;
    if (config.clock_show_date) {
      launcher_clock->date_rect.x = geo.screen_width - config.clock_margin - launcher_clock->date_rect.w + launcher_clock->x_offset_date;
    }
  }
  launcher_clock->time_rect.y = config.clock_margin - launcher_clock->y_offset;
  if (config.clock_show_date) {
    launcher_clock->date_rect.y = launcher_clock->time_rect.y + launcher_clock->y_advance;
  }
}

// A function to initialize the clock
void init_clock(launcher_clock_t *launcher_clock)
{
  // Initialize clock structure
  launcher_clock->text_info = (text_info_t) {.font = NULL,
                              .font_size = config.clock_font_size,
                              .font_path = &config.clock_font_path,
                              .color = &config.clock_color,
                              .oversize_mode = MODE_TEXT_NONE};
  launcher_clock->time_format = config.clock_time_format;
  launcher_clock->date_format = config.clock_date_format;
  launcher_clock->time_info = NULL;
  
  // Load the font
  int error = load_font(&launcher_clock->text_info, FILENAME_DEFAULT_CLOCK_FONT);
  if (error) {
    config.clock_enabled = false;
    return;
  }

  // Get time and format it into a string
  get_time(launcher_clock);
  if (launcher_clock->time_format == FORMAT_TIME_AUTO || launcher_clock->date_format == FORMAT_DATE_AUTO) {
    char region[3];
    memset(region, '\0', sizeof(region));
    get_region(region);
    if (launcher_clock->time_format == FORMAT_TIME_AUTO) {
      launcher_clock->time_format = get_time_format(region);
    }
    if (config.clock_show_date && launcher_clock->date_format == FORMAT_DATE_AUTO) {
      launcher_clock->date_format = get_date_format(region);
    }
  }

  // Render the time and date
  format_time(launcher_clock);
  launcher_clock->time_texture = render_text_texture(launcher_clock->time_string,
                                   &launcher_clock->text_info,
                                   &launcher_clock->time_rect,
                                   NULL
                                 );
  if (config.clock_show_date) {
    format_date(launcher_clock);
    launcher_clock->date_texture = render_text_texture(launcher_clock->date_string,
                                     &launcher_clock->text_info,
                                     &launcher_clock->date_rect,
                                     NULL
                                   );
  }

  // Calculate geometry
  calculate_clock_geometry(launcher_clock);
  calculate_clock_positioning(launcher_clock);
  launcher_clock->render_time = false;
  launcher_clock->render_date = false;
}

// A function to render the time to image
void render_clock(launcher_clock_t *launcher_clock)
{
  format_time(launcher_clock);
  launcher_clock->time_surface = render_text(launcher_clock->time_string,
                                   &launcher_clock->text_info,
                                   &launcher_clock->time_rect,
                                   NULL
                                 );
  if (launcher_clock->render_date) {
    format_date(launcher_clock);
    launcher_clock->date_surface = render_text(launcher_clock->date_string,
                                     &launcher_clock->text_info,
                                     &launcher_clock->date_rect,
                                     NULL
                                   );
    calculate_clock_geometry(launcher_clock);
  }
  calculate_clock_positioning(launcher_clock);
  state.clock_ready = true;
}

// A functio nto render the time to image in a separate thread
int render_clock_async(void *data)
{
  launcher_clock_t *launcher_clock = (launcher_clock_t*) data;
  render_clock(launcher_clock);
  return 0;
} 

// A function to get the time format for a region
time_format_t get_time_format(const char *region)
{
  const char *countries[] = {"US",
    "CA",
    "GB",
    "AU",
    "NZ",
    "IN"
  };
  time_format_t format = FORMAT_TIME_24HR;
  for (int i = 0; i < sizeof(countries) / sizeof(countries[0]); i++) {
    if (!strcmp(region, countries[i])) {
      format = FORMAT_TIME_12HR;
      break;
    }
  }
  return format;
}

// A function to get the date format for a region
date_format_t get_date_format(const char *region)
{
  const char *countries[] = {"US",
    "JP",
    "CN",
  };
  date_format_t format = FORMAT_DATE_LITTLE;
  for (int i = 0; i < sizeof(countries) / sizeof(countries[0]); i++) {
    if (!strcmp(region, countries[i])) {
      format = FORMAT_DATE_BIG;
      break;
    }
  }
  return format;
}
