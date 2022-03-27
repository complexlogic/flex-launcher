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

extern config_t config;
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
  log_file = fopen(log_file_path, FILE_MODE_WRITE);
  if (log_file == NULL) {
    #ifdef __unix__
    printf("Failed to create log file\n");
    #endif
    quit(EXIT_FAILURE);
  }
  #ifdef __unix__
  if (config.debug) {
    printf("Debug mode enabled\nLog is outputted to %s\n", log_file_path);
  }
  #endif
  return 0;
}

// A function to output a printf-style formatted line to the log
void output_log(log_level_t log_level, const char *format, ...)
{
  // Don't output a debug message if we aren't in debug mode
  if (log_level == LOGLEVEL_DEBUG && !config.debug) {
    return;
  }

  // Initialize logging if not already initialized
  else if (log_file == NULL) {
    init_log();
  }
  
  // Output log
  static char buffer[MAX_LOG_LINE_BYTES];
  va_list args;
  va_start(args, format);
  int length = vsnprintf(buffer, MAX_LOG_LINE_BYTES - 1, format, args);
  fwrite(buffer, 1, length, log_file);
  
  #ifdef __unix__
  if (log_level > LOGLEVEL_DEBUG) {
    fputs(buffer, stderr);
  }
  #endif
  va_end(args);

  if (log_level == LOGLEVEL_FATAL) {
    quit(EXIT_FAILURE);
  }
}
// A function to print the parsed settings to the command line
void debug_settings()
{
  output_log(LOGLEVEL_DEBUG, "======================== Settings ========================\n");
  output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_DEFAULT_MENU,config.default_menu);
  output_log(LOGLEVEL_DEBUG, "%s: %i\n", SETTING_MAX_BUTTONS,config.max_buttons);
  if (config.background_mode == MODE_COLOR) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_BACKGROUND_MODE, "Color");
  }
  else if (config.background_mode == MODE_IMAGE) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_BACKGROUND_MODE, "Image");
  }
  else if (config.background_mode == MODE_SLIDESHOW) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_BACKGROUND_MODE, "Slideshow");
  }
  output_log(LOGLEVEL_DEBUG, "%s R: %i\n", SETTING_BACKGROUND_COLOR, config.background_color.r);
  output_log(LOGLEVEL_DEBUG, "%s G: %i\n", SETTING_BACKGROUND_COLOR, config.background_color.g);
  output_log(LOGLEVEL_DEBUG, "%s B: %i\n", SETTING_BACKGROUND_COLOR, config.background_color.b);
  if (config.background_image != NULL) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_BACKGROUND_IMAGE, config.background_image);
  }
  if (config.slideshow_directory != NULL) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_SLIDESHOW_DIRECTORY , config.slideshow_directory);
  }
  output_log(LOGLEVEL_DEBUG, "%s: %i\n", SETTING_SLIDESHOW_IMAGE_DURATION, config.slideshow_image_duration / 1000);
  output_log(LOGLEVEL_DEBUG, "%s: %f\n", SETTING_SLIDESHOW_TRANSITION_TIME, ((float) config.slideshow_transition_time) / 1000.0f);
  if (config.background_overlay) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_BACKGROUND_OVERLAY, "true");
  }
  else {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_BACKGROUND_OVERLAY, "false");
  }
  output_log(LOGLEVEL_DEBUG, "%s R: %i\n", SETTING_BACKGROUND_OVERLAY_COLOR, config.background_overlay_color.r);
  output_log(LOGLEVEL_DEBUG, "%s G: %i\n", SETTING_BACKGROUND_OVERLAY_COLOR, config.background_overlay_color.g);
  output_log(LOGLEVEL_DEBUG, "%s B: %i\n", SETTING_BACKGROUND_OVERLAY_COLOR, config.background_overlay_color.b);
  output_log(LOGLEVEL_DEBUG, "%s A: %i\n", SETTING_BACKGROUND_OVERLAY_COLOR, config.background_overlay_color.a);
  output_log(LOGLEVEL_DEBUG, "%s: %i\n", SETTING_ICON_SIZE, config.icon_size);
  output_log(LOGLEVEL_DEBUG, "%s: %i\n", SETTING_ICON_SPACING, config.icon_spacing);
  output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_TITLE_FONT, config.title_font_path);
  output_log(LOGLEVEL_DEBUG, "%s: %i\n", SETTING_TITLE_FONT_SIZE, config.title_font_size);
  output_log(LOGLEVEL_DEBUG, "%s R: %i\n", SETTING_TITLE_FONT_COLOR, config.title_font_color.r);
  output_log(LOGLEVEL_DEBUG, "%s G: %i\n", SETTING_TITLE_FONT_COLOR, config.title_font_color.g);
  output_log(LOGLEVEL_DEBUG, "%s B: %i\n", SETTING_TITLE_FONT_COLOR, config.title_font_color.b);
  output_log(LOGLEVEL_DEBUG, "%s A: %i\n", SETTING_TITLE_FONT_COLOR, config.title_font_color.a);
  if (config.title_shadows) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_TITLE_SHADOWS, "true");
  }
  else {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_TITLE_SHADOWS, "false");
  }
  output_log(LOGLEVEL_DEBUG, "%s R: %i\n", SETTING_TITLE_SHADOW_COLOR, config.title_shadow_color.r);
  output_log(LOGLEVEL_DEBUG, "%s G: %i\n", SETTING_TITLE_SHADOW_COLOR, config.title_shadow_color.g);
  output_log(LOGLEVEL_DEBUG, "%s B: %i\n", SETTING_TITLE_SHADOW_COLOR, config.title_shadow_color.b);
  output_log(LOGLEVEL_DEBUG, "%s A: %i\n", SETTING_TITLE_SHADOW_COLOR, config.title_shadow_color.a);
  
  if (config.title_oversize_mode == MODE_TRUNCATE) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_TITLE_OVERSIZE_MODE, "Truncate");
  }
  else if (config.title_oversize_mode == MODE_SHRINK) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_TITLE_OVERSIZE_MODE, "Shrink");
  }
  else if (config.title_oversize_mode == MODE_NONE) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_TITLE_OVERSIZE_MODE, "None");
  }
  output_log(LOGLEVEL_DEBUG, "%s: %i\n", SETTING_TITLE_PADDING, config.title_padding);
  output_log(LOGLEVEL_DEBUG, "%s R: %i\n", SETTING_HIGHLIGHT_FILL_COLOR, config.highlight_fill_color.r);
  output_log(LOGLEVEL_DEBUG, "%s G: %i\n", SETTING_HIGHLIGHT_FILL_COLOR, config.highlight_fill_color.g);
  output_log(LOGLEVEL_DEBUG, "%s B: %i\n", SETTING_HIGHLIGHT_FILL_COLOR, config.highlight_fill_color.b);
  output_log(LOGLEVEL_DEBUG, "%s A: %i\n", SETTING_HIGHLIGHT_FILL_COLOR, config.highlight_fill_color.a);
  output_log(LOGLEVEL_DEBUG, "%s: %i\n", SETTING_HIGHLIGHT_OUTLINE_SIZE, config.highlight_outline_size);
  output_log(LOGLEVEL_DEBUG, "%s R: %i\n", SETTING_HIGHLIGHT_OUTLINE_COLOR, config.highlight_outline_color.r);
  output_log(LOGLEVEL_DEBUG, "%s G: %i\n", SETTING_HIGHLIGHT_OUTLINE_COLOR, config.highlight_outline_color.g);
  output_log(LOGLEVEL_DEBUG, "%s B: %i\n", SETTING_HIGHLIGHT_OUTLINE_COLOR, config.highlight_outline_color.b);
  output_log(LOGLEVEL_DEBUG, "%s A: %i\n", SETTING_HIGHLIGHT_OUTLINE_COLOR, config.highlight_outline_color.a);
  output_log(LOGLEVEL_DEBUG, "%s: %i\n", SETTING_HIGHLIGHT_CORNER_RADIUS, config.highlight_rx);
  output_log(LOGLEVEL_DEBUG, "%s: %i\n", SETTING_HIGHLIGHT_VPADDING, config.highlight_vpadding);
  output_log(LOGLEVEL_DEBUG, "%s: %i\n", SETTING_HIGHLIGHT_HPADDING, config.highlight_hpadding);
  if (config.scroll_indicators) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_SCROLL_INDICATORS, "true");
  }
  else {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_SCROLL_INDICATORS, "false");
  }
  output_log(LOGLEVEL_DEBUG, "%s R: %i\n", SETTING_SCROLL_INDICATOR_FILL_COLOR, config.scroll_indicator_fill_color.r);
  output_log(LOGLEVEL_DEBUG, "%s G: %i\n", SETTING_SCROLL_INDICATOR_FILL_COLOR, config.scroll_indicator_fill_color.g);
  output_log(LOGLEVEL_DEBUG, "%s B: %i\n", SETTING_SCROLL_INDICATOR_FILL_COLOR, config.scroll_indicator_fill_color.b);
  output_log(LOGLEVEL_DEBUG, "%s A: %i\n", SETTING_SCROLL_INDICATOR_FILL_COLOR, config.scroll_indicator_fill_color.a);
  output_log(LOGLEVEL_DEBUG, "%s: %i\n", SETTING_SCROLL_INDICATOR_OUTLINE_SIZE, config.scroll_indicator_outline_size);
  output_log(LOGLEVEL_DEBUG, "%s R: %i\n", SETTING_SCROLL_INDICATOR_OUTLINE_COLOR, config.scroll_indicator_outline_color.r);
  output_log(LOGLEVEL_DEBUG, "%s G: %i\n", SETTING_SCROLL_INDICATOR_OUTLINE_COLOR, config.scroll_indicator_outline_color.g);
  output_log(LOGLEVEL_DEBUG, "%s B: %i\n", SETTING_SCROLL_INDICATOR_OUTLINE_COLOR, config.scroll_indicator_outline_color.b);
  output_log(LOGLEVEL_DEBUG, "%s A: %i\n", SETTING_SCROLL_INDICATOR_OUTLINE_COLOR, config.scroll_indicator_outline_color.a);
  if (config.on_launch == MODE_HIDE) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_ON_LAUNCH, "Hide");
  }
  else if (config.on_launch == MODE_BLANK) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_ON_LAUNCH, "Blank");
  }
  else if (config.on_launch == MODE_NONE) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_ON_LAUNCH, "None");
  }
  if (config.reset_on_back) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_RESET_ON_BACK, "true");
  }
  else {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_RESET_ON_BACK, "false");
  }
  if (config.mouse_select) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_MOUSE_SELECT, "true");
  }
  else {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_MOUSE_SELECT, "false");
  }
  output_log(LOGLEVEL_DEBUG, "========================== Clock ==========================\n");
  if (config.clock_enabled) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_CLOCK_ENABLED, "true");
  }
  else {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_CLOCK_ENABLED, "false");
  }
  if (config.clock_show_date) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_CLOCK_SHOW_DATE, "true");
  }
  else {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_CLOCK_SHOW_DATE, "false");
  }
  if (config.clock_alignment == ALIGNMENT_LEFT) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_CLOCK_ALIGNMENT, "Left");
  }
  else if (config.clock_alignment == ALIGNMENT_RIGHT) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_CLOCK_ALIGNMENT, "Right");
  }
  output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_CLOCK_FONT, config.clock_font_path);
  output_log(LOGLEVEL_DEBUG, "%s: %u\n", SETTING_CLOCK_FONT_SIZE,config.clock_font_size);
  output_log(LOGLEVEL_DEBUG, "%s: %i\n",SETTING_CLOCK_MARGIN, config.clock_margin);  
  output_log(LOGLEVEL_DEBUG, "%s R: %i\n", SETTING_CLOCK_FONT_COLOR, config.clock_font_color.r);
  output_log(LOGLEVEL_DEBUG, "%s G: %i\n", SETTING_CLOCK_FONT_COLOR, config.clock_font_color.g);
  output_log(LOGLEVEL_DEBUG, "%s B: %i\n", SETTING_CLOCK_FONT_COLOR, config.clock_font_color.b);
  output_log(LOGLEVEL_DEBUG, "%s A: %i\n", SETTING_CLOCK_FONT_COLOR, config.clock_font_color.a);
  if (config.clock_shadows) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_CLOCK_SHADOWS, "true");
  }
  else {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_CLOCK_SHADOWS, "false");
  }
  output_log(LOGLEVEL_DEBUG, "%s R: %i\n", SETTING_CLOCK_SHADOW_COLOR, config.clock_shadow_color.r);
  output_log(LOGLEVEL_DEBUG, "%s G: %i\n", SETTING_CLOCK_SHADOW_COLOR, config.clock_shadow_color.g);
  output_log(LOGLEVEL_DEBUG, "%s B: %i\n", SETTING_CLOCK_SHADOW_COLOR, config.clock_shadow_color.b);
  output_log(LOGLEVEL_DEBUG, "%s A: %i\n", SETTING_CLOCK_SHADOW_COLOR, config.clock_shadow_color.a);
  if (config.clock_time_format == FORMAT_TIME_12HR) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_CLOCK_TIME_FORMAT, "12hr");
  }
  else if (config.clock_time_format == FORMAT_TIME_24HR) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_CLOCK_TIME_FORMAT, "24hr");
  }
  else {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_CLOCK_TIME_FORMAT, "Auto");
  }
  if (config.clock_date_format == FORMAT_DATE_LITTLE) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_CLOCK_DATE_FORMAT, "Little");
  }
  else if (config.clock_date_format == FORMAT_DATE_BIG) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_CLOCK_DATE_FORMAT, "Big");
  }
  else {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_CLOCK_DATE_FORMAT, "Auto");
  }
  if (config.clock_include_weekday) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_CLOCK_INCLUDE_WEEKDAY, "true");
  }
  else {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_CLOCK_INCLUDE_WEEKDAY, "false");
  }

  output_log(LOGLEVEL_DEBUG, "======================= Screensaver =======================\n");
  if (config.screensaver_enabled) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n",SETTING_SCREENSAVER_ENABLED, "true");
  }
  else {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n",SETTING_SCREENSAVER_ENABLED, "false");
  }
  output_log(LOGLEVEL_DEBUG, "%s: %i\n", SETTING_SCREENSAVER_IDLE_TIME, config.screensaver_idle_time / 1000);
  if(strlen(config.screensaver_intensity_str)) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n",SETTING_SCREENSAVER_INTENSITY, config.screensaver_intensity_str);
  }
  else {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n",SETTING_SCREENSAVER_INTENSITY, DEFAULT_SCREENSAVER_INTENSITY);
  }
  if (config.screensaver_pause_slideshow) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n",SETTING_SCREENSAVER_PAUSE_SLIDESHOW, "true");
  }
  else {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n",SETTING_SCREENSAVER_PAUSE_SLIDESHOW, "false");
  }
}

// A function to print the parsed menu entries to the command line
void debug_menu_entries(menu_t *first_menu, int num_menus) 
{
  if (first_menu == NULL) {
    output_log(LOGLEVEL_DEBUG, "No valid menus found\n");
    return;
  }
  output_log(LOGLEVEL_DEBUG, "======================= Menu Entries =======================\n");
  menu_t *menu = first_menu;
  entry_t *entry;
  for (int i = 0; i < num_menus; i ++) {
    output_log(LOGLEVEL_DEBUG, "Menu Name: %s\n",menu->name);
    output_log(LOGLEVEL_DEBUG, "Number of Entries: %i\n",menu->num_entries);
    entry = menu->first_entry;
    for (int j = 0; j < menu->num_entries; j++) {
      output_log(LOGLEVEL_DEBUG, "Entry %i Title: %s\n",j,entry->title);
      output_log(LOGLEVEL_DEBUG, "Entry %i Icon Path: %s\n",j,entry->icon_path);
      output_log(LOGLEVEL_DEBUG, "Entry %i Command: %s\n",j,entry->cmd);
      if (j != menu->num_entries - 1) {
        output_log(LOGLEVEL_DEBUG, "\n");
      }
      entry = entry->next;
    }
    if (i != num_menus - 1) {
      output_log(LOGLEVEL_DEBUG, "----------------------------------------------------------\n");
    }
    menu = menu->next;
  }
  output_log(LOGLEVEL_DEBUG, "\n");
}

void debug_gamepad(gamepad_control_t *gamepad_controls)
{
  output_log(LOGLEVEL_DEBUG, "======================== Gamepad ========================\n");
  if (config.gamepad_enabled) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n",SETTING_GAMEPAD_ENABLED, "true");
  }
  else {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n",SETTING_GAMEPAD_ENABLED, "false");
  }
  output_log(LOGLEVEL_DEBUG, "%s: %i\n", SETTING_GAMEPAD_DEVICE, config.gamepad_device);
  if (config.gamepad_mappings_file != NULL) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", SETTING_GAMEPAD_MAPPINGS_FILE, config.gamepad_mappings_file);
  }
  for (gamepad_control_t *i = gamepad_controls; i != NULL; i = i->next) {
    output_log(LOGLEVEL_DEBUG, "%s: %s\n", i->label, i->cmd);
  }
}

void debug_hotkeys(hotkey_t *hotkeys)
{
  if (hotkeys == NULL) {
    output_log(LOGLEVEL_DEBUG, "No hotkeys detected\n");
    return;
  }
  output_log(LOGLEVEL_DEBUG, "======================== Hotkeys =========================\n");
  int index = 0;
  for (hotkey_t *i = hotkeys; i != NULL; i = i->next) {
    output_log(LOGLEVEL_DEBUG, "Hotkey %i Keycode: %X\n", index, i->keycode);
    output_log(LOGLEVEL_DEBUG, "Hotkey %i Command: %s\n", index, i->cmd);
    index++;
  }
}

// A function to debug the parsed slideshow files
void debug_slideshow(slideshow_t *slideshow)
{
  output_log(LOGLEVEL_DEBUG, "======================== Slideshow ========================\n");
  output_log(LOGLEVEL_DEBUG, 
             "Found %i images in directory %s:\n", 
             slideshow->num_images, 
             config.slideshow_directory);
  for (int i = 0; i < slideshow->num_images; i++) {
    output_log(LOGLEVEL_DEBUG, "%s\n", slideshow->images[slideshow->order[i]]);
  }
}

// A function to debug the video settings
void debug_video(SDL_Renderer *renderer, SDL_DisplayMode *display_mode)
{
  output_log(LOGLEVEL_DEBUG, "===================== Video Information =====================\n");
  output_log(LOGLEVEL_DEBUG, "Resolution: %ix%i\n", display_mode->w, display_mode->h);
  output_log(LOGLEVEL_DEBUG, "Refresh rate: %i Hz\n", display_mode->refresh_rate);
  output_log(LOGLEVEL_DEBUG, "Video driver: %s\n", SDL_GetCurrentVideoDriver());
  SDL_RendererInfo info;
  SDL_GetRendererInfo(renderer, &info);
  output_log(LOGLEVEL_DEBUG, "Supported Texture formats:\n");
  for(int i = 0; i < info.num_texture_formats; i++) {
    output_log(LOGLEVEL_DEBUG, "%s\n", SDL_GetPixelFormatName(info.texture_formats[i]));
  }
}

