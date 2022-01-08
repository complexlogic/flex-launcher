#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <SDL.h>
#include <SDL_syswm.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_thread.h>
#include "launcher.h"
#include <launcher_config.h>
#include "image.h"
#include "util.h"
#include "debug.h"
#ifdef __unix__
#include "platform/unix.h"
#endif
#ifdef _WIN32
#include "platform/win32.h"
#endif


// Initialize default settings
config_t config = {
  .background_image = NULL,
  .slideshow_directory = NULL,
  .title_font_path = NULL,
  .font_size = DEFAULT_FONT_SIZE,
  .title_color.r = DEFAULT_TITLE_COLOR_R,
  .title_color.g = DEFAULT_TITLE_COLOR_G,
  .title_color.b = DEFAULT_TITLE_COLOR_B,
  .title_color.a = DEFAULT_TITLE_COLOR_A,
  .background_mode = MODE_COLOR,
  .background_color.r = DEFAULT_BACKGROUND_COLOR_R,
  .background_color.g = DEFAULT_BACKGROUND_COLOR_G,
  .background_color.b = DEFAULT_BACKGROUND_COLOR_B,
  .background_color.a = 0xFF,
  .icon_size = DEFAULT_ICON_SIZE,
  .default_menu = NULL,
  .highlight_color.r = DEFAULT_HIGHLIGHT_COLOR_R,
  .highlight_color.g = DEFAULT_HIGHLIGHT_COLOR_G,
  .highlight_color.b = DEFAULT_HIGHLIGHT_COLOR_B,
  .highlight_color.a = DEFAULT_HIGHLIGHT_COLOR_A,
  .highlight_rx = DEFAULT_HIGHLIGHT_CORNER_RADIUS,
  .title_padding = -1,
  .max_buttons = DEFAULT_MAX_BUTTONS,
  .icon_spacing = -1,
  .highlight_vpadding = -1,
  .highlight_hpadding = -1,
  .title_opacity[0] = '\0',
  .highlight_opacity[0] = '\0',
  .button_centerline[0] = '\0',
  .icon_spacing_str[0] = '\0',
  .scroll_indicators = DEFAULT_SCROLL_INDICATORS,
  .scroll_indicator_color.r = DEFAULT_SCROLL_INDICATOR_COLOR_R,
  .scroll_indicator_color.g = DEFAULT_SCROLL_INDICATOR_COLOR_G,
  .scroll_indicator_color.b = DEFAULT_SCROLL_INDICATOR_COLOR_B,
  .scroll_indicator_color.a = DEFAULT_SCROLL_INDICATOR_COLOR_A,
  .scroll_indicator_opacity[0] = '\0',
  .title_oversize_mode = MODE_TEXT_TRUNCATE,
  .reset_on_back = DEFAULT_RESET_ON_BACK,
  .screensaver_enabled = false,
  .screensaver_idle_time = DEFAULT_SCREENSAVER_IDLE_TIME*1000,
  .screensaver_intensity_str[0] = '\0',
  .screensaver_pause_slideshow = DEFAULT_SCREENSAVER_PAUSE_SLIDESHOW,
  .gamepad_enabled = DEFAULT_GAMEPAD_ENABLED,
  .gamepad_device = DEFAULT_GAMEPAD_DEVICE,
  .gamepad_mappings_file = NULL,
  .on_launch = MODE_ON_LAUNCH_HIDE,
  .debug = false,
  .exe_path = NULL,
  .first_menu = NULL,
  .gamepad_controls = NULL,
  .num_menus = 0,
  .slideshow_image_duration = DEFAULT_SLIDESHOW_IMAGE_DURATION,
  .slideshow_transition_time = DEFAULT_SLIDESHOW_TRANSITION_TIME
};

state_t state = {
  .screen_updates = false,
  .slideshow_transition = false,
  .slideshow_background_ready = false,
  .slideshow_paused = false,
  .screensaver_active = false,
  .screensaver_transition = false
};

// Global variables
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_SysWMinfo wmInfo;
SDL_DisplayMode display_mode;
SDL_RWops *log_file = NULL;
TTF_Font *title_font = NULL; // Font of the button title text
menu_t *default_menu = NULL;
menu_t *current_menu = NULL; // Current selected menu
entry_t *current_entry = NULL; // Current selected entry
hotkey_t *hotkeys = NULL;
ticks_t ticks;
SDL_GameController *gamepad = NULL;
SDL_Thread *slideshow_thread = NULL;
int delay_period;
int repeat_period; 
scroll_t *scroll = NULL;
slideshow_t *slideshow = NULL;
screensaver_t *screensaver = NULL;
SDL_Texture *background_texture = NULL; // Background texture (image only)
highlight_t *highlight = NULL; // Pointer containing highlight texture and coordinates
geometry_t geo; // Struct containing screen geometry for the current page of buttons

// A function to initialize SDL
int init_sdl()
{  
  // Set flags, hints
  int sdl_flags = SDL_INIT_VIDEO;
  int img_flags = IMG_INIT_PNG | 
                  IMG_INIT_JPG | 
                  IMG_INIT_WEBP;
  #ifdef __unix__
  SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
  #endif
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"1");
  if (config.gamepad_enabled) {
    sdl_flags |= SDL_INIT_GAMECONTROLLER;
    delay_period = GAMEPAD_REPEAT_DELAY / POLLING_PERIOD;
    repeat_period = GAMEPAD_REPEAT_INTERVAL / POLLING_PERIOD; 
  }

  // Initialize SDL
  if (SDL_Init(sdl_flags) < 0)
  {
    output_log(LOGLEVEL_FATAL, 
               "Fatal Error: Could not initialize SDL\n%s\n", 
               SDL_GetError());
    return 1;
  }

  // Create window, hide mouse cursor
  window = SDL_CreateWindow(PROJECT_NAME, SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          0,
                                          0,
                                          SDL_WINDOW_FULLSCREEN_DESKTOP | 
                                          SDL_WINDOW_BORDERLESS);
  if (window == NULL) {
    output_log(LOGLEVEL_FATAL, 
               "Fatal Error: Could not create SDL Window\n%s\n", 
               SDL_GetError());
    return 1;
  }
  SDL_ShowCursor(SDL_DISABLE);

  // Create HW accelerated renderer, get screen resolution
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  SDL_GetCurrentDisplayMode(0, &display_mode);
  geo.screen_width = display_mode.w;
  geo.screen_height = display_mode.h;
  SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
  if (renderer == NULL) {
    output_log(LOGLEVEL_FATAL, 
               "Fatal Error: Could not initialize renderer\n%s\n", 
               SDL_GetError());
    return 1;
  }

  // Set background color
  set_draw_color();

  // Initialize SDL_image
  if (!(IMG_Init(img_flags) & img_flags)) {
    output_log(LOGLEVEL_FATAL, 
               "Fatal Error: Could not initialize SDL_image\n%s\n", 
               IMG_GetError());
    return 1;
  }
  #ifdef _WIN32
  SDL_VERSION(&wmInfo.version);
  SDL_GetWindowWMInfo(window, &wmInfo);
  #endif
  return 0;
}

void set_draw_color()
{
  if (config.background_mode == MODE_COLOR) {
    SDL_SetRenderDrawColor(renderer,
                            config.background_color.r,
                            config.background_color.g,
                            config.background_color.b,
                            config.background_color.a);
  }
  else {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  }
}

// A function to initialize SDL's TTF subsystem
int init_ttf()
{
  // Initialize SDL_ttf
  if (TTF_Init() == -1) {
    output_log(LOGLEVEL_FATAL, 
               "Fatal Error: Could not initialize SDL_ttf\n%s\n", 
               TTF_GetError());
    return 1;
   }

  // Load user specified font
  if (config.title_font_path != NULL) {
    title_font = TTF_OpenFont(config.title_font_path, config.font_size);
  }

  // Try to find default font if user specified font is not found
  if (title_font == NULL){
    output_log(LOGLEVEL_ERROR, "Error: Could not initialize font from config file\n");
    char *prefixes[2];
    char fonts_exe_buffer[MAX_PATH_CHARS + 1];
    prefixes[0] = join_paths(fonts_exe_buffer, 3, config.exe_path, PATH_ASSETS_EXE, PATH_FONTS_EXE);
    #ifdef __unix__
    prefixes[1] = PATH_FONTS_SYSTEM;
    #else
    prefixes[1] = PATH_FONTS_RELATIVE;
    #endif
    char *default_font = find_file(FILENAME_DEFAULT_FONT, 2, prefixes);

    // Replace user font with default in config
    if (default_font != NULL) {
      title_font = TTF_OpenFont(default_font,config.font_size);
      free(config.title_font_path);
      copy_string(&config.title_font_path, default_font);
    }
    if(title_font == NULL) {
      output_log(LOGLEVEL_FATAL, "Fatal Error: Could not load default font\n");
      return 1;
    }
  }

  // Get font height for geometry calculations
  TTF_SizeUTF8(title_font,"TEST STRING",NULL,&geo.font_height);
  return 0;
}

// A function to close subsystems and free memory before quitting
void cleanup()
{
  // Destroy renderer and window
  if (renderer != NULL) {
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
  }
  if (window != NULL) {
    SDL_DestroyWindow(window);
    window = NULL;
  }

  // Quit subsystems
  SDL_Quit();
  IMG_Quit();
  TTF_Quit();
  quit_svg();

  // Close log file if open
  if (log_file != NULL) {
    SDL_RWclose(log_file);
  }

  // Free dynamically allocated memory
  free(config.default_menu);
  free(config.background_image);
  free(config.title_font_path);
  free(config.exe_path);
  free(highlight);
  free(scroll);

  if (config.background_mode == MODE_SLIDESHOW) {
    quit_slideshow();
  }
  if (config.screensaver_enabled) {
    free(screensaver);
  }

  // Free menu and entry linked lists
  entry_t *entry = NULL;
  entry_t *tmp_entry = NULL;
  menu_t *menu = config.first_menu;
  menu_t *tmp_menu = NULL;
  for (int i = 0; i < config.num_menus; i++) {
    free(menu->name);
    entry = menu->first_entry;
    for(int j = 0; j < menu->num_entries; j++) {
      free(entry->title);
      free(entry->icon_path);
      free(entry->cmd);
      tmp_entry = entry;
      entry = entry->next;
      free(tmp_entry);
    }
    tmp_menu = menu;
    menu = menu->next;
    free(tmp_menu);
  }

  // Free hotkeys

  hotkey_t *tmp_hotkey = NULL;
  for(hotkey_t *i = hotkeys; i != NULL; i = i->next) {
    free(tmp_hotkey);
    free(i->cmd);
    tmp_hotkey = i;
  }
  free(tmp_hotkey);

  // Free gamepad control linked list
  gamepad_control_t *tmp_gamepad = NULL;
  for (gamepad_control_t *i = config.gamepad_controls; i != NULL; i = i->next) {
    free(tmp_gamepad);
    free(i->label);
    free(i->cmd);
    tmp_gamepad = i;
  }
  free(tmp_gamepad);
}

// A function to handle key presses from keyboard
void handle_keypress(SDL_Keysym *key)
{
  if (config.debug) {
    output_log(LOGLEVEL_DEBUG, 
               "Key %s (%X) detected\n", 
               SDL_GetKeyName(key->sym),
               key->sym);
  }

  // Check default keys
  if (key->sym == SDLK_LEFT) {
    move_left();
  }
  else if (key->sym == SDLK_RIGHT) {
    move_right();
  }
  else if (key->sym == SDLK_RETURN) {
    output_log(LOGLEVEL_DEBUG, 
               "Selected Entry:\n"
               "Title: %s\n"
               "Icon Path: %s\n"
               "Command: %s\n", 
               current_entry->title, 
               current_entry->icon_path, 
               current_entry->cmd);
    
    execute_command(current_entry->cmd);
  }
  else if (key->sym == SDLK_BACKSPACE) {
    load_back_menu(current_menu);
  }

  //Check hotkeys
  else {
    for (hotkey_t *i = hotkeys; i != NULL; i = i->next) {
      if (key->sym == i->keycode) {
        execute_command(i->cmd);
        break;
      }
    }
  }
}

// A function to calculate the total width of all screen objects
unsigned int calculate_width(int buttons, int icon_spacing, int icon_size, int highlight_hpadding)
{
  return (buttons - 1)*icon_spacing + buttons*icon_size + 2*highlight_hpadding;
}

// A function to quit the slideshow mode in case of error or program exit
void quit_slideshow()
{
  // Free allocated image paths
  for (int i = 0; i < slideshow->num_images; i++) {
    free(slideshow->images[i]);
  }
  free(slideshow);
}

// A function to initialize the slideshow background mode
void init_slideshow()
{
  if (!directory_exists(config.slideshow_directory)) {
    output_log(LOGLEVEL_ERROR, 
               "Error: Slideshow directory %s does not exist\n"
               "Switching to color background mode\n",
               config.slideshow_directory);
    config.background_mode = MODE_COLOR;
    set_draw_color();
    return;
  }
  // Allocate and initialize slideshow struct
  slideshow = malloc(sizeof(slideshow_t));
  slideshow->i = -1;
  slideshow->num_images = 0;
  slideshow->transition_texture = NULL;
  slideshow->transition_alpha = 0.0f;
  slideshow->transition_change_rate = 255.0f / ((float) config.slideshow_transition_time / (float) POLLING_PERIOD);

  // Find background images from directory
  int num_images = scan_slideshow_directory(slideshow, config.slideshow_directory);
  
  // Handle errors
  if (num_images == 0) {
    output_log(LOGLEVEL_ERROR, 
               "Error: No images found in slideshow directory %s\n"
               "Changing background mode to color\n", 
               config.slideshow_directory);
    quit_slideshow();
    config.background_mode = MODE_COLOR;
    set_draw_color();
  } 
  else if (num_images == 1) {
    output_log(LOGLEVEL_ERROR, 
               "Error: Only one image found in slideshow directory %s\n"
               "Changing background mode to single image\n", 
               config.slideshow_directory);
    background_texture = load_texture(slideshow->images[0], NULL);
    quit_slideshow();
    config.background_mode = MODE_IMAGE;
  }

  // Generate array of random numbers for image order, load first image
  else {
    random_array(slideshow->order, slideshow->num_images);
    SDL_Surface *surface = load_next_slideshow_background(slideshow, false);
    background_texture = load_texture(NULL, surface);
    if (config.debug) {
      debug_slideshow(slideshow);
    }
  }
}

void init_screensaver()
{
  // Allocate memory for structure
  screensaver = malloc(sizeof(screensaver_t));
  
  // Convert intensity string to float
  char intensity[PERCENT_MAX_CHARS];
  int length = strlen(config.screensaver_intensity_str);
  if (length && 
  strchr(config.screensaver_intensity_str, (int) '%') == config.screensaver_intensity_str + length - 1) {
    strcpy(intensity, config.screensaver_intensity_str);
  }
  else {
    strcpy(intensity, DEFAULT_SCREENSAVER_INTENSITY);
  }
  intensity[length - 1] = '\0';
  float percent = atof(intensity);

  // Calculate alpha end value
  screensaver->alpha_end_value = 255.0f * percent / 100.0f;
  if (screensaver->alpha_end_value < 1.0f) {
    output_log(LOGLEVEL_ERROR, "Invalid screensaver intensity value, disabling feature\n");
    config.screensaver_enabled = false;
    free(screensaver);
    return;
  }
  else if (screensaver->alpha_end_value >= 255.0f) {
    screensaver->alpha_end_value = 255.0f;
  }
  screensaver->transition_change_rate = screensaver->alpha_end_value / ((float) SCREENSAVER_TRANSITION_TIME / (float) POLLING_PERIOD);
  
  // Render texture
  SDL_Surface *surface = NULL;
  surface = SDL_CreateRGBSurfaceWithFormat(0, 
              geo.screen_width, 
              geo.screen_height, 
              32,
              SDL_PIXELFORMAT_ARGB8888);
  Uint32 color = SDL_MapRGBA(surface->format, 0, 0, 0, 0xFF);
  SDL_FillRect(surface, NULL, color);
  screensaver->texture = load_texture(NULL, surface);
  screensaver->alpha = 0.0f;
  SDL_SetTextureAlphaMod(screensaver->texture, 0.0f);
}

// A function to resume th slideshow after a launched application returns
void resume_slideshow()
{
  ticks.slideshow_load = ticks.main;
}

// A function to advance X spaces in the entry linked list (left or right)
entry_t *advance_entries(entry_t *entry, int spaces, mode direction)
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

void render_scroll_indicators()
{
  // Calcuate the geometry
  scroll = malloc(sizeof(scroll_t));
  scroll->texture = NULL;
  int scroll_indicator_size = geo.screen_height / 10; // ~10% of screen height
  scroll->rect_right.w = scroll_indicator_size;
  scroll->rect_right.h = scroll_indicator_size;
  scroll->rect_left.w = scroll_indicator_size;
  scroll->rect_left.h = scroll_indicator_size;

  // Find scroll indicator file
  char *prefixes[2];
  char assets_exe_buffer[MAX_PATH_CHARS + 1];
  prefixes[0] = join_paths(assets_exe_buffer, 2, config.exe_path, PATH_ASSETS_EXE);
  #ifdef __unix__
  prefixes[1] = PATH_ASSETS_SYSTEM;
  #else
  prefixes[1] = PATH_ASSETS_RELATIVE;
  #endif
  char *scroll_indicator_path = find_file(FILENAME_SCROLL_INDICATOR, 2, prefixes);

  if (scroll_indicator_path == NULL) {
    output_log(LOGLEVEL_ERROR, 
               "Error: Could not find scroll indicator SVG, disabling feature\n");
    config.scroll_indicators = false;
  }
  else {
    output_log(LOGLEVEL_DEBUG, "Scroll indicator found: %s\n", 
               scroll_indicator_path);

    // Render the SVG
    scroll->texture = rasterize_svg(scroll_indicator_path,
                                    NULL,
                                    scroll_indicator_size,
                                    scroll_indicator_size);
    free(scroll_indicator_path);
    if (scroll->texture == NULL) {
      output_log(LOGLEVEL_ERROR, "Error: Could not render scroll indicator, disabling feature\n");
      config.scroll_indicators = false;
    }
    else {

      // Calculate screen position based on margin macro
      scroll->rect_right.y = (int) ((1.0F - SCROLL_INDICATOR_MARGIN)
                                    *(float) geo.screen_height) - scroll_indicator_size;
      scroll->rect_right.x = geo.screen_width - (geo.screen_height - scroll->rect_right.y);
      scroll->rect_left.y = scroll->rect_right.y;
      scroll->rect_left.x = geo.screen_width - (scroll->rect_right.x + scroll_indicator_size);

      // Set color
      SDL_SetTextureColorMod(scroll->texture,
                              config.scroll_indicator_color.r,
                              config.scroll_indicator_color.g,
                              config.scroll_indicator_color.b);
      SDL_SetTextureAlphaMod(scroll->texture,
                              config.scroll_indicator_color.a);
    }
  }
}

// A function to load a menu by name OR existing menu struct
int load_menu(const char *menu_name, menu_t *menu, bool set_back_menu, bool reset_position)
{
  int buttons;
  menu_t *previous_menu = current_menu;

  // Get the menu struct from menu name
  if (menu_name != NULL) {
    current_menu = get_menu(menu_name, config.first_menu);
  }
  else {
    current_menu = menu;
  }

  // Return error if the menu doesn't exist in the config file
  if (current_menu == NULL) {
    current_menu = previous_menu;
    if (menu_name != NULL) {
      output_log(LOGLEVEL_ERROR, 
                 "Error: Menu \"%s\" not found in config file\n", 
                 menu_name);
    }
    return 1;
  }

  output_log(LOGLEVEL_DEBUG, "Loading menu \"%s\"\n", current_menu->name);

  // Return error if the menu doesn't contain entires
  if (current_menu->num_entries == 0) {
    output_log(LOGLEVEL_ERROR, 
               "Error: No valid entries found for Menu \"%s\"", 
               current_menu->name);
    return 1;
  }

  // Render the menu if not already rendered
  if (current_menu->rendered == false) {
    render_buttons(current_menu);
  }

  // Set menu properties
  if (set_back_menu) {
    current_menu->back = previous_menu;
  }

  if (reset_position) {
    current_entry = current_menu->first_entry;
    current_menu->root_entry = current_entry;
    current_menu->highlight_position = 0;
    current_menu->page = 0;
  }
  else {
    current_entry = current_menu->last_selected_entry;
  }

  buttons = current_menu->num_entries - (current_menu->page)*config.max_buttons;
  if (buttons > config.max_buttons) {
    buttons = config.max_buttons;
  }
  
  // Recalculate the screen geometry
  calculate_geometry(current_menu->root_entry, buttons);
  //if (config.debug) {
  //  debug_button_positions(current_menu->root_entry, current_menu, &geo);
  //}
  highlight->rect.x = current_entry->icon_rect.x - config.highlight_hpadding;
  highlight->rect.y = current_entry->icon_rect.y - config.highlight_vpadding;
  
  // Output to screen
  state.screen_updates = true;
  return 0;
}

// A function to calculate the layout of the buttons
void calculate_geometry(entry_t *entry, int buttons)
{
  // Calculate proper spacing
  int button_height = config.icon_size + config.title_padding + geo.font_height;
  geo.x_margin = (geo.screen_width - config.icon_size*buttons -
                  buttons*config.icon_spacing + config.icon_spacing) / 2;
  geo.x_advance = config.icon_size + config.icon_spacing;
  geo.num_buttons = buttons;

  // Assign values to entries
  for (int i = 0; i < geo.num_buttons; i++) {
      entry->icon_rect.x = geo.x_margin + i*geo.x_advance;
      entry->icon_rect.y = geo.y_margin;
      entry->icon_rect.w = config.icon_size;
      entry->icon_rect.h = config.icon_size;
      entry->text_rect.x = entry->icon_rect.x +
                          (entry->icon_rect.w - entry->text_rect.w) / 2;
      entry->text_rect.y = entry->icon_rect.y +
                          config.icon_size + entry->title_offset + config.title_padding;
      entry = entry->next;
  }
}

// A function to render all buttons (icon and text) for a menu
void render_buttons(menu_t *menu)
{
  entry_t *entry;
  for (entry = menu->first_entry; entry != NULL; entry = entry->next) {
    entry->icon = load_texture(entry->icon_path, NULL);
    entry->title_texture = render_text(entry, &geo);
  }
  menu->rendered = true;
}

// A function to output all visible buttons to the renderer
void draw_buttons(entry_t *entry)
{
  for (int i = 0; i < geo.num_buttons; i++) {
    SDL_RenderCopy(renderer,entry->icon,NULL,&entry->icon_rect);
    SDL_RenderCopy(renderer,entry->title_texture,NULL,&entry->text_rect);
    entry = entry-> next;
  }
}

// A function to move the selection left when clicked by user
void move_left()
{
  // If we are not in leftmost position, move highlight left
  if (current_menu->highlight_position > 0) {
    highlight->rect.x -= geo.x_advance;
    current_menu->highlight_position--;
    current_entry = current_entry->previous;
    state.screen_updates = true;
  }

  // If we are in leftmost position, but there is a previous page, load the previous page
  else if (current_menu->highlight_position == 0 && current_menu->page > 0) {
    int buttons = config.max_buttons;
    current_entry = current_entry->previous;
    current_menu->root_entry = advance_entries(current_menu->root_entry,buttons,DIRECTION_LEFT);
    calculate_geometry(current_menu->root_entry, buttons);
    highlight->rect.x = current_entry->icon_rect.x
                        - config.highlight_hpadding;
    current_menu->page--;
    current_menu->highlight_position = buttons - 1;
    //if (config.debug) {
    //  debug_button_positions(current_menu->root_entry, current_menu, &geo);
    //}
    state.screen_updates = true;
  }
}

// A function to move the selection right when clicked by the user
void move_right()
{
  // If we are not in the rightmost position, move highlight right
  if (current_menu->highlight_position < (geo.num_buttons - 1)) {
    highlight->rect.x += geo.x_advance;
    current_menu->highlight_position++;
    current_entry = current_entry->next;
    state.screen_updates = true;
  }

  // If we are in the rightmost postion, but there are more entries in the menu, load next page
  else if (current_menu->highlight_position + current_menu->page*config.max_buttons <
  (current_menu->num_entries - 1)) {
    int buttons = current_menu->num_entries
                  - (current_menu->page + 1)*config.max_buttons;
    if (buttons > config.max_buttons) {
      buttons = config.max_buttons;
    }
    current_entry = current_entry->next;
    current_menu->root_entry = current_entry;
    calculate_geometry(current_menu->root_entry, buttons);
    highlight->rect.x = current_entry->icon_rect.x - config.highlight_hpadding;
    current_menu->page++;
    current_menu->highlight_position = 0;
    //if (config.debug) {
    //  debug_button_positions(current_menu->root_entry, current_menu, &geo);
    //}
    state.screen_updates = true;
  }
}

// A function to load a submenu
void load_submenu(char *submenu)
{
  current_menu->last_selected_entry = current_entry;
  load_menu(submenu, NULL, true, true);
}

// A function to load the previous menu
void load_back_menu(menu_t *menu)
{
  load_menu(NULL, menu->back, false, config.reset_on_back);
}

// A function to update the screen with all visible textures
void draw_screen()
{
  // Draw background
  if (config.background_mode == MODE_COLOR) {
    SDL_RenderClear(renderer);
  }
  else {
    SDL_RenderCopy(renderer, background_texture, NULL, NULL);
  }

  if (config.background_mode == MODE_SLIDESHOW && state.slideshow_transition) {
    SDL_RenderCopy(renderer, slideshow->transition_texture, NULL, NULL);
  }

  // Draw scroll indicators
  if (config.scroll_indicators &&
  (current_menu->page*config.max_buttons + geo.num_buttons) <= (current_menu->num_entries - 1)) {
    SDL_RenderCopy(renderer,scroll->texture,NULL,&scroll->rect_right);
  }
  if (config.scroll_indicators && current_menu->page > 0) {
    SDL_RenderCopyEx(renderer,scroll->texture,NULL,&scroll->rect_left,0,NULL,SDL_FLIP_HORIZONTAL);
  }

  // Draw highlight
  SDL_RenderCopy(renderer,
                 highlight->texture,
                 NULL,
                 &highlight->rect);

  // Draw buttons
  draw_buttons(current_menu->root_entry);

  if (state.screensaver_active) {
    SDL_RenderCopy(renderer, screensaver->texture, NULL, NULL);
  }

  // Output to screen
  SDL_RenderPresent(renderer);
  state.screen_updates = false;
}

// A function to execute the user's command
void execute_command(const char *command)
{
  if (strlen(command) == 0) {
    return;
  }

  // Copy command into separate buffer
  char *cmd;
  copy_string(&cmd, command);

  // Parse .desktop file (Linux only)
  #ifdef __unix__
    char *desktop_exec = NULL;
    int desktop = parse_desktop_file(cmd, &desktop_exec);
    if (desktop == DESKTOP_SUCCESS) {
      free(cmd);
      cmd = desktop_exec;
    }
    else if (desktop == DESKTOP_ERROR) {
      free(cmd);
      return;
    }
  #endif

  // Parse special commands
  if (cmd[0] == ':') {
    char *delimiter = " ";
    char *special_command = strtok(cmd,delimiter);
    if (!strcmp(special_command, SCMD_SUBMENU)) {
      char *submenu = strtok(NULL,delimiter);
      load_submenu(submenu);
    }
    else if (!strcmp(special_command, SCMD_LEFT)) {
      move_left();
    }
    else if (!strcmp(special_command, SCMD_RIGHT)) {
      move_right();
    }
    else if (!strcmp(special_command, SCMD_SELECT)) {
      execute_command(current_entry->cmd);
    }
    else if (!strcmp(special_command, SCMD_HOME)) {
      load_menu(NULL, default_menu, false, true);
    }
    else if (!strcmp(special_command, SCMD_BACK)) {
      load_back_menu(current_menu);
    }
    else if (!strcmp(special_command, SCMD_QUIT)) {
      quit(0);
    }
    else if (!strcmp(special_command, SCMD_SHUTDOWN)) {
      system(CMD_SHUTDOWN);
    }
    else if (!strcmp(special_command, SCMD_RESTART)) {
      system(CMD_RESTART);
    }
    else if (!strcmp(special_command, SCMD_SLEEP)) {
      system(CMD_SLEEP);
    }
  }

  // Launch external application
  else {

    // Perform prelaunch behavior from OnLaunch setting
    if (config.on_launch == MODE_ON_LAUNCH_HIDE) {
      SDL_HideWindow(window);
    }
    else if (config.on_launch == MODE_ON_LAUNCH_BLANK) {
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
      SDL_RenderClear(renderer);
      SDL_RenderPresent(renderer);
    }

    // Launch application
    launch_application(cmd);

    // Rebaseline the timing after the program is done
    ticks.main = SDL_GetTicks();
    ticks.last_input = ticks.main;

    if (config.background_mode == MODE_SLIDESHOW) {
      resume_slideshow();
    }

    if (config.on_launch == MODE_ON_LAUNCH_HIDE) {
      SDL_ShowWindow(window);
    }
  }
  free(cmd);
}

// A function to connect to a gamepad
void connect_gamepad(int device_index)
{
  gamepad = SDL_GameControllerOpen(device_index);
  if (gamepad == NULL) {
    output_log(LOGLEVEL_ERROR, 
               "Error: Could not open gamepad at device index %i\n", 
               config.gamepad_device);
    return;
  }
  if (config.debug) {
    char *mapping = SDL_GameControllerMapping(gamepad);
    output_log(LOGLEVEL_DEBUG, 
               "Gamepad Mapping:\n%s\n", 
               mapping);
    SDL_free(mapping);
  }
}

// A function to poll the connected gamepad for commands
void poll_gamepad()
{
  int value_multiplier; // Handles positive or negative axis
  for (gamepad_control_t *i = config.gamepad_controls; i != NULL; i = i->next) {
    
    // Check if axis value exceeds dead zone
    if (i->type == TYPE_AXIS_POS || i->type == TYPE_AXIS_NEG) {
      if (i->type == TYPE_AXIS_POS) {
        value_multiplier = 1;
      }
      else if (i->type == TYPE_AXIS_NEG) {
        value_multiplier = -1;
      }
      if (value_multiplier*SDL_GameControllerGetAxis(gamepad, i->index) > GAMEPAD_DEADZONE) {
        i->repeat++;
      }
      else {
        i->repeat = 0;
      }
    }

    // Check buttons
    else if (i->type == TYPE_BUTTON) {
      if (SDL_GameControllerGetButton(gamepad, i->index)) {
        i->repeat++;
      }
      else {
        i->repeat = 0;
      }
    }

    // Execute command if first press or valid repeat
    if (i->repeat == 1) {
      output_log(LOGLEVEL_DEBUG, "Gamepad %s detected\n", i->label);
      ticks.last_input = ticks.main;
      execute_command(i->cmd);
    }
    else if (i->repeat == delay_period) {
      ticks.last_input = ticks.main;
      execute_command(i->cmd);
      i->repeat -= repeat_period;
    }
  }
}

// A function to update the slideshow
void update_slideshow()
{
  // If image duration time has elapsed, load the next image and start the transition
  if (!state.slideshow_transition && (ticks.main - ticks.slideshow_load > config.slideshow_image_duration) &&
  !state.slideshow_paused) {
    
    // Render the new background image in a separate thread so we don't block the main thread
    if (!state.slideshow_background_rendering && !state.slideshow_background_ready) {
      slideshow_thread = SDL_CreateThread(load_next_slideshow_background_async, "Slideshow Thread", (void*) slideshow);
      state.slideshow_background_rendering = true;
    }

    // Convert background to texture after the rendering thread has completed
    else if (state.slideshow_background_ready) {
      SDL_WaitThread(slideshow_thread, NULL);
      slideshow_thread = NULL;
      if (config.slideshow_transition_time > 0) {
        slideshow->transition_texture = load_texture(NULL, slideshow->transition_surface);
        SDL_SetTextureAlphaMod(slideshow->transition_texture, 0);
        state.slideshow_transition = true;
      }
      else {
        SDL_DestroyTexture(background_texture);
        background_texture = load_texture(NULL, slideshow->transition_surface);
        ticks.slideshow_load = ticks.main;
      }
    slideshow->transition_surface = NULL;
    state.slideshow_background_ready = false;
    state.screen_updates = true;  
    }
  }
  else if (state.slideshow_transition) {
    
    // Increase the transparency
    slideshow->transition_alpha += slideshow->transition_change_rate;
    
    // If transition is done, destroy old background and replace it with the new one
    if (slideshow->transition_alpha >= 255.0f) {
      SDL_SetTextureAlphaMod(slideshow->transition_texture, 0xFF);
      slideshow->transition_alpha = 0.0f;
      SDL_DestroyTexture(background_texture);
      background_texture = slideshow->transition_texture;
      slideshow->transition_texture = NULL;
      state.slideshow_transition = false;
      ticks.slideshow_load = ticks.main;
    }
    else {
      SDL_SetTextureAlphaMod(slideshow->transition_texture, (Uint8) slideshow->transition_alpha);
    }
  state.screen_updates = true;
  }
}

// A function to update the screensaver
void update_screensaver()
{
  // Activate the screensaver if the launcher has been idle for the required time
  if (!state.screensaver_active && ticks.main - ticks.last_input > config.screensaver_idle_time) {
    state.screensaver_active = true;
    state.screensaver_transition = true;
    if (config.background_mode == MODE_SLIDESHOW && config.screensaver_pause_slideshow) {
      state.slideshow_paused = true;
    }
  }
  else {

    // Transition the screen to dark
    if (state.screensaver_transition) {
      screensaver->alpha += screensaver->transition_change_rate;
      if (screensaver->alpha >= screensaver->alpha_end_value) {
        SDL_SetTextureAlphaMod(screensaver->texture, (Uint8) screensaver->alpha_end_value);
        state.screensaver_transition = false;
      }
      else {
        SDL_SetTextureAlphaMod(screensaver->texture, (Uint8) screensaver->alpha);
      }
    state.screen_updates = true;
    }

    // User has pressed input, deactivate the screensaver
    if (state.screensaver_active && ticks.last_input == ticks.main) {
      SDL_SetTextureAlphaMod(screensaver->texture, 0);
      screensaver->alpha = 0.0f;
      state.screensaver_active = false;
      state.screensaver_transition = false;
      if (config.background_mode == MODE_SLIDESHOW) {
        state.slideshow_paused = false;
        
        // Reset the slideshow time so we don't have a transition immediately 
        // after coming out of screensaver mode
        ticks.slideshow_load = ticks.main;
      }
      state.screen_updates = true;
    }
  }
}

void quit(int status)
{
  output_log(LOGLEVEL_DEBUG, "Quitting program\n");
  cleanup();
  exit(status);
}


int main(int argc, char *argv[]) 
{
  SDL_Event event;
  int error;
  char *config_file_path = NULL;
  config.exe_path = SDL_GetBasePath();

  error = handle_arguments(argc, argv, &config_file_path);
  if (error == NO_ERROR_QUIT) {
    quit(0);
  }
  else if (error == ERROR_QUIT) {
    quit(1);
  }
  output_log(LOGLEVEL_DEBUG, "Config file found: %s\n", config_file_path);

  // Parse config file for settings and menu entries
  error = ini_parse(config_file_path, config_handler, &config);
  if (error < 0) {
    output_log(LOGLEVEL_FATAL, "Fatal Error: Config file %s not found\n", config_file_path);
    quit(1);
  }
  free(config_file_path);

  // Initialize libraries
  if (init_sdl() || init_ttf() || init_svg()) {
    quit(1);
  }

  ticks.main = SDL_GetTicks();
  ticks.last_input = ticks.main;

  // Check settings against requirements
  validate_settings(&geo);

  // Load gamepad overrides
  if (config.gamepad_enabled) {
    if (config.gamepad_mappings_file != NULL) {
      error = SDL_GameControllerAddMappingsFromFile(config.gamepad_mappings_file);
      if (error) {
        output_log(LOGLEVEL_ERROR, 
                   "Error: Could not load gamepad mappings from %s\n", 
                   config.gamepad_mappings_file);
      }
    }
  }

  // Render background image
  if (config.background_mode == MODE_IMAGE) {
    background_texture = load_texture(config.background_image,NULL);

    // Switch to color mode if loading background image failed
    if (background_texture == NULL) {
      config.background_mode = MODE_COLOR;
      output_log(LOGLEVEL_ERROR, "Error: Couldn't load background image, defaulting to color background\n");
      set_draw_color();
    }
  }

  // Initialize slideshow
  else if (config.background_mode == MODE_SLIDESHOW) {
    init_slideshow();
  }

  // Initialize screensaver
  if (config.screensaver_enabled) {
    init_screensaver();
  }
  
  // Render highlight
  int button_height = config.icon_size + config.title_padding + geo.font_height;
  highlight = malloc(sizeof(highlight_t));
  highlight->rect.w = config.icon_size + 2*config.highlight_hpadding;
  highlight->rect.h = button_height + 2*config.highlight_vpadding;
  highlight->texture = render_highlight(highlight->rect.w,
                                        highlight->rect.h,
                                        config.highlight_rx);

  
  // Render scroll indicators
  if (config.scroll_indicators) {
    render_scroll_indicators();
  }
  
  // Debug info
  if (config.debug) {
    debug_video(renderer, &display_mode);
    debug_settings();
    debug_hotkeys(hotkeys);  
    debug_menu_entries(config.first_menu, config.num_menus);
  }

  // Load the default menu and display it
  if (config.default_menu == NULL) {
    output_log(LOGLEVEL_FATAL, "Fatal Error: No default menu defined in config file\n");
    quit(1);
  }

  default_menu = get_menu(config.default_menu, config.first_menu);
  if (default_menu == NULL) {
    output_log(LOGLEVEL_FATAL, 
               "Fatal Error: Default Menu \"%s\" not found in config file\n", 
               config.default_menu);
    quit(1);

  }
  error = load_menu(NULL, default_menu, false, true);
  if (error) {
    quit(1);
  }
  
  // Draw initial screen
  draw_screen();
   
  // Main program loop
  output_log(LOGLEVEL_DEBUG, "Begin program loop\n");
  while (1) {
    ticks.main = SDL_GetTicks();
    while (SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_QUIT:
          quit(0);
          break;

        case SDL_KEYDOWN:
          ticks.last_input = ticks.main;
          handle_keypress(&event.key.keysym);         
          break;

        case SDL_CONTROLLERDEVICEADDED:
          output_log(LOGLEVEL_DEBUG, "Gamepad connected with device index %i\n", event.cdevice.which);
          if (event.cdevice.which == config.gamepad_device) {
            connect_gamepad(event.cdevice.which);
          }
          break;

        case SDL_CONTROLLERDEVICEREMOVED:
          output_log(LOGLEVEL_DEBUG, "Gamepad removed with device index %i\n", event.cdevice.which);
          if (event.cdevice.which == config.gamepad_device) {
            SDL_GameControllerClose(gamepad);
            gamepad = NULL;
          }

        case SDL_WINDOWEVENT:
          if (event.window.event == SDL_WINDOWEVENT_SHOWN || 
          event.window.event == SDL_WINDOWEVENT_EXPOSED || 
          event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
            if (config.on_launch == MODE_ON_LAUNCH_BLANK) {
              set_draw_color();
              state.screen_updates = true;
            }
            else {
              state.screen_updates = true;
            }
          }
          else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
            output_log(LOGLEVEL_DEBUG, "Lost keyboard focus\n");
          }
          else if (event.window.event == SDL_WINDOWEVENT_LEAVE) {
            output_log(LOGLEVEL_DEBUG, "Lost mouse focus\n");
          }
      }
    }

    // Post-loop updates
    if (gamepad != NULL) {
      poll_gamepad();
    }
    if (config.background_mode == MODE_SLIDESHOW) {
      update_slideshow();
    }
    if (config.screensaver_enabled) {
      update_screensaver();
    }
    
    //output_log(LOGLEVEL_DEBUG, "Loop time: %i ms\n", SDL_GetTicks() - ticks.main);
    
    if (state.screen_updates) {
      draw_screen();
    }
    SDL_Delay(POLLING_PERIOD);
  }
  quit(0);
}