#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_syswm.h>
#include "launcher.h"
#include <launcher_config.h>
#include "util.h"
#include "debug.h"
#include "platform/platform.h"
#include "external/ini.h"

extern config_t          config;
extern gamepad_control_t *gamepad_controls;
extern hotkey_t          *hotkeys;
menu_t                   *menu  = NULL;
entry_t                  *entry = NULL;

// A function to handle the arguments from the command line
void handle_arguments(int argc, char *argv[], char **config_file_path)
{
  // Parse command line arguments
  if (argc > 1) {
    bool version = false;
    bool help = false;
    int config_file_index = -1;
    for (int i = 1; i < argc; i++) {

      // Current argument is config file path if -c or --config was previous argument
      if (i == config_file_index && *config_file_path == NULL) {
        if (file_exists(argv[i])) {
          copy_string_alloc(config_file_path, argv[i]);
        }
        else {
          output_log(LOGLEVEL_FATAL, "Fatal Error: Config file %s not found\n", argv[i]);
        }
      }
      if (!strcmp(argv[i],"-c") || !strcmp(argv[i],"--config")) {
        config_file_index = i + 1;
      }
      else if (!strcmp(argv[i],"-d") || !strcmp(argv[i],"--debug")) {
        config.debug = true;
      }
      #ifdef __unix__
      else if (!strcmp(argv[i],"-h") || !strcmp(argv[i],"--help")) {
        help = true;
      }
      else if (!strcmp(argv[i],"-v") || !strcmp(argv[i],"--version")) {
        version = true;
      }
      #endif
      else if (i != config_file_index) {
        #ifdef __unix__
        fprintf(stderr, "Unrecognized option %s\n",argv[i]);
        print_usage();
        #endif
        quit(EXIT_FAILURE);
      }
    }

    // Check version, help flags
    #ifdef __unix__
    if (version) {
      print_version();
      quit(EXIT_SUCCESS);
    }
    if (help) {
      print_usage();
      quit(EXIT_SUCCESS);
    }
    #endif
  }

  // Try to find config file if none is specified on the command line
  if (*config_file_path == NULL) {
    #ifdef __unix__
    char *prefixes[4];
    char home_config_buffer[MAX_PATH_CHARS + 1];
    prefixes[0] = CURRENT_DIRECTORY;
    prefixes[1] = config.exe_path;
    prefixes[2] = join_paths(home_config_buffer, sizeof(home_config_buffer), 3, getenv("HOME"), ".config", EXECUTABLE_TITLE);
    prefixes[3] = PATH_CONFIG_SYSTEM;
    *config_file_path = find_file(FILENAME_DEFAULT_CONFIG, 4, prefixes);
    #else
    char *prefixes[2];
    prefixes[0] = CURRENT_DIRECTORY;
    prefixes[1] = config.exe_path;
    *config_file_path = find_file(FILENAME_DEFAULT_CONFIG, 2, prefixes);
    #endif

    if (*config_file_path == NULL) {
      output_log(LOGLEVEL_FATAL, "Fatal Error: No config file found\n");
    }
  }
  output_log(LOGLEVEL_DEBUG, "Config file found: %s\n", *config_file_path);
}

// A function to parse the config file and store the settings into the config struct
void parse_config_file(const char *config_file_path)
{
  FILE *file = fopen(config_file_path, "r");
  if (file == NULL) {
    output_log(LOGLEVEL_FATAL, "Fatal Error: Could not open config file\n");
  }
  int error = ini_parse_file(file, config_handler, &config);
  fclose(file);
  
  if (error < 0) {
    output_log(LOGLEVEL_FATAL, "Fatal Error: Could not parse config file\n");
  }
}

// A function to handle config file parsing
int config_handler(void *user, const char *section, const char *name, const char *value)
{
  config_t *pconfig = (config_t*) user;

  // Parse settings
  if (!strcmp(section,"Settings")) {
    if (!strcmp(name,SETTING_BACKGROUND_IMAGE)) {
      copy_string_alloc(&pconfig->background_image, value);
      clean_path(pconfig->background_image);
    }
    else if (!strcmp(name,SETTING_TITLE_FONT)) {
      copy_string_alloc(&pconfig->title_font_path, value);
      clean_path(pconfig->title_font_path);
    }
    else if (!strcmp(name,SETTING_TITLE_FONT_SIZE)) {
      pconfig->title_font_size = (unsigned int) atoi(value);
    }
    else if (!strcmp(name,SETTING_TITLE_FONT_COLOR)) {
      hex_to_color(value, &pconfig->title_font_color);
    }
    else if (!strcmp(name, SETTING_TITLE_SHADOWS)) {
      pconfig->title_shadows = convert_bool(value, DEFAULT_TITLE_SHADOWS);
    }
    else if (!strcmp(name, SETTING_TITLE_SHADOW_COLOR)) {
      hex_to_color(value, &pconfig->title_shadow_color);
    }
    else if (!strcmp(name,SETTING_BACKGROUND_MODE)) {
      if (!strcmp(value, "Image")) {
        pconfig->background_mode = MODE_IMAGE;
      }
      else if (!strcmp(value, "Slideshow")) {
        pconfig->background_mode = MODE_SLIDESHOW;
      }
      else {
        pconfig->background_mode = MODE_COLOR;
      }
    }
    else if (!strcmp(name,SETTING_BACKGROUND_COLOR)) {
      hex_to_color(value, &pconfig->background_color);
    }
    else if (!strcmp(name,SETTING_SLIDESHOW_DIRECTORY)) {
      copy_string_alloc(&pconfig->slideshow_directory, value);
      clean_path(pconfig->slideshow_directory);
    }
    else if (!strcmp(name, SETTING_BACKGROUND_OVERLAY)) {
      pconfig->background_overlay = convert_bool(value, DEFAULT_BACKGROUND_OVERLAY);
    }
    else if (!strcmp(name,SETTING_BACKGROUND_OVERLAY_COLOR)) {
      hex_to_color(value, &pconfig->background_overlay_color);
    }
    else if (!strcmp(name, SETTING_BACKGROUND_OVERLAY_OPACITY)) {
      if (is_percent(value)) {
        copy_string(pconfig->background_overlay_opacity, value, sizeof(pconfig->background_overlay_opacity));
      }
    }
    else if (!strcmp(name,SETTING_ICON_SIZE)) {
      Uint16 icon_size = (Uint16) atoi(value);
      if (icon_size >= MIN_ICON_SIZE && icon_size <= MAX_ICON_SIZE) {
        pconfig->icon_size = icon_size;
      }
    }
    else if (!strcmp(name, SETTING_DEFAULT_MENU)) {
      copy_string_alloc(&pconfig->default_menu, value);
    }
    else if (!strcmp(name, SETTING_HIGHLIGHT_FILL_COLOR)) {
      hex_to_color(value, &pconfig->highlight_fill_color);
    }
    else if (!strcmp(name, SETTING_HIGHLIGHT_OUTLINE_COLOR)) {
      hex_to_color(value, &pconfig->highlight_outline_color);
    }
    else if (!strcmp(name, SETTING_HIGHLIGHT_OUTLINE_SIZE)) {
      int highlight_outline_size = atoi(value);
      if (highlight_outline_size >= 0) {
        pconfig->highlight_outline_size = highlight_outline_size;
      }
    }
    else if (!strcmp(name,SETTING_HIGHLIGHT_CORNER_RADIUS)) {
      Uint16 rx = (Uint16) atoi(value);
      if (rx >= MIN_RX_SIZE && rx <= MAX_RX_SIZE) {
        pconfig->highlight_rx = rx;
      }
    }
    else if (!strcmp(name,SETTING_TITLE_PADDING)) {
      int title_padding = atoi(value);
      if (title_padding >= 0) {
        pconfig->title_padding = (unsigned int) title_padding;
      }
    }
    else if (!strcmp(name,SETTING_MAX_BUTTONS)) {
      int max_buttons = atoi(value);
      if (max_buttons > 0) {
        pconfig->max_buttons = (unsigned int) max_buttons;
      }
    }
    else if (!strcmp(name, SETTING_ICON_SPACING)) {
      if (is_percent(value)) {
        copy_string(pconfig->icon_spacing_str, value, sizeof(pconfig->icon_spacing_str));
      }
      else {
        int icon_spacing = atoi(value);
        if (icon_spacing > 0 || !strcmp(value,"0")) {
          pconfig->icon_spacing = icon_spacing;
        }
      }
    }
    else if (!strcmp(name,SETTING_HIGHLIGHT_VPADDING)) {
      int highlight_vpadding = atoi(value);
      if (highlight_vpadding > 0 || !strcmp(value,"0")) {
        pconfig->highlight_vpadding = highlight_vpadding;
      }
    }
    else if (!strcmp(name,SETTING_HIGHLIGHT_HPADDING)) {
      int highlight_hpadding = atoi(value);
      if (highlight_hpadding > 0 || !strcmp(value,"0")) {
        pconfig->highlight_hpadding = highlight_hpadding;
      }
    }
    else if (!strcmp(name, SETTING_SLIDESHOW_IMAGE_DURATION)) {
      Uint32 slideshow_image_duration = ((Uint32) atoi(value))*1000;
      if (slideshow_image_duration >= MIN_SLIDESHOW_IMAGE_DURATION && 
      slideshow_image_duration <= MAX_SLIDESHOW_IMAGE_DURATION) {
        pconfig->slideshow_image_duration = slideshow_image_duration;
      }
    }
    else if (!strcmp(name, SETTING_SLIDESHOW_TRANSITION_TIME)) {
      Uint32 slideshow_transition_time = (Uint32) (atof(value)*1000.0f);
      if (slideshow_transition_time >= MIN_SLIDESHOW_TRANSITION_TIME && 
      slideshow_transition_time <= MAX_SLIDESHOW_TRANSITION_TIME) {
        pconfig->slideshow_transition_time = slideshow_transition_time;
      }
    }
    else if (!strcmp(name, SETTING_TITLE_OPACITY)) {
      if (is_percent(value)) {
        copy_string(pconfig->title_opacity, value, sizeof(pconfig->title_opacity));
      }
    }
    else if (!strcmp(name, SETTING_HIGHLIGHT_FILL_OPACITY)) {
      if (is_percent(value)) {
        copy_string(pconfig->highlight_fill_opacity, value, sizeof(pconfig->highlight_fill_opacity));
      }
    }
    else if (!strcmp(name, SETTING_HIGHLIGHT_OUTLINE_OPACITY)) {
      if (is_percent(value)) {
        copy_string(pconfig->highlight_outline_opacity, value, sizeof(pconfig->highlight_outline_opacity));
      }
    }
    else if (!strcmp(name, SETTING_BUTTON_CENTERLINE)) {
      if (is_percent(value)) {
        copy_string(pconfig->button_centerline, value, sizeof(pconfig->button_centerline));
      }
    }
    else if (!strcmp(name,SETTING_SCROLL_INDICATORS)) {
      pconfig->scroll_indicators = convert_bool(value, DEFAULT_SCROLL_INDICATORS);
    }
    else if (!strcmp(name,SETTING_SCROLL_INDICATOR_COLOR)) {
      hex_to_color(value, &pconfig->scroll_indicator_color);
    }
    else if (!strcmp(name,SETTING_SCROLL_INDICATOR_OPACITY)) {
      if (is_percent(value)) {
        copy_string(pconfig->scroll_indicator_opacity, value, sizeof(pconfig->scroll_indicator_opacity));
      }
    }
    else if (!strcmp(name,SETTING_TITLE_OVERSIZE_MODE)) {
      if (!strcmp(value,"Shrink")) {
        pconfig->title_oversize_mode = MODE_SHRINK;
      }
      else if (!strcmp(value,"None")) {
        pconfig->title_oversize_mode = MODE_NONE;
      }
    }
    else if (!strcmp(name, SETTING_ON_LAUNCH)) {
      if (!strcmp(value, "None")) {
        pconfig->on_launch = MODE_NONE;
      }
      else if (!strcmp(value, "Blank")) {
        pconfig->on_launch = MODE_BLANK;
      }
      else {
        pconfig->on_launch = MODE_HIDE;
      }
    }
    else if (!strcmp(name, SETTING_RESET_ON_BACK)) {
      pconfig->reset_on_back = convert_bool(value, DEFAULT_RESET_ON_BACK);
    }
    else if (!strcmp(name, SETTING_MOUSE_SELECT)) {
      pconfig->mouse_select = convert_bool(value, DEFAULT_MOUSE_SELECT);
    }
  }

  // Parse clock settings
  else if (!strcmp(section, "Clock")) {
    if (!strcmp(name, SETTING_CLOCK_ENABLED)) {
      pconfig->clock_enabled = convert_bool(value, DEFAULT_CLOCK_ENABLED);
    }
    else if (!strcmp(name, SETTING_CLOCK_SHOW_DATE)) {
      pconfig->clock_show_date = convert_bool(value, DEFAULT_CLOCK_SHOW_DATE);
    }
    else if (!strcmp(name, SETTING_CLOCK_ALIGNMENT)) {
      if(!strcmp(value, "Left")) {
        pconfig->clock_alignment = ALIGNMENT_LEFT;
      }
      else if (!strcmp(value, "Right")) {
        pconfig->clock_alignment = ALIGNMENT_RIGHT;
      }
    }
    else if (!strcmp(name, SETTING_CLOCK_FONT)) {
      copy_string_alloc(&pconfig->clock_font_path, value);
      clean_path(pconfig->clock_font_path);
    }
    else if (!strcmp(name, SETTING_CLOCK_MARGIN)) {
      if (is_percent(value)) {
        copy_string(pconfig->clock_margin_str, value, sizeof(pconfig->clock_margin_str));
      }
      else {
        int clock_margin = atoi(value);
        if (clock_margin > 0 || !strcmp(value,"0")) {
          pconfig->clock_margin = clock_margin;
        }
      }
    }
    else if (!strcmp(name, SETTING_CLOCK_FONT_COLOR)) {
      hex_to_color(value, &pconfig->clock_font_color);
    }
    else if (!strcmp(name, SETTING_CLOCK_SHADOW_COLOR)) {
      hex_to_color(value, &pconfig->clock_shadow_color);
    }
    else if (!strcmp(name, SETTING_CLOCK_SHADOWS)) {
      pconfig->clock_shadows = convert_bool(value, DEFAULT_CLOCK_SHADOWS);
    }
    else if (!strcmp(name, SETTING_CLOCK_OPACITY)) {
      if (is_percent(value)) {
        copy_string(pconfig->clock_opacity, value, sizeof(pconfig->clock_opacity));
      }
    }
    else if (!strcmp(name, SETTING_CLOCK_FONT_SIZE)) {
      unsigned int font_size = (unsigned int) atoi(value);
      if (font_size) {
        pconfig->clock_font_size = font_size;
      }
    }
    else if (!strcmp(name, SETTING_CLOCK_TIME_FORMAT)) {
      if (!strcmp(value, "12hr")) {
        pconfig->clock_time_format = FORMAT_TIME_12HR;
      }
      else if (!strcmp(value, "24hr")) {
        pconfig->clock_time_format = FORMAT_TIME_24HR;
      }
    }
    else if (!strcmp(name, SETTING_CLOCK_DATE_FORMAT)) {
      if (!strcmp(value, "Little")) {
        pconfig->clock_date_format = FORMAT_DATE_LITTLE;
      }
      else if (!strcmp(value, "Big")) {
        pconfig->clock_date_format = FORMAT_DATE_BIG;
      }
    }
    else if (!strcmp(name, SETTING_CLOCK_INCLUDE_WEEKDAY)) {
      pconfig->clock_include_weekday = convert_bool(value, DEFAULT_CLOCK_INCLUDE_WEEKDAY);
    }
  }

  // Parse screensaver setttings
  else if (!strcmp(section, "Screensaver")) {
    if (!strcmp(name, SETTING_SCREENSAVER_ENABLED)) {
      pconfig->screensaver_enabled = convert_bool(value, DEFAULT_SCREENSAVER_ENABLED);
    }
    else if (!strcmp(name, SETTING_SCREENSAVER_IDLE_TIME)) {
      Uint32 screensaver_idle_time = (Uint32) atoi(value);
      if (screensaver_idle_time >= MIN_SCREENSAVER_IDLE_TIME &&
      screensaver_idle_time <= MAX_SCREENSAVER_IDLE_TIME) {
        pconfig->screensaver_idle_time = screensaver_idle_time*1000; // Convert to ms
      }
    }
    else if (!strcmp(name, SETTING_SCREENSAVER_INTENSITY)) {
      if (is_percent(value)) {
        copy_string(pconfig->screensaver_intensity_str, value, sizeof(pconfig->screensaver_intensity_str));
      }
    }
    else if (!strcmp(name, SETTING_SCREENSAVER_PAUSE_SLIDESHOW)) {
      pconfig->screensaver_pause_slideshow = convert_bool(value, DEFAULT_SCREENSAVER_PAUSE_SLIDESHOW);
    }
  }
  
  // Parse hotkeys
  else if (!strcmp(section, "Hotkeys")) {
    char *keycode = strtok(value, ";");
    if (keycode != NULL) {
      char *cmd = strtok(NULL, "");
      if (cmd != NULL) {
        add_hotkey(keycode, cmd);
      }
    }
  }

  // Parse gamepad settings
  else if (!strcmp(section, "Gamepad")) {
    if (!strcmp(name, SETTING_GAMEPAD_ENABLED)) {
      pconfig->gamepad_enabled = convert_bool(value, DEFAULT_GAMEPAD_ENABLED);
    }
    else if (!strcmp(name, SETTING_GAMEPAD_DEVICE)) {
      int device_index = atoi(value);
      if (device_index >= 0) {
        pconfig->gamepad_device = device_index;
      }
    }
    else if (!strcmp(name, SETTING_GAMEPAD_MAPPINGS_FILE)) {
      copy_string_alloc(&pconfig->gamepad_mappings_file, value);
      clean_path(pconfig->gamepad_mappings_file);
    }

    // Parse gamepad controls
    else {
      add_gamepad_control(name, value);
    }
  }

  // Parse menus/entries
  else {
    entry_t *previous_entry;

    // Check if menu struct exists for current section
    if (pconfig->first_menu == NULL) {
      pconfig->first_menu = create_menu(section, &pconfig->num_menus);
      menu = pconfig->first_menu;
    }
    else {
      bool menu_exists = false;
      for (menu_t *tmp = pconfig->first_menu; tmp != NULL;
      tmp = tmp->next) {
        if (!strcmp(tmp->name,section)) {
          menu_exists = true;
          break;
        }
      }

    // Create menu if it doesn't already exist
      if (menu_exists == false) {
        menu->next = create_menu(section, &pconfig->num_menus);
        menu = menu->next;
      }
    }

    // Parse entry line for title, icon path, command
    char *string = (char*) value;
    char *token;
    char *delimiter = ";";
    token = strtok(string, delimiter);
    if (token != NULL) {

      // Create first entry in the menu if none exists
      if (menu->first_entry == NULL) {
        menu->first_entry = malloc(sizeof(entry_t));
        entry = menu->first_entry;
        entry->next = NULL;
      }

      // Add entry to the end of the linked list
      else {
        previous_entry = entry;
        entry = entry->next;
        entry = malloc(sizeof(entry_t));
        previous_entry->next = entry;
        entry->next = NULL;
      }
      entry->title_offset = 0;
    }

    // Store data in entry struct
    int i;
    for (i = 0;i < 3 && token != NULL; i++) {
        if (i == 0) {
          copy_string_alloc(&entry->title, token);
        }
        else if (i == 1) {
          copy_string_alloc(&entry->icon_path, token);
          clean_path(entry->icon_path);
          delimiter = "";
        }
        else if (i == 2){
          copy_string_alloc(&entry->cmd, token);
        }
        token = strtok(NULL, delimiter);
    }

    // Delete entry if parse failed to find 3 valid tokens
    if (i != 3 || !strcmp(":select", entry->cmd)) {
      if (menu->num_entries == 0) {
        free(menu->first_entry);
        menu->first_entry = NULL;
      }
      else {
        free(entry);
        entry = previous_entry;
        entry->next = NULL;
      }
    }
    else {
      if (menu->num_entries == 0) {
        entry->previous = NULL;
      }
      else {
        entry->previous = previous_entry;
      }
      menu->num_entries++;
    }
  }
  return 0;
}

// A function to determine if a string is a percent value
bool is_percent(const char *string)
{
  int length = strlen(string);
  if (length > 0 && length < PERCENT_MAX_CHARS && strchr(string, '%') == string + length - 1) {
    return true;
  }
  else {
    return false;
  }
}

// A function to remove quotation marks that enclose a path
// because SDL cannot handle them
void clean_path(char *path)
{
  int length = strlen(path);
  if (length >= 3 && path[0] == '"' && path[length - 1] == '"') {
    path[length - 1] = '\0';
    for (int i = 1; i <= length; i++) {
      *(path + i - 1) = *(path + i);
    }
  }  
}

// A function to convert a hex-formatted string into a color struct
bool hex_to_color(const char *string, SDL_Color *color)
{

  // If strtoul returned 0, and the hex string wasn't 000..., then there was an error
  int length = strlen(string);
  Uint32 hex = (Uint32) strtoul(string, NULL, 16);
  if ((!hex && strcmp(string,"00000000")) || 
  (!hex && strcmp(string,"000000")) || 
  (length != 6 && length != 8)) {
    return false;
  }

  // Convert int to SDL_Color struct via bitwise logic
  if (length == 8) {
    color->r = (Uint8) (hex >> 24);
    color->g = (Uint8) ((hex & 0x00ff0000) >> 16);
    color->b = (Uint8) ((hex & 0x0000ff00) >> 8);
    return true;
  }
  else if (length == 6) {
    color->r = (Uint8) (hex >> 16);
    color->g = (Uint8) ((hex & 0x0000ff00) >> 8);
    color->b = (Uint8) (hex & 0x000000ff);
    return true;
  }
  else {
    return false;
  }
}

// A function to convert a string into a bool
bool convert_bool(const char *string, bool default_setting)
{
  if (!strcmp(string, "true")) {
    return true;
  }
  else if (!strcmp(string, "false")) {
    return false;
  }
  else {
    return default_setting;
  }
}

// A function to copy a string into an existing buffer
void copy_string(char *dest, const char *string, size_t size)
{
  strncpy(dest, string, size);
  dest[size - 1] = '\0';
}

// Allocates memory and copies a variable length string
void copy_string_alloc(char **dest, const char *string)
{
  int length = strlen(string);
  if (length) {
    *dest = malloc(sizeof(char)*(length + 1));
    strcpy(*dest, string);
  }
  else {
    *dest = NULL;
  }
}

// A function to join paths together
char *join_paths(char *buffer, size_t bytes, int num_paths, ...)
{
  va_list list;
  char *arg;
  int length;
  va_start(list, num_paths);

  // Add each subdirectory to path
  for (int i = 0; i < num_paths && bytes > 1; i++) {
    arg = va_arg(list, char*);
    length = strlen(arg);
    if (length > bytes - 1) {
      length = bytes - 1;  
    }
    if (i == 0) {
      copy_string(buffer, arg, bytes);
      bytes -= length;
      if (bytes == 1) {
        break;
      }
    }
    else {

      // Don't copy preceding slash if present
      if (*arg == '/' || *arg == '\\') {
        strncat(buffer, arg + 1, bytes - 1);
        bytes -= (length - 1);
      }
      else {
        strncat(buffer, arg, bytes - 1);
        bytes -= length;
      }
    }

    // Add trailing slash if not present, except last argument
    if ((i != num_paths - 1) && bytes > 1 && *(buffer + strlen(buffer) - 1) != '/' &&
    *(buffer + strlen(buffer) - 1) != '\\')  {
      strncat(buffer, PATH_SEPARATOR, bytes - 1);
      bytes -= 1;
    }
  }
  va_end(list);
  return buffer;
}

// A function to find a file from a filename and list of path prefixes
char *find_file(const char *file, int num_prefixes, const char **prefixes)
{
  char buffer[MAX_PATH_CHARS + 1];
  for (int i = 0; i < num_prefixes; i++) {
    if (prefixes[i] != NULL) {
      join_paths(buffer, sizeof(buffer), 2, prefixes[i], file);
      if (file_exists(buffer)) {
        char *output;
        copy_string_alloc(&output, buffer);
        return output;
      }
    }
  }
  return NULL;
}

// Calculates the length of a utf-8 encoded string
int utf8_length(const char *string)
{
  int length = 0;
  char *ptr = string;
  while (*ptr != '\0') {
    // If byte is 0xxxxxxx, then it's a 1 byte (ASCII) char
    if ((*ptr & 0x80) == 0) {
      ptr++;
    }
    // If byte is 110xxxxx, then it's a 2 byte char
    else if ((*ptr & 0xE0) == 0xC0) {
      ptr +=2;
    }
    // If byte is 1110xxxx, then it's a 3 byte char
    else if ((*ptr & 0xF0) == 0xE0) {
      ptr +=3;
    }
    // If byte is 11110xxx, then it's a 4 byte char
    else if ((*ptr & 0xF8) == 0xF0) {
      ptr+=4;    
    }
  length++;
  }
  return length;
}

// A function to truncate a utf-8 encoded string to max number of pixels
void utf8_truncate(char *string, int width, int max_width)
{
  int string_length = utf8_length(string);
  int avg_width = width / string_length;
  int num_chars = max_width / avg_width;
  int spaces = (string_length - num_chars) + 3; // Number of spaces to go back
  char *ptr = string + strlen(string);  // Change to null character of string
  int chars = 0;

  // Go back required number of spaces
  do {
    ptr--;    
    if (!(*ptr & 0x80)) { // ASCII characters have 0 as most significant bit
      chars++;
    }
    else { // Non-ASCII character detected
      do {
        ptr--;
      } while (ptr > string && (*ptr & 0xC0) == 0x80); // Non-ASCII most significant byte begins with 0b11
      chars++;
    }
  } while (chars < spaces);

  // Add "..." to end of string to inform user of truncation
  if (strlen(ptr) > 2) {
    *ptr = '.';
    *(ptr + 1) = '.';
    *(ptr + 2) = '.';
    *(ptr + 3) = '\0';
  }
}

// A function to extract the Unicode code point from the first character in a UTF-8 string
Uint16 get_unicode_code_point(const char *p, int *bytes)
{
  Uint16 result;

  // 1 byte ASCII char
  if ((*p & 0x80) == 0) {
    result = (Uint16) *p;
    *bytes = 1;
  }

  // If byte is 110xxxxx, then it's a 2 byte char
  else if ((*p & 0xE0) == 0xC0) {
    Uint8 byte1 = *p & 0x1F;
    Uint8 byte2 = *(p + 1) & 0x3F;
    result = (Uint16) ((byte1 << 6) + byte2);
    *bytes = 2;
  }

  // If byte is 1110xxxx, then it's a 3 byte char
  else if ((*p & 0xF0) == 0xE0) {
    Uint8 byte1 = *p & 0x0F;
    Uint8 byte2 = *(p + 1) & 0x3F;
    Uint8 byte3 = *(p + 2) & 0x3F;
    result = (Uint16) ((byte1 << 12) + (byte2 << 6) + byte3);
    *bytes = 3;
  }
  else {
    result = 0;
    *bytes = 1;
  }
  return result;
}

// A function to generate an array of random indices
void random_array(int *array, int array_size)
{
  // Fill array with initial indices
  for (int i = 0; i < array_size; i++) {
    array[i] = i;
  }

  // Shuffle array indices randomly, see https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
  srand(time(NULL));
  int j;
  int tmp;
  for (int i = 0; i < array_size - 1; i++) {
    int j = (rand() % (array_size - i)) + i;
    tmp = array[i];
    array[i] = array[j];
    array[j] = tmp;
  }
}

// A function to calculate the total width of all screen objects
unsigned int calculate_width(int buttons, int icon_spacing, int icon_size, int highlight_hpadding)
{
  return (buttons - 1)*icon_spacing + buttons*icon_size + 2*highlight_hpadding;
}

// A function to add a hotkey to the linked list
void add_hotkey(const char *keycode, const char *cmd)
{
  // Convert hex string to binary
  static hotkey_t *current_hotkey = NULL;
  SDL_Keycode code = (SDL_Keycode) strtol(keycode, NULL, 16);

  // Check if exit hotkey for Windows
  #ifdef _WIN32
  if (!strcmp(cmd, SCMD_EXIT)) {
    set_exit_hotkey(code);
    return;
  }
  #endif

  // Create first node if not initialized, else add to end of linked list
  if (current_hotkey == NULL) {
    hotkeys = malloc(sizeof(hotkey_t));
    current_hotkey = hotkeys;
  }
  else {
    current_hotkey->next = malloc(sizeof(hotkey_t));
    current_hotkey = current_hotkey->next;
  }
  current_hotkey->keycode = code;
  copy_string_alloc(&current_hotkey->cmd, cmd);
  current_hotkey->next = NULL;
}

// A function to add a gamepad control to the linked list
static void add_gamepad_control(const char *label, const char *cmd)
{
  if (cmd[0] == '\0') {
    return;
  }
  
  // Table of gamepad info
  static const struct gamepad_info info[] = {
    {SETTING_GAMEPAD_LSTICK_XM,             TYPE_AXIS_NEG, SDL_CONTROLLER_AXIS_LEFTX},
    {SETTING_GAMEPAD_LSTICK_XP,             TYPE_AXIS_POS, SDL_CONTROLLER_AXIS_LEFTX},
    {SETTING_GAMEPAD_LSTICK_YM,             TYPE_AXIS_NEG, SDL_CONTROLLER_AXIS_LEFTY},
    {SETTING_GAMEPAD_LSTICK_YP,             TYPE_AXIS_POS, SDL_CONTROLLER_AXIS_LEFTY},
    {SETTING_GAMEPAD_RSTICK_XM,             TYPE_AXIS_NEG, SDL_CONTROLLER_AXIS_RIGHTX},
    {SETTING_GAMEPAD_RSTICK_XP,             TYPE_AXIS_POS, SDL_CONTROLLER_AXIS_RIGHTX},
    {SETTING_GAMEPAD_RSTICK_YM,             TYPE_AXIS_NEG, SDL_CONTROLLER_AXIS_RIGHTY},
    {SETTING_GAMEPAD_RSTICK_YP,             TYPE_AXIS_POS, SDL_CONTROLLER_AXIS_RIGHTY},
    {SETTING_GAMEPAD_LTRIGGER,              TYPE_AXIS_POS, SDL_CONTROLLER_AXIS_TRIGGERLEFT},
    {SETTING_GAMEPAD_RTRIGGER,              TYPE_AXIS_POS, SDL_CONTROLLER_AXIS_TRIGGERRIGHT},
    {SETTING_GAMEPAD_BUTTON_A,              TYPE_BUTTON,   SDL_CONTROLLER_BUTTON_A},
    {SETTING_GAMEPAD_BUTTON_B,              TYPE_BUTTON,   SDL_CONTROLLER_BUTTON_B},
    {SETTING_GAMEPAD_BUTTON_X,              TYPE_BUTTON,   SDL_CONTROLLER_BUTTON_X},
    {SETTING_GAMEPAD_BUTTON_Y,              TYPE_BUTTON,   SDL_CONTROLLER_BUTTON_Y},
    {SETTING_GAMEPAD_BUTTON_BACK,           TYPE_BUTTON,   SDL_CONTROLLER_BUTTON_BACK},
    {SETTING_GAMEPAD_BUTTON_GUIDE,          TYPE_BUTTON,   SDL_CONTROLLER_BUTTON_GUIDE},
    {SETTING_GAMEPAD_BUTTON_START,          TYPE_BUTTON,   SDL_CONTROLLER_BUTTON_START},
    {SETTING_GAMEPAD_BUTTON_LEFT_STICK,     TYPE_BUTTON,   SDL_CONTROLLER_BUTTON_LEFTSTICK},
    {SETTING_GAMEPAD_BUTTON_RIGHT_STICK,    TYPE_BUTTON,   SDL_CONTROLLER_BUTTON_RIGHTSTICK},
    {SETTING_GAMEPAD_BUTTON_LEFT_SHOULDER,  TYPE_BUTTON,   SDL_CONTROLLER_BUTTON_LEFTSHOULDER},
    {SETTING_GAMEPAD_BUTTON_RIGHT_SHOULDER, TYPE_BUTTON,   SDL_CONTROLLER_BUTTON_RIGHTSHOULDER},
    {SETTING_GAMEPAD_BUTTON_DPAD_UP,        TYPE_BUTTON,   SDL_CONTROLLER_BUTTON_DPAD_UP},
    {SETTING_GAMEPAD_BUTTON_DPAD_DOWN,      TYPE_BUTTON,   SDL_CONTROLLER_BUTTON_DPAD_DOWN},
    {SETTING_GAMEPAD_BUTTON_DPAD_LEFT,      TYPE_BUTTON,   SDL_CONTROLLER_BUTTON_DPAD_LEFT},
    {SETTING_GAMEPAD_BUTTON_DPAD_RIGHT,     TYPE_BUTTON,   SDL_CONTROLLER_BUTTON_DPAD_RIGHT}
  };

  // Find correct gamepad info for label, return if none found
  int i;
  for (i = 0; i < sizeof(info) / sizeof(info[0]); i++) {
    if (!strcmp(info[i].label, label)) {
      break;
    }
  }
  if (i == sizeof(info) / sizeof(info[0])) {
    return;
  }

  // Begin the linked list if none exists
  static gamepad_control_t *current_gamepad_control = NULL;
  if (current_gamepad_control == NULL) {
    gamepad_controls = malloc(sizeof(gamepad_control_t));
    current_gamepad_control = gamepad_controls;
  }

  // Add another node to the linked list
  else {
    current_gamepad_control->next = malloc(sizeof(gamepad_control_t));
    current_gamepad_control = current_gamepad_control->next;
  }

  // Copy the parameters in the struct
  *current_gamepad_control = (gamepad_control_t) { 
    .type   = info[i].type,
    .index  = info[i].index,
    .label  = info[i].label,
    .repeat = 0,
    .next   = NULL
  };
  copy_string_alloc(&current_gamepad_control->cmd, cmd);
}

// A function to convert a string percent setting to an int value
void convert_percent_to_int(char *string, int *result, int max_value)
{
  int length = strlen(string);
  char tmp[PERCENT_MAX_CHARS];
  copy_string(tmp, string, sizeof(tmp));
  tmp[length - 1] = '\0';
  float percent = atof(tmp);
  if (percent >= 0.0F && percent <= 100.0F) {
    *result = (int) ((percent / 100.0F) * (float) max_value);
  }
}

// A function to make sure all settings are in their correct range
void validate_settings(geometry_t *geo)
{
  // Reduce number of buttons if they can't all fit on screen
  if (config.icon_size * config.max_buttons > geo->screen_width) {
    int i;
    for (i = config.max_buttons; i * config.icon_size > geo->screen_width && i > 0; i--);
    output_log(LOGLEVEL_ERROR, "Error: Not enough screen space for %i buttons, reducing to %i\n", 
                               config.max_buttons, 
                               i);
    config.max_buttons = i; 
  }

  // Convert % opacity settings to 0-255
  if (config.title_opacity[0] != '\0') {
    int title_opacity = INVALID_PERCENT_VALUE;
    convert_percent_to_int(config.title_opacity, &title_opacity, 255);
    if (title_opacity != INVALID_PERCENT_VALUE) {
      config.title_font_color.a = (Uint8) title_opacity;
    }
  }
  if (config.background_overlay_opacity[0] != '\0') {
    int background_overlay_opacity = INVALID_PERCENT_VALUE;
    convert_percent_to_int(config.background_overlay_opacity, &background_overlay_opacity, 255);
    if (background_overlay_opacity != INVALID_PERCENT_VALUE) {
      config.background_overlay_color.a = (Uint8) background_overlay_opacity;
    }
  }

  if (config.highlight_fill_opacity[0] != '\0') {
    int highlight_fill_opacity = INVALID_PERCENT_VALUE;
    convert_percent_to_int(config.highlight_fill_opacity, &highlight_fill_opacity, 255);
    if (highlight_fill_opacity != INVALID_PERCENT_VALUE) {
      config.highlight_fill_color.a = (Uint8) highlight_fill_opacity;
    }
  }
  if (config.highlight_outline_opacity[0] != '\0') {
    int highlight_outline_opacity = INVALID_PERCENT_VALUE;
    convert_percent_to_int(config.highlight_outline_opacity, &highlight_outline_opacity, 255);
    if (highlight_outline_opacity != INVALID_PERCENT_VALUE) {
      config.highlight_outline_color.a = (Uint8) highlight_outline_opacity;
    }
  }
  if (config.scroll_indicator_opacity[0] != '\0') {
    int scroll_indicator_opacity = INVALID_PERCENT_VALUE;
    convert_percent_to_int(config.scroll_indicator_opacity, &scroll_indicator_opacity, 255);
    if (scroll_indicator_opacity != INVALID_PERCENT_VALUE) {
      config.scroll_indicator_color.a = (Uint8) scroll_indicator_opacity;
    }
  }
  if (config.clock_opacity[0] != '\0') {
    int clock_opacity = INVALID_PERCENT_VALUE;
    convert_percent_to_int(config.clock_opacity, &clock_opacity, 255);
    if (clock_opacity != INVALID_PERCENT_VALUE) {
      config.clock_font_color.a = (Uint8) clock_opacity;
    }
  }

  // Set default IconSpacing if none is in the config file
  if (config.icon_spacing < 0) {
    int icon_spacing = INVALID_PERCENT_VALUE;
    if (config.icon_spacing_str[0] != '\0') {
      convert_percent_to_int(config.icon_spacing_str, &icon_spacing, geo->screen_width);
    }
    if (icon_spacing == INVALID_PERCENT_VALUE) {
      convert_percent_to_int(DEFAULT_ICON_SPACING, &icon_spacing, geo->screen_width);
    }
    config.icon_spacing = icon_spacing;
  }
  
  // Convert clock margin setting and check limits
  if (config.clock_margin < 0) {
    int clock_margin = INVALID_PERCENT_VALUE;
    if (config.clock_margin_str[0] != '\0') {
      convert_percent_to_int(config.clock_margin_str, &clock_margin, geo->screen_height);
    }
    if (clock_margin == INVALID_PERCENT_VALUE) {
      convert_percent_to_int(DEFAULT_CLOCK_MARGIN, &clock_margin, geo->screen_height);
    }
    config.clock_margin = clock_margin;
  }
  int clock_margin_limit = (int) ((float) geo->screen_height*MAX_CLOCK_MARGIN);
  if (config.clock_margin > clock_margin_limit) {
    config.clock_margin = clock_margin_limit;
  }

  // Reduce highlight hpadding to prevent overlaps
  if (config.highlight_hpadding > (config.icon_spacing / 2)) {
    config.highlight_hpadding = config.icon_spacing / 2;
  }

  // Reduce icon spacing and highlight padding if too large to fit onscreen
  unsigned int required_length = calculate_width(config.max_buttons,
                                                 config.icon_spacing,
                                                 config.icon_size,
                                                 config.highlight_hpadding);
  int highlight_hpadding = config.highlight_hpadding;
  int icon_spacing = config.icon_spacing;
  for (int i = 0; i < 100 && required_length > geo->screen_width; i++) {
    if (highlight_hpadding > 0) {
      highlight_hpadding = (highlight_hpadding * 9) / 10;
    }
    if (icon_spacing > 0) {
      icon_spacing = (icon_spacing * 9) / 10;
    }
    required_length = calculate_width(config.max_buttons,icon_spacing,config.icon_size,highlight_hpadding);
  }
  if (config.highlight_hpadding != highlight_hpadding) {
    output_log(LOGLEVEL_ERROR, 
               "Error: Highlight padding value %i too large to fit screen, shrinking to %i\n",
               config.highlight_hpadding, 
               highlight_hpadding);
    config.highlight_hpadding = highlight_hpadding;
  }
  if (config.icon_spacing != icon_spacing) {
    output_log(LOGLEVEL_ERROR, 
               "Error: Icon spacing value %i too large to fit screen, shrinking to %i\n",
               config.icon_spacing, 
               icon_spacing);
    config.icon_spacing = icon_spacing;
  }

  // Make sure title padding is in valid range
  if (config.title_padding < 0 || config.title_padding > config.icon_size / 2) {
    int title_padding = config.icon_size / 10;
    output_log(LOGLEVEL_ERROR, 
               "Error: Text padding value %i invalid, changing to %i\n",
               config.title_padding, 
               title_padding);
    config.title_padding = title_padding;
  }

  // Calculate y margin for buttons from centerline setting string, check limits
  int button_centerline = INVALID_PERCENT_VALUE;
  int button_height = config.icon_size + config.title_padding + geo->font_height;
  float f_screen_height = (float) geo->screen_height;
  int lower_limit = (int) (MIN_BUTTON_CENTERLINE*f_screen_height);
  int upper_limit = (int) (MAX_BUTTON_CENTERLINE*f_screen_height);

  // Convert percent to int
  if (config.button_centerline[0] != '\0') {
    convert_percent_to_int(config.button_centerline, &button_centerline, geo->screen_height);
  }
  if (button_centerline == INVALID_PERCENT_VALUE) {
    convert_percent_to_int(DEFAULT_BUTTON_CENTERLINE, &button_centerline, geo->screen_height);
  }

  // Check limits, calculate margin
  if (button_centerline < lower_limit) {
    button_centerline = lower_limit;
  }
  else if (button_centerline > upper_limit) {
    button_centerline = upper_limit;
  }
  geo->y_margin = button_centerline - button_height / 2;

  // Max highlight outline
  int max_highlight_outline_size = (config.highlight_hpadding < config.highlight_vpadding) 
                                   ? config.highlight_hpadding : config.highlight_vpadding;
  if (config.highlight_outline_size > max_highlight_outline_size) {
    config.highlight_outline_size = max_highlight_outline_size;
  }

  // Don't allow rounded rectangle with outline due to Nanosvg bug
  if (config.highlight_rx && config.highlight_outline_size) {
    config.highlight_rx = 0;
  }
}

// A function to retreive menu struct from the linked list via the menu name
menu_t *get_menu(char *menu_name)
{
  for (menu_t *menu = config.first_menu; menu != NULL; menu = menu->next) {
    if (!strcmp(menu_name, menu->name)) {
      return menu;
    }
  }
  output_log(LOGLEVEL_ERROR, 
    "Error: Menu \"%s\" not found in config file\n", 
    menu_name
  );
  return NULL;
}

// A function to allocate memory to and initialize a menu struct
menu_t *create_menu(char *menu_name, int *num_menus)
{
  menu_t *menu = malloc(sizeof(menu_t));
  copy_string_alloc(&menu->name, menu_name);  
  menu->first_entry = NULL;
  menu->next = NULL;
  menu->back = NULL;
  menu->root_entry = NULL;
  menu->num_entries = 0;
  menu->page = 0;
  menu->highlight_position = 0;
  menu->rendered = false;
  (*num_menus)++;
  return menu;
}

// A function to advance X spaces in the entry linked list (left or right)
entry_t *advance_entries(entry_t *entry, int spaces, direction_t direction)
{
  if (direction == DIRECTION_LEFT) {
    for (int i = 0; i < spaces; i++) {
      entry = entry->previous;
    }
  }
  else if (direction == DIRECTION_RIGHT) {
    for (int i = 0; i < spaces; i++) {
      entry = entry->next;
    }
  }
  return entry;
}

// A function to read a file into a null-terminated buffer
void read_file(const char *path, char **buffer)
{
  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    output_log(LOGLEVEL_ERROR, "Error: Could not open file\n%s\n");
    return;
  }

  // Allocate buffer
  fseek(file, 0L, SEEK_END);
  int64_t file_size = ftell(file);
  rewind(file);
  *buffer = malloc(file_size + 1);
  if (*buffer == NULL) {
    output_log(LOGLEVEL_ERROR, "Error: Failed to allocate memory for file\n");
    return;
  }

  // Read contents of file into buffer
  int64_t total_bytes_read = 0;
  int64_t current_bytes_read;
  char *p = *buffer;
  do {
    current_bytes_read = fread(p, 1, file_size - total_bytes_read, file);
    total_bytes_read += current_bytes_read;
    p += current_bytes_read;
  } while (total_bytes_read < file_size && current_bytes_read > 0);
  fclose(file);

  if (total_bytes_read != file_size) {
    free(*buffer);
    *buffer = NULL;
  }
  *(*buffer + total_bytes_read) = '\0';
}

// A function to dynamically allocate a buffer for and copy a formatted string
void sprintf_alloc(char **buffer, const char *format, ...)
{
  va_list args1, args2;
  va_start(args1, format);
  va_copy(args2, args1);
  
  int length = vsnprintf(NULL, 0, format, args1);
  if (length) {
    *buffer = malloc(length + 1);
    vsnprintf(*buffer, length + 1, format, args2);
  }
  va_end(args1);
  va_end(args2);
}