#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <SDL.h>
#include "launcher.h"
#include <launcher_config.h>
#include "util.h"
#include "debug.h"
#include "platform/platform.h"
#ifdef __unix__
#include "platform/unix.h"
#endif

extern Config config;
extern FILE *log_file;

// A function to initialize the logging subsystem
static int init_log()
{
    // Determine log path
    char log_file_path[MAX_PATH_CHARS + 1];
#ifdef __unix__
    char log_file_directory[MAX_PATH_CHARS + 1];
    join_paths(log_file_directory, sizeof(log_file_directory), 4, getenv("HOME"), ".local", "share", EXECUTABLE_TITLE);
    make_directory(log_file_directory);
    join_paths(log_file_path, sizeof(log_file_path), 2, log_file_directory, FILENAME_LOG);
#else
    join_paths(log_file_path, sizeof(log_file_path), 2, config.exe_path, FILENAME_LOG);
#endif

    // Open log
    log_file = fopen(log_file_path, "wb");
    if (log_file == NULL) {
#ifdef __unix__
        printf("Failed to create log file");
#endif
        quit(EXIT_FAILURE);
    }
print_version(log_file);
fputs("\n", log_file);
print_compiler_info(log_file);
fputs("\n", log_file);
#ifdef __unix__
    if (config.debug) {
        printf("Debug mode enabled\nLog is outputted to %s\n", log_file_path);
    }
#endif
    return 0;
}

// A function to output a printf-style formatted line to the log
void output_log(LogLevel log_level, const char *format, ...)
{
    // Don't output a debug message if we aren't in debug mode
    if (log_level == LOGLEVEL_DEBUG && !config.debug)
        return;

    // Initialize logging if not already initialized
    else if (log_file == NULL)
        init_log();
    
    // Output log
    static char buffer[MAX_LOG_LINE_BYTES];
    va_list args;
    va_start(args, format);
    int length = vsnprintf(buffer, MAX_LOG_LINE_BYTES - 1, format, args);
    fwrite(buffer, 1, length, log_file);
    if (config.debug)
        fflush(log_file);
    
#ifdef __unix__
    if (log_level > LOGLEVEL_DEBUG)
        fputs(buffer, stderr);
#endif
    va_end(args);

    if (log_level == LOGLEVEL_FATAL)
        quit(EXIT_FAILURE);
}

void print_compiler_info(FILE *stream)
{
    fputs("Build date: " __DATE__ "\n", stream);
#ifdef __GNUC__
    fprintf(stream, "Compiler:   GCC %u.%u\n", __GNUC__, __GNUC_MINOR__);
#endif
#ifdef _MSC_VER
    fprintf(stream, "Compiler:   Microsoft C/C++ %.2f\n", (float) _MSC_VER / 100.0f);
#endif

}


// A function to print the parsed settings to the command line
void debug_settings()
{
    log_debug("======================== Settings ========================");
    log_debug("%s: %s", SETTING_DEFAULT_MENU,config.default_menu);
    log_debug("%s: %i", SETTING_MAX_BUTTONS,config.max_buttons);
    if (config.background_mode == MODE_COLOR) {
        log_debug("%s: %s", SETTING_BACKGROUND_MODE, "Color");
    }
    else if (config.background_mode == MODE_IMAGE) {
        log_debug("%s: %s", SETTING_BACKGROUND_MODE, "Image");
    }
    else if (config.background_mode == MODE_SLIDESHOW) {
        log_debug("%s: %s", SETTING_BACKGROUND_MODE, "Slideshow");
    }
    log_debug("%s R: %i", SETTING_BACKGROUND_COLOR, config.background_color.r);
    log_debug("%s G: %i", SETTING_BACKGROUND_COLOR, config.background_color.g);
    log_debug("%s B: %i", SETTING_BACKGROUND_COLOR, config.background_color.b);
    if (config.background_image != NULL) {
        log_debug("%s: %s", SETTING_BACKGROUND_IMAGE, config.background_image);
    }
    if (config.slideshow_directory != NULL) {
        log_debug("%s: %s", SETTING_SLIDESHOW_DIRECTORY , config.slideshow_directory);
    }
    log_debug("%s: %i", SETTING_SLIDESHOW_IMAGE_DURATION, config.slideshow_image_duration / 1000);
    log_debug("%s: %f", SETTING_SLIDESHOW_TRANSITION_TIME, ((float) config.slideshow_transition_time) / 1000.0f);
    if (config.background_overlay) {
        log_debug("%s: %s", SETTING_BACKGROUND_OVERLAY, "true");
    }
    else {
        log_debug("%s: %s", SETTING_BACKGROUND_OVERLAY, "false");
    }
    log_debug("%s R: %i", SETTING_BACKGROUND_OVERLAY_COLOR, config.background_overlay_color.r);
    log_debug("%s G: %i", SETTING_BACKGROUND_OVERLAY_COLOR, config.background_overlay_color.g);
    log_debug("%s B: %i", SETTING_BACKGROUND_OVERLAY_COLOR, config.background_overlay_color.b);
    log_debug("%s A: %i", SETTING_BACKGROUND_OVERLAY_COLOR, config.background_overlay_color.a);
    log_debug("%s: %i", SETTING_ICON_SIZE, config.icon_size);
    log_debug("%s: %i", SETTING_ICON_SPACING, config.icon_spacing);
    log_debug("%s: %s", SETTING_TITLE_FONT, config.title_font_path);
    log_debug("%s: %i", SETTING_TITLE_FONT_SIZE, config.title_font_size);
    log_debug("%s R: %i", SETTING_TITLE_FONT_COLOR, config.title_font_color.r);
    log_debug("%s G: %i", SETTING_TITLE_FONT_COLOR, config.title_font_color.g);
    log_debug("%s B: %i", SETTING_TITLE_FONT_COLOR, config.title_font_color.b);
    log_debug("%s A: %i", SETTING_TITLE_FONT_COLOR, config.title_font_color.a);
    if (config.title_shadows) {
        log_debug("%s: %s", SETTING_TITLE_SHADOWS, "true");
    }
    else {
        log_debug("%s: %s", SETTING_TITLE_SHADOWS, "false");
    }
    log_debug("%s R: %i", SETTING_TITLE_SHADOW_COLOR, config.title_shadow_color.r);
    log_debug("%s G: %i", SETTING_TITLE_SHADOW_COLOR, config.title_shadow_color.g);
    log_debug("%s B: %i", SETTING_TITLE_SHADOW_COLOR, config.title_shadow_color.b);
    log_debug("%s A: %i", SETTING_TITLE_SHADOW_COLOR, config.title_shadow_color.a);
    
    if (config.title_oversize_mode == MODE_TRUNCATE) {
        log_debug("%s: %s", SETTING_TITLE_OVERSIZE_MODE, "Truncate");
    }
    else if (config.title_oversize_mode == MODE_SHRINK) {
        log_debug("%s: %s", SETTING_TITLE_OVERSIZE_MODE, "Shrink");
    }
    else if (config.title_oversize_mode == MODE_NONE) {
        log_debug("%s: %s", SETTING_TITLE_OVERSIZE_MODE, "None");
    }
    log_debug("%s: %i", SETTING_TITLE_PADDING, config.title_padding);
    log_debug("%s R: %i", SETTING_HIGHLIGHT_FILL_COLOR, config.highlight_fill_color.r);
    log_debug("%s G: %i", SETTING_HIGHLIGHT_FILL_COLOR, config.highlight_fill_color.g);
    log_debug("%s B: %i", SETTING_HIGHLIGHT_FILL_COLOR, config.highlight_fill_color.b);
    log_debug("%s A: %i", SETTING_HIGHLIGHT_FILL_COLOR, config.highlight_fill_color.a);
    log_debug("%s: %i", SETTING_HIGHLIGHT_OUTLINE_SIZE, config.highlight_outline_size);
    log_debug("%s R: %i", SETTING_HIGHLIGHT_OUTLINE_COLOR, config.highlight_outline_color.r);
    log_debug("%s G: %i", SETTING_HIGHLIGHT_OUTLINE_COLOR, config.highlight_outline_color.g);
    log_debug("%s B: %i", SETTING_HIGHLIGHT_OUTLINE_COLOR, config.highlight_outline_color.b);
    log_debug("%s A: %i", SETTING_HIGHLIGHT_OUTLINE_COLOR, config.highlight_outline_color.a);
    log_debug("%s: %i", SETTING_HIGHLIGHT_CORNER_RADIUS, config.highlight_rx);
    log_debug("%s: %i", SETTING_HIGHLIGHT_VPADDING, config.highlight_vpadding);
    log_debug("%s: %i", SETTING_HIGHLIGHT_HPADDING, config.highlight_hpadding);
    if (config.scroll_indicators) {
        log_debug("%s: %s", SETTING_SCROLL_INDICATORS, "true");
    }
    else {
        log_debug("%s: %s", SETTING_SCROLL_INDICATORS, "false");
    }
    log_debug("%s R: %i", SETTING_SCROLL_INDICATOR_FILL_COLOR, config.scroll_indicator_fill_color.r);
    log_debug("%s G: %i", SETTING_SCROLL_INDICATOR_FILL_COLOR, config.scroll_indicator_fill_color.g);
    log_debug("%s B: %i", SETTING_SCROLL_INDICATOR_FILL_COLOR, config.scroll_indicator_fill_color.b);
    log_debug("%s A: %i", SETTING_SCROLL_INDICATOR_FILL_COLOR, config.scroll_indicator_fill_color.a);
    log_debug("%s: %i", SETTING_SCROLL_INDICATOR_OUTLINE_SIZE, config.scroll_indicator_outline_size);
    log_debug("%s R: %i", SETTING_SCROLL_INDICATOR_OUTLINE_COLOR, config.scroll_indicator_outline_color.r);
    log_debug("%s G: %i", SETTING_SCROLL_INDICATOR_OUTLINE_COLOR, config.scroll_indicator_outline_color.g);
    log_debug("%s B: %i", SETTING_SCROLL_INDICATOR_OUTLINE_COLOR, config.scroll_indicator_outline_color.b);
    log_debug("%s A: %i", SETTING_SCROLL_INDICATOR_OUTLINE_COLOR, config.scroll_indicator_outline_color.a);
    if (config.on_launch == MODE_BLANK) {
        log_debug("%s: %s", SETTING_ON_LAUNCH, "Blank");
    }
    else if (config.on_launch == MODE_NONE) {
        log_debug("%s: %s", SETTING_ON_LAUNCH, "None");
    }
    if (config.reset_on_back) {
        log_debug("%s: %s", SETTING_RESET_ON_BACK, "true");
    }
    else {
        log_debug("%s: %s", SETTING_RESET_ON_BACK, "false");
    }
    if (config.mouse_select) {
        log_debug("%s: %s", SETTING_MOUSE_SELECT, "true");
    }
    else {
        log_debug("%s: %s", SETTING_MOUSE_SELECT, "false");
    }
    if (config.inhibit_os_screensaver) {
        log_debug("%s: %s", SETTING_INHIBIT_OS_SCREENSAVER, "true");
    }
    else {
        log_debug("%s: %s", SETTING_INHIBIT_OS_SCREENSAVER, "false");
    }
    if (config.startup_cmd != NULL) {
        log_debug("%s: %s", SETTING_STARTUP_CMD, config.startup_cmd);
    }
    if (config.quit_cmd != NULL) {
        log_debug("%s: %s", SETTING_QUIT_CMD, config.quit_cmd);
    }
    log_debug("========================== Clock ==========================");
    if (config.clock_enabled) {
        log_debug("%s: %s", SETTING_CLOCK_ENABLED, "true");
    }
    else {
        log_debug("%s: %s", SETTING_CLOCK_ENABLED, "false");
    }
    if (config.clock_show_date) {
        log_debug("%s: %s", SETTING_CLOCK_SHOW_DATE, "true");
    }
    else {
        log_debug("%s: %s", SETTING_CLOCK_SHOW_DATE, "false");
    }
    if (config.clock_alignment == ALIGNMENT_LEFT) {
        log_debug("%s: %s", SETTING_CLOCK_ALIGNMENT, "Left");
    }
    else if (config.clock_alignment == ALIGNMENT_RIGHT) {
        log_debug("%s: %s", SETTING_CLOCK_ALIGNMENT, "Right");
    }
    log_debug("%s: %s", SETTING_CLOCK_FONT, config.clock_font_path);
    log_debug("%s: %u", SETTING_CLOCK_FONT_SIZE,config.clock_font_size);
    log_debug("%s: %i",SETTING_CLOCK_MARGIN, config.clock_margin);    
    log_debug("%s R: %i", SETTING_CLOCK_FONT_COLOR, config.clock_font_color.r);
    log_debug("%s G: %i", SETTING_CLOCK_FONT_COLOR, config.clock_font_color.g);
    log_debug("%s B: %i", SETTING_CLOCK_FONT_COLOR, config.clock_font_color.b);
    log_debug("%s A: %i", SETTING_CLOCK_FONT_COLOR, config.clock_font_color.a);
    if (config.clock_shadows) {
        log_debug("%s: %s", SETTING_CLOCK_SHADOWS, "true");
    }
    else {
        log_debug("%s: %s", SETTING_CLOCK_SHADOWS, "false");
    }
    log_debug("%s R: %i", SETTING_CLOCK_SHADOW_COLOR, config.clock_shadow_color.r);
    log_debug("%s G: %i", SETTING_CLOCK_SHADOW_COLOR, config.clock_shadow_color.g);
    log_debug("%s B: %i", SETTING_CLOCK_SHADOW_COLOR, config.clock_shadow_color.b);
    log_debug("%s A: %i", SETTING_CLOCK_SHADOW_COLOR, config.clock_shadow_color.a);
    if (config.clock_time_format == FORMAT_TIME_12HR) {
        log_debug("%s: %s", SETTING_CLOCK_TIME_FORMAT, "12hr");
    }
    else if (config.clock_time_format == FORMAT_TIME_24HR) {
        log_debug("%s: %s", SETTING_CLOCK_TIME_FORMAT, "24hr");
    }
    else {
        log_debug("%s: %s", SETTING_CLOCK_TIME_FORMAT, "Auto");
    }
    if (config.clock_date_format == FORMAT_DATE_LITTLE) {
        log_debug("%s: %s", SETTING_CLOCK_DATE_FORMAT, "Little");
    }
    else if (config.clock_date_format == FORMAT_DATE_BIG) {
        log_debug("%s: %s", SETTING_CLOCK_DATE_FORMAT, "Big");
    }
    else {
        log_debug("%s: %s", SETTING_CLOCK_DATE_FORMAT, "Auto");
    }
    if (config.clock_include_weekday) {
        log_debug("%s: %s", SETTING_CLOCK_INCLUDE_WEEKDAY, "true");
    }
    else {
        log_debug("%s: %s", SETTING_CLOCK_INCLUDE_WEEKDAY, "false");
    }

    log_debug("======================= Screensaver =======================");
    if (config.screensaver_enabled) {
        log_debug("%s: %s",SETTING_SCREENSAVER_ENABLED, "true");
    }
    else {
        log_debug("%s: %s",SETTING_SCREENSAVER_ENABLED, "false");
    }
    log_debug("%s: %i", SETTING_SCREENSAVER_IDLE_TIME, config.screensaver_idle_time / 1000);
    if(strlen(config.screensaver_intensity_str)) {
        log_debug("%s: %s",SETTING_SCREENSAVER_INTENSITY, config.screensaver_intensity_str);
    }
    else {
        log_debug("%s: %s",SETTING_SCREENSAVER_INTENSITY, DEFAULT_SCREENSAVER_INTENSITY);
    }
    if (config.screensaver_pause_slideshow) {
        log_debug("%s: %s",SETTING_SCREENSAVER_PAUSE_SLIDESHOW, "true");
    }
    else {
        log_debug("%s: %s",SETTING_SCREENSAVER_PAUSE_SLIDESHOW, "false");
    }
}

// A function to print the parsed menu entries to the command line
void debug_menu_entries(Menu *first_menu, int num_menus) 
{
    if (first_menu == NULL) {
        log_debug("No valid menus found");
        return;
    }
    log_debug("======================= Menu Entries =======================");
    Menu *menu = first_menu;
    Entry *entry;
    for (int i = 0; i < num_menus; i ++) {
        log_debug("Menu Name: %s",menu->name);
        log_debug("Number of Entries: %i",menu->num_entries);
        entry = menu->first_entry;
        for (int j = 0; j < menu->num_entries; j++) {
            log_debug("Entry %i Title: %s",j,entry->title);
            log_debug("Entry %i Icon Path: %s",j,entry->icon_path);
            log_debug("Entry %i Command: %s",j,entry->cmd);
            if (j != menu->num_entries - 1) {
                log_debug("");
            }
            entry = entry->next;
        }
        if (i != num_menus - 1) {
            log_debug("----------------------------------------------------------");
        }
        menu = menu->next;
    }
    log_debug("");
}

void debug_gamepad(GamepadControl *gamepad_controls)
{
    log_debug("======================== Gamepad ========================");
    if (config.gamepad_enabled) {
        log_debug("%s: %s",SETTING_GAMEPAD_ENABLED, "true");
    }
    else {
        log_debug("%s: %s",SETTING_GAMEPAD_ENABLED, "false");
    }
    log_debug("%s: %i", SETTING_GAMEPAD_DEVICE, config.gamepad_device);
    if (config.gamepad_mappings_file != NULL) {
        log_debug("%s: %s", SETTING_GAMEPAD_MAPPINGS_FILE, config.gamepad_mappings_file);
    }
    for (GamepadControl *i = gamepad_controls; i != NULL; i = i->next) {
        log_debug("%s: %s", i->label, i->cmd);
    }
}

void debug_hotkeys(Hotkey *hotkeys)
{
    if (hotkeys == NULL) {
        log_debug("No hotkeys detected");
        return;
    }
    log_debug("======================== Hotkeys =========================");
    int index = 0;
    for (Hotkey *i = hotkeys; i != NULL; i = i->next) {
        log_debug("Hotkey %i Keycode: %X", index, i->keycode);
        log_debug("Hotkey %i Command: %s", index, i->cmd);
        index++;
    }
}

// A function to debug the parsed slideshow files
void debug_slideshow(Slideshow *slideshow)
{
    log_debug("======================== Slideshow ========================");
    log_debug(
      "Found %i images in directory %s:", 
      slideshow->num_images, 
      config.slideshow_directory
    );
    for (int i = 0; i < slideshow->num_images; i++) {
        log_debug("%s", slideshow->images[slideshow->order[i]]);
    }
}

// A function to debug the video settings
void debug_video(SDL_Renderer *renderer, SDL_DisplayMode *display_mode)
{
    log_debug("===================== Video Information =====================");
    log_debug("Resolution: %ix%i", display_mode->w, display_mode->h);
    log_debug("Refresh rate: %i Hz", display_mode->refresh_rate);
    log_debug("Video driver: %s", SDL_GetCurrentVideoDriver());
    SDL_RendererInfo info;
    SDL_GetRendererInfo(renderer, &info);
    log_debug("Supported Texture formats:");
    for(int i = 0; i < info.num_texture_formats; i++) {
        log_debug("%s", SDL_GetPixelFormatName(info.texture_formats[i]));
    }
}

