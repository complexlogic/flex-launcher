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
#include "platform/platform.h"

static void calculate_text_metrics(TTF_Font *font, const char *text, int *h, int *x_offset);
static void calculate_clock_geometry(Clock *clk);
static void format_time(Clock *clk);
static void format_date(Clock *clk);
static void calculate_clock_positioning(Clock *clk);

extern Config config;
extern State state;
extern Geometry geo;

// A function to calculate height and x offset of text
static void calculate_text_metrics(TTF_Font *font, const char *text, int *h, int *x_offset)
{
    int ymin = 0;
    int ymax = 0; 
    int xmin, xmax, xadvance;
    int current_ymin, current_ymax;
    char *p = (char*) text;
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
        if (current_ymax > ymax)
            ymax = current_ymax;
        if (current_ymin < ymin)
            ymin = current_ymin;
        if (p == text && config.clock_alignment == ALIGNMENT_LEFT)
            *x_offset = xmin;

    p += bytes;
    }
    if (config.clock_alignment == ALIGNMENT_RIGHT)
        *x_offset = xadvance - xmax;
    *h = ymax - ymin;
}

// A function to calculate the spacing and offsets of the clock text
static void calculate_clock_geometry(Clock *clk)
{
    int line_skip = TTF_FontLineSkip(clk->text_info.font);
    int h_time, h_date;

    // Calculate text height and x offset
    calculate_text_metrics(clk->text_info.font, 
        clk->time_string,
        &h_time,
        &clk->x_offset_time
    );

    if (config.clock_show_date) {
        calculate_text_metrics(clk->text_info.font, 
            clk->date_string,
            &h_date,
            &clk->x_offset_date
        );

        int spacing = (int) (CLOCK_SPACING_FACTOR * (float) h_time);
        clk->y_advance = spacing + h_time;
    }

    // Calculate y offset from margin
    clk->y_offset = (line_skip - h_time) / 2;
}

// A function to get the current time from the operating system
void get_time(Clock *clk)
{
    int previous_min = 0;
    int previous_day = 0;
    if (clk->time_info != NULL) {
        previous_min = clk->time_info->tm_min;
        previous_day = clk->time_info->tm_mday;
    }

    // Get current time
    time(&clk->current_time);
    clk->time_info = localtime(&clk->current_time);
    
    // Set render flags if time and/or date changed
    if (clk->time_info == NULL || previous_min != clk->time_info->tm_min) {
        clk->render_time = true;
        if (clk->time_info == NULL || previous_day != clk->time_info->tm_mday)
            clk->render_date = true;
    }
}

// A function to format the current time according to user settings
static void format_time(Clock *clk)
{
    char *format = NULL;
    if (clk->time_format == FORMAT_TIME_24HR)
        format = TIME_STRING_24HR;
    else
        format = TIME_STRING_12HR;
    strftime(clk->time_string, 
        sizeof(clk->time_string), 
        format, 
        clk->time_info
    );
}

// A function to format the current date according to user settings
static void format_date(Clock *clk)
{
    char *format = NULL;
 
    // Get date format
    if (clk->date_format == FORMAT_DATE_LITTLE)
        format = DATE_STRING_LITTLE;
    else
        format = DATE_STRING_BIG;
    char weekday[MAX_CLOCK_CHARS + 1];
    char date[MAX_CLOCK_CHARS + 1];
    size_t bytes = sizeof(clk->date_string);

    // Get weekday name
    if (config.clock_include_weekday) {
        strftime(weekday, 
            sizeof(weekday), 
            "%a ", 
            clk->time_info
        );
    }
    else
        weekday[0] = '\0';
    copy_string(clk->date_string, 
        weekday, 
        sizeof(clk->date_string)
    );
    bytes -= strlen(weekday);
    if (bytes) {
        // Get date
        strftime(date, 
            sizeof(date), 
            format, 
            clk->time_info
        );
        bytes -= strlen(date);
        strncat(clk->date_string, 
            date,
            bytes
        );
    }
}

// A function to calculate the x and y coordinates of the clock text
static void calculate_clock_positioning(Clock *clk)
{
    if (config.clock_alignment == ALIGNMENT_LEFT) {
        clk->time_rect.x = config.clock_margin - clk->x_offset_time;
        if (config.clock_show_date)
            clk->date_rect.x = config.clock_margin - clk->x_offset_date;
    }
    else {
        clk->time_rect.x = geo.screen_width - config.clock_margin - clk->time_rect.w + clk->x_offset_time;
        if (config.clock_show_date)
            clk->date_rect.x = geo.screen_width - config.clock_margin - clk->date_rect.w + clk->x_offset_date;
    }
    clk->time_rect.y = config.clock_margin - clk->y_offset;
    if (config.clock_show_date)
        clk->date_rect.y = clk->time_rect.y + clk->y_advance;
}

// A function to initialize the clock
void init_clock(Clock *clk)
{
    // Initialize clock structure
    clk->text_info = (TextInfo) {
        .font = NULL,
        .font_size = (int) config.clock_font_size,
        .font_path = &config.clock_font_path,
        .color = &config.clock_font_color,
        .shadow = config.clock_shadows,
        .oversize_mode = OVERSIZE_NONE
    };
    clk->time_format = config.clock_time_format;
    clk->date_format = config.clock_date_format;
    clk->time_info = NULL;
    if (config.clock_shadows) {
        clk->text_info.shadow_color = &config.clock_shadow_color;
        calculate_shadow_alpha(clk->text_info);
    }
    else
        clk->text_info.shadow_color = NULL;
    
    // Load the font
    int error = load_font(&clk->text_info, FILENAME_DEFAULT_CLOCK_FONT);
    if (error) {
        config.clock_enabled = false;
        return;
    }

    // Get time and format it into a string
    get_time(clk);
    if (clk->time_format == FORMAT_TIME_AUTO || clk->date_format == FORMAT_DATE_AUTO) {
        char region[3];
        memset(region, '\0', sizeof(region));
        get_region(region);
        if (clk->time_format == FORMAT_TIME_AUTO)
            clk->time_format = get_time_format(region);
        if (config.clock_show_date && clk->date_format == FORMAT_DATE_AUTO)
            clk->date_format = get_date_format(region);
    }

    // Render the time and date
    format_time(clk);
    clk->time_texture = render_text_texture(clk->time_string,
                            &clk->text_info,
                            &clk->time_rect,
                            NULL
                        );
    if (config.clock_show_date) {
        format_date(clk);
        clk->date_texture = render_text_texture(clk->date_string,
                                &clk->text_info,
                                &clk->date_rect,
                                NULL
                            );
    }

    // Calculate geometry
    calculate_clock_geometry(clk);
    calculate_clock_positioning(clk);
    clk->render_time = false;
    clk->render_date = false;
}

// A function to render the time to image
void render_clock(Clock *clk)
{
    format_time(clk);
    clk->time_surface = render_text(clk->time_string,
                            &clk->text_info,
                            &clk->time_rect,
                            NULL
                        );
    if (clk->render_date) {
        format_date(clk);
        clk->date_surface = render_text(clk->date_string,
                                &clk->text_info,
                                &clk->date_rect,
                                NULL
                            );
        calculate_clock_geometry(clk);
    }
    calculate_clock_positioning(clk);
    state.clock_ready = true;
}

// A functio nto render the time to image in a separate thread
int render_clock_async(void *data)
{
    Clock *clk = (Clock*) data;
    render_clock(clk);
    return 0;
} 

// A function to get the time format for a region
TimeFormat get_time_format(const char *region)
{
    const char *countries[] = {
        "US",
        "CA",
        "GB",
        "AU",
        "NZ",
        "IN"
    };
    TimeFormat format = FORMAT_TIME_24HR;
    for (size_t i = 0; i < sizeof(countries) / sizeof(countries[0]); i++) {
        if (!strcmp(region, countries[i])) {
            format = FORMAT_TIME_12HR;
            break;
        }
    }
    return format;
}

// A function to get the date format for a region
DateFormat get_date_format(const char *region)
{
    const char *countries[] = {
        "US",
        "JP",
        "CN"
    };
    DateFormat format = FORMAT_DATE_LITTLE;
    for (size_t i = 0; i < sizeof(countries) / sizeof(countries[0]); i++) {
        if (!strcmp(region, countries[i])) {
            format = FORMAT_DATE_BIG;
            break;
        }
    }
    return format;
}
