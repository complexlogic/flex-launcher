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

static int init_log(void);

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
    if (config.debug)
        printf("Debug mode enabled\nLog is outputted to %s\n", log_file_path);
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
    if (log_file == NULL)
        init_log();
    
    // Output log
    static char buffer[MAX_LOG_LINE_BYTES];
    va_list args;
    va_start(args, format);
    size_t length = (size_t) vsnprintf(buffer, MAX_LOG_LINE_BYTES - 1, format, args);
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

// A function to print the parsed settings to the log
void debug_settings()
{
    log_debug("======================= General ========================\n");
    DEBUG_STR(SETTING_DEFAULT_MENU, config.default_menu);
    DEBUG_BOOL(SETTING_VSYNC, config.vsync);
    DEBUG_INT(SETTING_FPS_LIMIT, config.fps_limit);
    DEBUG_INT(SETTING_APPLICATION_TIMEOUT, config.application_timeout / 1000);
    DEBUG_MODE(SETTING_ON_LAUNCH, MODE_SETTING_ON_LAUNCH, config.on_launch);
    DEBUG_BOOL(SETTING_WRAP_ENTRIES, config.wrap_entries);
    DEBUG_BOOL(SETTING_RESET_ON_BACK, config.reset_on_back);
    DEBUG_BOOL(SETTING_MOUSE_SELECT, config.mouse_select);
    DEBUG_BOOL(SETTING_INHIBIT_OS_SCREENSAVER, config.inhibit_os_screensaver);
    DEBUG_STR(SETTING_STARTUP_CMD, config.startup_cmd);
    DEBUG_STR(SETTING_QUIT_CMD, config.quit_cmd);
    log_debug("");

    log_debug("===================== Background =======================\n");
    DEBUG_MODE(SETTING_BACKGROUND_MODE, MODE_SETTING_BACKGROUND, config.background_mode);
    DEBUG_COLOR(SETTING_BACKGROUND_COLOR, config.background_color);
    DEBUG_STR(SETTING_BACKGROUND_IMAGE, config.background_image);
    DEBUG_STR(SETTING_SLIDESHOW_DIRECTORY, config.slideshow_directory);
    DEBUG_INT(SETTING_SLIDESHOW_IMAGE_DURATION, config.slideshow_image_duration / 1000);
    DEBUG_FLOAT(SETTING_SLIDESHOW_TRANSITION_TIME, ((float) config.slideshow_transition_time) / 1000.0f);
    DEBUG_BOOL(SETTING_BACKGROUND_OVERLAY, config.background_overlay);
    DEBUG_COLOR(SETTING_BACKGROUND_OVERLAY_COLOR, config.background_overlay_color);
    log_debug("");

    log_debug("======================= Layout =========================\n");
    DEBUG_INT(SETTING_MAX_BUTTONS, config.max_buttons);
    DEBUG_INT(SETTING_ICON_SIZE, config.icon_size);
    DEBUG_INT(SETTING_ICON_SPACING, config.icon_spacing);
    DEBUG_STR(SETTING_VCENTER, config.vcenter[0] != '\0' ? config.vcenter : "50%");
    log_debug("");

    log_debug("======================== Titles ========================\n");
    DEBUG_BOOL(SETTING_TITLES_ENABLED, config.titles_enabled);
    DEBUG_STR(SETTING_TITLE_FONT, config.title_font_path);
    DEBUG_INT(SETTING_TITLE_FONT_SIZE, config.title_font_size);
    DEBUG_COLOR(SETTING_TITLE_FONT_COLOR, config.title_font_color);
    DEBUG_BOOL(SETTING_TITLE_SHADOWS, config.title_shadows);
    DEBUG_COLOR(SETTING_TITLE_SHADOW_COLOR, config.title_shadow_color);
    DEBUG_MODE(SETTING_TITLE_OVERSIZE_MODE, MODE_SETTING_OVERSIZE, config.title_oversize_mode);
    DEBUG_INT(SETTING_TITLE_PADDING, config.title_padding);
    log_debug("");

    log_debug("====================== Highlight =======================\n");
    DEBUG_COLOR(SETTING_HIGHLIGHT_FILL_COLOR, config.highlight_fill_color);
    DEBUG_INT(SETTING_HIGHLIGHT_OUTLINE_SIZE, config.highlight_outline_size);
    DEBUG_COLOR(SETTING_HIGHLIGHT_OUTLINE_COLOR, config.highlight_outline_color);
    DEBUG_INT(SETTING_HIGHLIGHT_CORNER_RADIUS, config.highlight_rx);
    DEBUG_INT(SETTING_HIGHLIGHT_VPADDING, config.highlight_vpadding);
    DEBUG_INT(SETTING_HIGHLIGHT_HPADDING, config.highlight_hpadding);
    log_debug("");

    log_debug("================== Scroll Indicators ===================\n");
    DEBUG_BOOL(SETTING_SCROLL_INDICATORS, config.scroll_indicators);
    DEBUG_COLOR(SETTING_SCROLL_INDICATOR_FILL_COLOR, config.scroll_indicator_fill_color);
    DEBUG_INT(SETTING_SCROLL_INDICATOR_OUTLINE_SIZE, config.scroll_indicator_outline_size);
    DEBUG_COLOR(SETTING_SCROLL_INDICATOR_OUTLINE_COLOR, config.scroll_indicator_outline_color);
    log_debug("");

    log_debug("======================== Clock =========================\n");
    DEBUG_BOOL(SETTING_CLOCK_ENABLED, config.clock_enabled);
    DEBUG_BOOL(SETTING_CLOCK_SHOW_DATE, config.clock_show_date);
    DEBUG_MODE(SETTING_CLOCK_ALIGNMENT, MODE_SETTING_ALIGNMENT, config.clock_alignment);
    DEBUG_STR(SETTING_CLOCK_FONT, config.clock_font_path);
    DEBUG_INT(SETTING_CLOCK_FONT_SIZE, config.clock_font_size);
    DEBUG_INT(SETTING_CLOCK_MARGIN, config.clock_margin);
    DEBUG_COLOR(SETTING_CLOCK_FONT_COLOR, config.clock_font_color);
    DEBUG_BOOL(SETTING_CLOCK_SHADOWS, config.clock_shadows);
    DEBUG_COLOR(SETTING_CLOCK_SHADOW_COLOR, config.clock_shadow_color);
    DEBUG_MODE(SETTING_CLOCK_TIME_FORMAT, MODE_SETTING_TIME_FORMAT, config.clock_time_format);
    DEBUG_MODE(SETTING_CLOCK_DATE_FORMAT, MODE_SETTING_DATE_FORMAT, config.clock_date_format);
    DEBUG_BOOL(SETTING_CLOCK_INCLUDE_WEEKDAY, config.clock_include_weekday);
    log_debug("");

    log_debug("===================== Screensaver ======================\n");
    DEBUG_BOOL(SETTING_SCREENSAVER_ENABLED, config.screensaver_enabled);
    DEBUG_INT(SETTING_SCREENSAVER_IDLE_TIME, config.screensaver_idle_time / 1000);
    DEBUG_STR(SETTING_SCREENSAVER_INTENSITY, config.screensaver_intensity_str[0] != '\0' ? config.screensaver_intensity_str : DEFAULT_SCREENSAVER_INTENSITY);
    DEBUG_BOOL(SETTING_SCREENSAVER_PAUSE_SLIDESHOW, config.screensaver_pause_slideshow);
    log_debug("");
}

// A function to print the parsed menu entries to the command line
void debug_menu_entries(Menu *first_menu, size_t num_menus)
{
    if (first_menu == NULL) {
        log_debug("No valid menus found");
        return;
    }
    log_debug("======================= Menu Entries =======================\n");
    Menu *menu = first_menu;
    Entry *entry;
    for (size_t i = 0; i < num_menus; i ++) {
        log_debug("Menu Name: %s",menu->name);
        log_debug("Number of Entries: %i",menu->num_entries);
        entry = menu->first_entry;
        for (size_t j = 0; j < menu->num_entries; j++) {
            log_debug("Entry %i Title: %s",j,entry->title);
            log_debug("Entry %i Icon Path: %s",j,entry->icon_path);
            log_debug("Entry %i Command: %s",j,entry->cmd);
            if (j != menu->num_entries - 1)
                log_debug("");
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
    log_debug("======================= Gamepad ========================\n");
    DEBUG_BOOL(SETTING_GAMEPAD_ENABLED, config.gamepad_enabled);
    DEBUG_INT(SETTING_GAMEPAD_DEVICE, config.gamepad_device);
    DEBUG_STR(SETTING_GAMEPAD_MAPPINGS_FILE, config.gamepad_mappings_file);
    for (GamepadControl *i = gamepad_controls; i != NULL; i = i->next)
        log_debug("%-25s %s", i->label, i->cmd);
    log_debug("");
}

void debug_hotkeys(Hotkey *hotkeys)
{
    if (hotkeys == NULL) {
        log_debug("No hotkeys detected");
        return;
    }
    log_debug("======================== Hotkeys =========================\n");
    int index = 0;
    for (Hotkey *i = hotkeys; i != NULL; i = i->next) {
        log_debug("Hotkey %i Keycode: %X", index, i->keycode);
        log_debug("Hotkey %i Command: %s", index, i->cmd);
        index++;
    }
    log_debug("");
}

// A function to debug the parsed slideshow files
void debug_slideshow(Slideshow *slideshow)
{
    log_debug("======================== Slideshow ========================");
    log_debug("Found %i images in directory %s:", 
      slideshow->num_images, 
      config.slideshow_directory
    );
    for (int i = 0; i < slideshow->num_images; i++)
        log_debug("  %s", slideshow->images[slideshow->order[i]]);
}

// A function to debug the video settings
void debug_video(SDL_Renderer *renderer, SDL_DisplayMode *display_mode)
{
    log_debug("================== Video Information ===================\n");
    log_debug("Resolution:    %ix%i", display_mode->w, display_mode->h);
    log_debug("Refresh rate:  %i Hz", display_mode->refresh_rate);
    log_debug("Video driver:  %s", SDL_GetCurrentVideoDriver());
    log_debug("");
    SDL_RendererInfo info;
    SDL_GetRendererInfo(renderer, &info);
    log_debug("Supported Texture formats:");
    for(size_t i = 0; i < info.num_texture_formats; i++)
        log_debug("  %s", SDL_GetPixelFormatName(info.texture_formats[i]));
    log_debug("");
}
