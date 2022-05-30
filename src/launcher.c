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
#include "clock.h"
#include "platform/platform.h"

// Initialize default settings
Config config = {
    .default_menu                     = NULL,
    .background_image                 = NULL,
    .slideshow_directory              = NULL,
    .title_font_path                  = NULL,
    .title_font_size                  = DEFAULT_FONT_SIZE,
    .title_font_color.r               = DEFAULT_TITLE_FONT_COLOR_R,
    .title_font_color.g               = DEFAULT_TITLE_FONT_COLOR_G,
    .title_font_color.b               = DEFAULT_TITLE_FONT_COLOR_B,
    .title_font_color.a               = DEFAULT_TITLE_FONT_COLOR_A,
    .title_shadows                    = DEFAULT_TITLE_SHADOWS,
    .title_shadow_color.r             = DEFAULT_TITLE_SHADOW_COLOR_R,
    .title_shadow_color.g             = DEFAULT_TITLE_SHADOW_COLOR_G,
    .title_shadow_color.b             = DEFAULT_TITLE_SHADOW_COLOR_B,
    .title_shadow_color.a             = DEFAULT_TITLE_SHADOW_COLOR_A,
    .background_mode                  = MODE_COLOR,
    .background_color.r               = DEFAULT_BACKGROUND_COLOR_R,
    .background_color.g               = DEFAULT_BACKGROUND_COLOR_G,
    .background_color.b               = DEFAULT_BACKGROUND_COLOR_B,
    .background_color.a               = 0xFF,
    .background_overlay               = DEFAULT_BACKGROUND_OVERLAY,
    .background_overlay_color.r       = DEFAULT_BACKGROUND_OVERLAY_COLOR_R,
    .background_overlay_color.g       = DEFAULT_BACKGROUND_OVERLAY_COLOR_G,
    .background_overlay_color.b       = DEFAULT_BACKGROUND_OVERLAY_COLOR_B,
    .background_overlay_color.a       = DEFAULT_BACKGROUND_OVERLAY_COLOR_A,
    .background_overlay_opacity[0]    = '\0',
    .icon_size                        = DEFAULT_ICON_SIZE,
    .highlight_fill_color.r           = DEFAULT_HIGHLIGHT_FILL_COLOR_R,
    .highlight_fill_color.g           = DEFAULT_HIGHLIGHT_FILL_COLOR_G,
    .highlight_fill_color.b           = DEFAULT_HIGHLIGHT_FILL_COLOR_B,
    .highlight_fill_color.a           = DEFAULT_HIGHLIGHT_FILL_COLOR_A,
    .highlight_outline_color.r        = DEFAULT_HIGHLIGHT_OUTLINE_COLOR_R,
    .highlight_outline_color.g        = DEFAULT_HIGHLIGHT_OUTLINE_COLOR_G,
    .highlight_outline_color.b        = DEFAULT_HIGHLIGHT_OUTLINE_COLOR_B,
    .highlight_outline_color.a        = DEFAULT_HIGHLIGHT_OUTLINE_COLOR_A,
    .highlight_outline_size           = DEFAULT_HIGHLIGHT_OUTLINE_SIZE,
    .highlight_rx                     = DEFAULT_HIGHLIGHT_CORNER_RADIUS,
    .title_padding                    = -1,
    .max_buttons                      = DEFAULT_MAX_BUTTONS,
    .icon_spacing                     = -1,
    .highlight_vpadding               = -1,
    .highlight_hpadding               = -1,
    .title_opacity[0]                 = '\0',
    .highlight_fill_opacity[0]        = '\0',
    .highlight_outline_opacity[0]     = '\0',
    .button_centerline[0]             = '\0',
    .icon_spacing_str[0]              = '\0',
    .scroll_indicators                = DEFAULT_SCROLL_INDICATORS,
    .scroll_indicator_fill_color.r    = DEFAULT_SCROLL_INDICATOR_FILL_COLOR_R,
    .scroll_indicator_fill_color.g    = DEFAULT_SCROLL_INDICATOR_FILL_COLOR_G,
    .scroll_indicator_fill_color.b    = DEFAULT_SCROLL_INDICATOR_FILL_COLOR_B,
    .scroll_indicator_fill_color.a    = DEFAULT_SCROLL_INDICATOR_FILL_COLOR_A,
    .scroll_indicator_outline_size    = DEFAULT_SCROLL_INDICATOR_OUTLINE_SIZE,
    .scroll_indicator_outline_color.r = DEFAULT_SCROLL_INDICATOR_OUTLINE_COLOR_R,
    .scroll_indicator_outline_color.g = DEFAULT_SCROLL_INDICATOR_OUTLINE_COLOR_G,
    .scroll_indicator_outline_color.b = DEFAULT_SCROLL_INDICATOR_OUTLINE_COLOR_B,
    .scroll_indicator_outline_color.a = DEFAULT_SCROLL_INDICATOR_OUTLINE_COLOR_A,
    .scroll_indicator_opacity[0]      = '\0',
    .title_oversize_mode              = MODE_TRUNCATE,
    .reset_on_back                    = DEFAULT_RESET_ON_BACK,
    .mouse_select                     = DEFAULT_MOUSE_SELECT,
    .startup_cmd                      = NULL,
    .quit_cmd                         = NULL,
    .screensaver_enabled              = false,
    .screensaver_idle_time            = DEFAULT_SCREENSAVER_IDLE_TIME*1000,
    .screensaver_intensity_str[0]     = '\0',
    .screensaver_pause_slideshow      = DEFAULT_SCREENSAVER_PAUSE_SLIDESHOW,
    .gamepad_enabled                  = DEFAULT_GAMEPAD_ENABLED,
    .gamepad_device                   = DEFAULT_GAMEPAD_DEVICE,
    .gamepad_mappings_file            = NULL,
    .on_launch                        = MODE_HIDE,
    .debug                            = false,
    .exe_path                         = NULL,
    .first_menu                       = NULL,
    .num_menus                        = 0,
    .clock_enabled                    = DEFAULT_CLOCK_ENABLED,
    .clock_show_date                  = DEFAULT_CLOCK_SHOW_DATE,
    .clock_alignment                  = DEFAULT_CLOCK_ALIGNMENT,
    .clock_font_path                  = NULL,
    .clock_margin_str[0]              = '\0',
    .clock_margin                     = -1,
    .clock_font_color.r               = DEFAULT_CLOCK_FONT_COLOR_R,
    .clock_font_color.g               = DEFAULT_CLOCK_FONT_COLOR_G,
    .clock_font_color.b               = DEFAULT_CLOCK_FONT_COLOR_B,
    .clock_font_color.a               = DEFAULT_CLOCK_FONT_COLOR_A,
    .clock_shadows                    = DEFAULT_CLOCK_SHADOWS,
    .clock_shadow_color.r             = DEFAULT_CLOCK_SHADOW_COLOR_R,
    .clock_shadow_color.g             = DEFAULT_CLOCK_SHADOW_COLOR_G,
    .clock_shadow_color.b             = DEFAULT_CLOCK_SHADOW_COLOR_B,
    .clock_shadow_color.a             = DEFAULT_CLOCK_SHADOW_COLOR_A,
    .clock_opacity[0]                 = '\0',
    .clock_font_size                  = DEFAULT_CLOCK_FONT_SIZE,
    .clock_time_format                = DEFAULT_CLOCK_TIME_FORMAT,
    .clock_date_format                = DEFAULT_CLOCK_DATE_FORMAT,
    .clock_include_weekday            = DEFAULT_CLOCK_INCLUDE_WEEKDAY,
    .slideshow_image_duration         = DEFAULT_SLIDESHOW_IMAGE_DURATION,
    .slideshow_transition_time        = DEFAULT_SLIDESHOW_TRANSITION_TIME
};

// Initialize default states
State state = {
    .slideshow_transition             = false,
    .slideshow_background_ready       = false,
    .slideshow_paused                 = false,
    .screensaver_active               = false,
    .screensaver_transition           = false,
    .clock_rendering                  = false,
    .clock_ready                      = false
};

// Global variables
SDL_Window *window                    = NULL;
SDL_Renderer *renderer                = NULL;
SDL_Texture *background_texture       = NULL;
SDL_Texture *background_overlay       = NULL;
Menu *default_menu                    = NULL;
Menu *current_menu                    = NULL;
Entry *current_entry                  = NULL;
Highlight *highlight                  = NULL;
Scroll *scroll                        = NULL;
Slideshow *slideshow                  = NULL;
Screensaver *screensaver              = NULL;
FILE *log_file                        = NULL;
SDL_GameController *gamepad           = NULL;
GamepadControl *gamepad_controls      = NULL;
Hotkey *hotkeys                       = NULL;
Clock *clk                            = NULL;
TTF_Font *clock_font                  = NULL;
SDL_Thread *Slideshowhread            = NULL;
SDL_Thread *clock_thread              = NULL;
SDL_Event event;
SDL_SysWMinfo wm_info;
SDL_DisplayMode display_mode;
TextInfo title_info;
Ticks ticks;
Geometry geo;
int refresh_period;
int delay_period;
int repeat_period; 


// A function to initialize SDL
static void init_sdl()
{    
    // Set flags, hints
    int sdl_flags = SDL_INIT_VIDEO;
#ifdef __unix__
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
#endif
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"1");
    if (config.gamepad_enabled) {
        sdl_flags |= SDL_INIT_GAMECONTROLLER;
    }

    // Initialize SDL
    if (SDL_Init(sdl_flags) < 0)
    {
        output_log(LOGLEVEL_FATAL, 
            "Fatal Error: Could not initialize SDL\n%s\n", 
            SDL_GetError()
        );
    }
    int ret = SDL_GetDesktopDisplayMode(0, &display_mode);
    geo.screen_width = display_mode.w;
    geo.screen_height = display_mode.h;
    refresh_period = 1000 / display_mode.refresh_rate;
    if (config.gamepad_enabled) {
        delay_period = GAMEPAD_REPEAT_DELAY / refresh_period;
        repeat_period = GAMEPAD_REPEAT_INTERVAL / refresh_period; 
    }
    geo.screen_margin = (int) (SCREEN_MARGIN * (float) geo.screen_height);
}

// A function to create the window and renderer
static void create_window()
{
    window = SDL_CreateWindow(PROJECT_NAME, 
                 SDL_WINDOWPOS_UNDEFINED,
                 SDL_WINDOWPOS_UNDEFINED,
                 0,
                 0,
                 SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_BORDERLESS
             );
    if (window == NULL) {
        output_log(LOGLEVEL_FATAL, 
            "Fatal Error: Could not create SDL Window\n%s\n", 
            SDL_GetError()
        );
    }
    SDL_ShowCursor(SDL_DISABLE);

    // Create HW accelerated renderer, get screen resolution for geometry calculations
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    if (renderer == NULL) {
        output_log(LOGLEVEL_FATAL, 
            "Fatal Error: Could not initialize renderer\n%s\n", 
            SDL_GetError()
        );
    }

    // Set background color
    set_draw_color();

#ifdef _WIN32
    SDL_VERSION(&wm_info.version);
    SDL_GetWindowWMInfo(window, &wm_info);
#endif
}

// A function to initialize the SDL_image library
static void init_sdl_image()
{
    int img_flags = IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_WEBP;
    // Initialize SDL_image
    if (!(IMG_Init(img_flags) & img_flags)) {
        output_log(LOGLEVEL_FATAL, 
            "Fatal Error: Could not initialize SDL_image\n%s\n", 
            IMG_GetError()
        );
    }
}

// A function to set the color of the renderer
void set_draw_color()
{
    if (config.background_mode == MODE_COLOR) {
        SDL_SetRenderDrawColor(renderer,
            config.background_color.r,
            config.background_color.g,
            config.background_color.b,
            config.background_color.a
        );
    }
    else {
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    }
}

// A function to initialize SDL's TTF subsystem
static void init_sdl_ttf()
{
    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        output_log(LOGLEVEL_FATAL, 
            "Fatal Error: Could not initialize SDL_ttf\n%s\n", 
            TTF_GetError()
        );
     }
    
    title_info = (TextInfo) { 
        .font_size = config.title_font_size,
        .shadow = config.title_shadows,
        .font_path = &config.title_font_path,
        .max_width = config.icon_size,
        .oversize_mode = config.title_oversize_mode,
        .color = &config.title_font_color
    };
    if (config.title_shadows) {
        title_info.shadow_color = &config.title_shadow_color;
        calculate_shadow_alpha(title_info);
    }
    else {
        title_info.shadow_color = NULL;
    }

    int error = load_font(&title_info, FILENAME_DEFAULT_FONT);
    if (error) {
        output_log(LOGLEVEL_FATAL, "Fatal Error: Could not load title font\n");
    }

    TTF_SizeUTF8(title_info.font, "TEST STRING", NULL, &geo.font_height);
}

// A function to close subsystems and free memory before quitting
static void cleanup()
{
    // Wait until all threads have completed
    SDL_WaitThread(Slideshowhread, NULL);
    SDL_WaitThread(clock_thread, NULL);
    
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
    if (config.background_mode == MODE_SLIDESHOW) {
        quit_slideshow();
    }

    // Close log file if open
    if (log_file != NULL) {
        fclose(log_file);
    }

    // Free dynamically allocated memory
    free(config.default_menu);
    free(config.background_image);
    free(config.title_font_path);
    free(config.exe_path);
    free(config.slideshow_directory);
    free(config.clock_font_path);
    free(config.gamepad_mappings_file);
    free(config.startup_cmd);
    free(highlight);
    free(scroll);
    free(screensaver);
    free(clk);

    // Free menu and entry linked lists
    Entry *entry = NULL;
    Entry *tmp_entry = NULL;
    Menu *menu = config.first_menu;
    Menu *tmp_menu = NULL;
    for (int i = 0; i < config.num_menus; i++) {
        free(menu->name);
        entry = menu->first_entry;
        for(int j = 0; j < menu->num_entries; j++) {
            free(entry->title);
            free(entry->icon_path);
            free(entry->icon_selected_path);
            free(entry->cmd);
            tmp_entry = entry;
            entry = entry->next;
            free(tmp_entry);
        }
        tmp_menu = menu;
        menu = menu->next;
        free(tmp_menu);
    }

    // Free hotkey linked list
    Hotkey *tmp_hotkey = NULL;
    for(Hotkey *i = hotkeys; i != NULL; i = i->next) {
        free(tmp_hotkey);
        free(i->cmd);
        tmp_hotkey = i;
    }
    free(tmp_hotkey);

    // Free gamepad control linked list
    GamepadControl *tmp_gamepad = NULL;
    for (GamepadControl *i = gamepad_controls; i != NULL; i = i->next) {
        free(tmp_gamepad);
        free(i->cmd);
        tmp_gamepad = i;
    }
    free(tmp_gamepad);
}

// A function to handle key presses from keyboard
static void handle_keypress(SDL_Keysym *key)
{
    if (config.debug) {
        output_log(LOGLEVEL_DEBUG, 
            "Key %s (%X) detected\n", 
            SDL_GetKeyName(key->sym),
            key->sym
        );
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
            current_entry->cmd
        );
        
        execute_command(current_entry->cmd);
    }
    else if (key->sym == SDLK_BACKSPACE) {
        load_back_menu(current_menu);
    }

    //Check hotkeys
    else {
        for (Hotkey *i = hotkeys; i != NULL; i = i->next) {
            if (key->sym == i->keycode) {
                execute_command(i->cmd);
                break;
            }
        }
    }
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
static void init_slideshow()
{
    if (!directory_exists(config.slideshow_directory)) {
        output_log(LOGLEVEL_ERROR, 
            "Error: Slideshow directory %s does not exist\n"
            "Switching to color background mode\n",
            config.slideshow_directory
        );
        config.background_mode = MODE_COLOR;
        set_draw_color();
        return;
    }
    // Allocate and initialize slideshow struct
    slideshow = malloc(sizeof(Slideshow));
    slideshow->i = -1;
    slideshow->num_images = 0;
    slideshow->transition_texture = NULL;
    slideshow->transition_alpha = 0.0f;
    slideshow->transition_change_rate = 255.0f / ((float) config.slideshow_transition_time / (float) refresh_period);

    // Find background images from directory
    int num_images = scan_slideshow_directory(slideshow, config.slideshow_directory);
    
    // Handle errors
    if (num_images == 0) {
        output_log(LOGLEVEL_ERROR, 
            "Error: No images found in slideshow directory %s\n"
            "Changing background mode to color\n", 
            config.slideshow_directory
        );
        config.background_mode = MODE_COLOR;
        quit_slideshow();
    } 
    else if (num_images == 1) {
        output_log(LOGLEVEL_ERROR, 
            "Error: Only one image found in slideshow directory %s\n"
            "Changing background mode to single image\n", 
            config.slideshow_directory
        );
        free(config.background_image);
        copy_string_alloc(&config.background_image, slideshow->images[0]);
        config.background_mode = MODE_IMAGE;
        quit_slideshow();
    }

    // Generate array of random numbers for image order, load first image
    else {
        random_array(slideshow->order, slideshow->num_images);
        if (config.debug) {
            debug_slideshow(slideshow);
        }
    }
}

// A function to initialize the screensaver feature
static void init_screensaver()
{
    // Allocate memory for structure
    screensaver = malloc(sizeof(Screensaver));
    
    // Convert intensity string to float
    char intensity[PERCENT_MAX_CHARS];
    if (config.screensaver_intensity_str[0] != '\0') {
        copy_string(intensity, config.screensaver_intensity_str, sizeof(intensity));
    }
    else {
        copy_string(intensity, DEFAULT_SCREENSAVER_INTENSITY, sizeof(intensity));
    }
    int length = strlen(intensity);
    intensity[length - 1] = '\0';
    float percent = atof(intensity);

    // Calculate alpha end value
    screensaver->alpha_end_value = 255.0f * percent / 100.0f;
    if (screensaver->alpha_end_value < 1.0f) {
        output_log(LOGLEVEL_ERROR, "Invalid screensaver intensity value, disabling feature\n");
        config.screensaver_enabled = false;
        free(screensaver);
        screensaver = NULL;
        return;
    }
    else if (screensaver->alpha_end_value >= 255.0f) {
        screensaver->alpha_end_value = 255.0f;
    }
    screensaver->transition_change_rate = screensaver->alpha_end_value / ((float) SCREENSAVER_TRANSITION_TIME / (float) refresh_period);
    
    // Render texture
    SDL_Surface *surface = NULL;
    surface = SDL_CreateRGBSurfaceWithFormat(0, 
                  geo.screen_width, 
                  geo.screen_height, 
                  32,
                  SDL_PIXELFORMAT_ARGB8888
              );
    Uint32 color = SDL_MapRGBA(surface->format, 0, 0, 0, 0xFF);
    SDL_FillRect(surface, NULL, color);
    screensaver->texture = load_texture(surface);
    screensaver->alpha = 0.0f;
    SDL_SetTextureAlphaMod(screensaver->texture, 0.0f);
}

// A function to resume the slideshow after a launched application returns
static void resume_slideshow()
{
    ticks.slideshow_load = ticks.main;
}

// A function to load a menu
static int load_menu(Menu *menu, bool set_back_menu, bool reset_position)
{
    if (menu == NULL) {
        return 1;
    }
    int buttons;
    Menu *previous_menu = current_menu;

    current_menu = menu;
    output_log(LOGLEVEL_DEBUG, "Loading menu \"%s\"\n", current_menu->name);

    // Return error if the menu doesn't contain entires
    if (current_menu->num_entries == 0) {
        output_log(LOGLEVEL_ERROR, 
            "Error: No valid entries found for Menu \"%s\"", 
            current_menu->name
        );
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
    calculate_button_geometry(current_menu->root_entry, buttons);
    highlight->rect.x = current_entry->icon_rect.x - config.highlight_hpadding;
    highlight->rect.y = current_entry->icon_rect.y - config.highlight_vpadding;
    return 0;
}

// A function to load a menu by its name
static int load_menu_by_name(const char *menu_name, bool set_back_menu, bool reset_position)
{
    Menu *menu = get_menu(menu_name);
    return load_menu(menu, set_back_menu, reset_position);
}

// A function to calculate the layout of the buttons
static void calculate_button_geometry(Entry *entry, int buttons)
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
            entry->text_rect.y = entry->icon_rect.y + config.icon_size + entry->title_offset + 
                                 config.title_padding;
            entry = entry->next;
    }
}

// A function to render all buttons (icon and text) for a menu
static void render_buttons(Menu *menu)
{
    Entry *entry;
    int h;
    for (entry = menu->first_entry; entry != NULL; entry = entry->next) {
        entry->icon = load_texture_from_file(entry->icon_path);
        entry->icon_selected = (entry->icon_selected_path != NULL) ? load_texture_from_file(entry->icon_selected_path) : NULL;
        entry->title_texture = render_text_texture(entry->title,
                                   &title_info, 
                                   &entry->text_rect,
                                   &h
                               );
        if (config.title_oversize_mode == MODE_SHRINK && h != geo.font_height) {
            entry->title_offset = (geo.font_height - h) / 2;
        }
    }
    menu->rendered = true;
}

// A function to move the selection left when clicked by user
static void move_left()
{
    // If we are not in leftmost position, move highlight left
    if (current_menu->highlight_position > 0) {
        highlight->rect.x -= geo.x_advance;
        current_menu->highlight_position--;
        current_entry = current_entry->previous;
    }

    // If we are in leftmost position, but there is a previous page, load the previous page
    else if (current_menu->highlight_position == 0 && current_menu->page > 0) {
        int buttons = config.max_buttons;
        current_entry = current_entry->previous;
        current_menu->root_entry = advance_entries(current_menu->root_entry,buttons,DIRECTION_LEFT);
        calculate_button_geometry(current_menu->root_entry, buttons);
        highlight->rect.x = current_entry->icon_rect.x - config.highlight_hpadding;
        current_menu->page--;
        current_menu->highlight_position = buttons - 1;
    }
}

// A function to move the selection right when clicked by the user
static void move_right()
{
    // If we are not in the rightmost position, move highlight right
    if (current_menu->highlight_position < (geo.num_buttons - 1)) {
        highlight->rect.x += geo.x_advance;
        current_menu->highlight_position++;
        current_entry = current_entry->next;
    }

    // If we are in the rightmost postion, but there are more entries in the menu, load next page
    else if (current_menu->highlight_position + current_menu->page*config.max_buttons <
    (current_menu->num_entries - 1)) {
        int buttons = current_menu->num_entries - (current_menu->page + 1)*config.max_buttons;
        if (buttons > config.max_buttons) {
            buttons = config.max_buttons;
        }
        current_entry = current_entry->next;
        current_menu->root_entry = current_entry;
        calculate_button_geometry(current_menu->root_entry, buttons);
        highlight->rect.x = current_entry->icon_rect.x - config.highlight_hpadding;
        current_menu->page++;
        current_menu->highlight_position = 0;
    }
}

// A function to load a submenu
static void load_submenu(const char *submenu)
{
    current_menu->last_selected_entry = current_entry;
    load_menu_by_name(submenu, true, true);
}

// A function to load the previous menu
static void load_back_menu(Menu *menu)
{
    load_menu(menu->back, false, config.reset_on_back);
}

// A function to update the screen with all visible textures
static void draw_screen()
{
    // Draw background
    SDL_RenderClear(renderer);
    
    if (config.background_mode == MODE_IMAGE || config.background_mode == MODE_SLIDESHOW) {
        SDL_RenderCopy(renderer, background_texture, NULL, NULL);
    }

    if (config.background_mode == MODE_SLIDESHOW && state.slideshow_transition) {
        SDL_RenderCopy(renderer, slideshow->transition_texture, NULL, NULL);
    }

    // Draw background overlay
    if (config.background_overlay) {
        SDL_RenderCopy(renderer, background_overlay, NULL, NULL);
    }

    // Draw scroll indicators
    if (config.scroll_indicators &&
    (current_menu->page*config.max_buttons + geo.num_buttons) <= (current_menu->num_entries - 1)) {
        SDL_RenderCopy(renderer, scroll->texture, NULL, &scroll->rect_right);
    }
    if (config.scroll_indicators && current_menu->page > 0) {
        SDL_RenderCopyEx(renderer, scroll->texture, NULL, &scroll->rect_left, 0, NULL, SDL_FLIP_HORIZONTAL);
    }

    // Draw clock
    if (config.clock_enabled) {
        SDL_RenderCopy(renderer, clk->time_texture, NULL, &clk->time_rect);
        if (config.clock_show_date) {
            SDL_RenderCopy(renderer, clk->date_texture, NULL, &clk->date_rect);
        }
    }

    // Draw highlight
    SDL_RenderCopy(renderer,
        highlight->texture,
        NULL,
        &highlight->rect
    );

    // Draw buttons
    Entry *entry = current_menu->root_entry;
    SDL_Texture *icon;
    for (int i = 0; i < geo.num_buttons; i++) {
        icon = (entry->icon_selected != NULL && i == current_menu->highlight_position) ? entry->icon_selected : entry->icon;
        SDL_RenderCopy(renderer, icon, NULL, &entry->icon_rect);
        SDL_RenderCopy(renderer, entry->title_texture, NULL, &entry->text_rect);
        entry = entry-> next;
    }

    // Draw screensaver
    if (state.screensaver_active) {
        SDL_RenderCopy(renderer, screensaver->texture, NULL, NULL);
    }

    // Output to screen
    SDL_RenderPresent(renderer);
}

// A function to execute the user's command
static void execute_command(const char *command)
{
    // Copy command into separate buffer
    char *cmd;
    copy_string_alloc(&cmd, command);

    // Parse special commands
    if (cmd[0] == ':') {
        char *delimiter = " ";
        char *special_command = strtok(cmd, delimiter);
        if (!strcmp(special_command, SCMD_SUBMENU)) {
            char *submenu = strtok(NULL, "");
            if (submenu != NULL) {
                load_submenu(submenu);
            }
        }
        else if (!strcmp(special_command, SCMD_FORK)) {
            char *fork_command = strtok(NULL, "");
            if (fork_command != NULL) {
                start_process(fork_command, false);
            }
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
            load_menu(default_menu, false, true);
        }
        else if (!strcmp(special_command, SCMD_BACK)) {
            load_back_menu(current_menu);
        }
        else if (!strcmp(special_command, SCMD_QUIT)) {
            quit(EXIT_SUCCESS);
        }
        else if (!strcmp(special_command, SCMD_SHUTDOWN)) {
            scmd_shutdown();
        }
        else if (!strcmp(special_command, SCMD_RESTART)) {
            scmd_restart();
        }
        else if (!strcmp(special_command, SCMD_SLEEP)) {
            scmd_sleep();
        }
    }

    // Launch external application
    else {

        // Perform prelaunch behavior from OnLaunch setting
        if (config.on_launch == MODE_HIDE) {
            SDL_HideWindow(window);
        }
        else if (config.on_launch == MODE_BLANK) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);
        }
        if (gamepad != NULL) {
            SDL_GameControllerClose(gamepad);
            gamepad = NULL;
        }

        // Launch application
        SDL_Delay(50);
        launch_application(cmd);

        // Rebaseline the timing after the program is done
        ticks.main = SDL_GetTicks();
        ticks.last_input = ticks.main;
        
        // Post-application updates
        if (config.gamepad_enabled) {
            connect_gamepad(config.gamepad_device);
        }
        if (config.clock_enabled) {
            update_clock(true);
        }
        if (config.background_mode == MODE_SLIDESHOW) {
            resume_slideshow();
        }
        if (config.on_launch == MODE_HIDE) {
            SDL_ShowWindow(window);
        }
        else if (config.on_launch == MODE_BLANK) {
            set_draw_color();
        }
    
// Prevent any duplicate keypresses that were used to exit the program (Wayland workaround)
#ifdef __unix__
        SDL_Delay(50);
        SDL_PumpEvents();
        SDL_FlushEvent(SDL_KEYDOWN);
#endif
    }
    free(cmd);
}

// A function to launch an external application
static void launch_application(char *cmd)
{
    bool successful = start_process(cmd, true);
    if (!successful) 
        return;
    bool has_focus = true;
    
// Initialize exit hotkey for Windows
#ifdef _WIN32
    if (has_exit_hotkey()) {
        SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
    }
#endif

    // Check for events while the application is running
    do {
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    quit(EXIT_SUCCESS);
                    break;

                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                        has_focus = false;
                        output_log(LOGLEVEL_DEBUG, "Lost focus\n");
                    }
                    break;

#ifdef _WIN32
                case SDL_SYSWMEVENT:
                    check_exit_hotkey(event.syswm.msg);
                    break;
#endif
            }
        }

        // Keep drawing the screen until the application has created its window
        if (config.on_launch == MODE_NONE && has_focus) {
            draw_screen();
        }
        else if (config.on_launch == MODE_BLANK && has_focus) {
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);
         }
     
        // We're in the background, keep waiting until the application finishes
        else {
            SDL_Delay(APPLICATION_WAIT_PERIOD);
        }
    } while(process_running());

#ifdef _WIN32
    SDL_EventState(SDL_SYSWMEVENT, SDL_DISABLE);
#endif

    output_log(LOGLEVEL_DEBUG, "Application finished\n");
}

// A function to connect to a gamepad
static void connect_gamepad(int device_index)
{
    gamepad = SDL_GameControllerOpen(device_index);
    if (gamepad == NULL) {
        output_log(LOGLEVEL_ERROR, 
            "Error: Could not open gamepad at device index %i\n", 
            config.gamepad_device
        );
        return;
    }
    if (config.debug) {
        char *mapping = SDL_GameControllerMapping(gamepad);
        output_log(LOGLEVEL_DEBUG, 
            "Gamepad Mapping:\n%s\n", 
            mapping
        );
        SDL_free(mapping);
    }
}

// A function to poll the connected gamepad for commands
static void poll_gamepad()
{
    int value_multiplier; // Handles positive or negative axis
    for (GamepadControl *i = gamepad_controls; i != NULL; i = i->next) {
        
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
static void update_slideshow()
{
    // If image duration time has elapsed, load the next image and start the transition
    if (!state.slideshow_transition && (ticks.main - ticks.slideshow_load > config.slideshow_image_duration) &&
    !state.slideshow_paused) {
        
        // Render the new background image in a separate thread so we don't block the main thread
        if (!state.slideshow_background_rendering && !state.slideshow_background_ready) {
            Slideshowhread = SDL_CreateThread(load_next_slideshow_background_async, "Slideshow Thread", (void*) slideshow);
            state.slideshow_background_rendering = true;
        }

        // Convert background to texture after the rendering thread has completed
        else if (state.slideshow_background_ready) {
            SDL_WaitThread(Slideshowhread, NULL);
            Slideshowhread = NULL;
            if (config.slideshow_transition_time > 0) {
                slideshow->transition_texture = load_texture(slideshow->transition_surface);
                SDL_SetTextureAlphaMod(slideshow->transition_texture, 0);
                state.slideshow_transition = true;
            }
            else {
                SDL_DestroyTexture(background_texture);
                background_texture = load_texture(slideshow->transition_surface);
                ticks.slideshow_load = ticks.main;
            }
        slideshow->transition_surface = NULL;
        state.slideshow_background_ready = false;
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
    }
}

// A function to update the screensaver
static void update_screensaver()
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
        }
    }
}

// A function to update the clock display
static void update_clock(bool block)
{
    if (ticks.main - ticks.clock_update > CLOCK_UPDATE_PERIOD) {
        if (!state.clock_rendering) {

            // Check to see if the time has changed
            get_time(clk);
            if (clk->render_time) {
                state.clock_rendering = true;
                if (block) {
                    render_clock(clk);
                }
                else {
                    clock_thread = SDL_CreateThread(render_clock_async, "Clock Thread", (void*) clk);
                }    
            }
            else {
                ticks.clock_update = ticks.main;
            }
        }

        // Render texture
        if (state.clock_ready) {
            SDL_WaitThread(clock_thread, NULL);
            clock_thread = NULL;
            SDL_DestroyTexture(clk->time_texture);
            clk->time_texture = load_texture(clk->time_surface);
            clk->time_surface = NULL;
            if (clk->render_date) {
                SDL_DestroyTexture(clk->date_texture);
                clk->date_texture = load_texture(clk->date_surface);
                clk->date_surface = NULL;
            }
            ticks.clock_update = ticks.main;
            clk->render_time = false;
            clk->render_date = false;
            state.clock_rendering = false;
            state.clock_ready = false;
        }
    }
}

// A function to quit the launcher
void quit(int status)
{
    output_log(LOGLEVEL_DEBUG, "Quitting program\n");
    cleanup();
    if (config.quit_cmd != NULL) {
        execute_command(config.quit_cmd);
        free(config.quit_cmd);
    }
    exit(status);
}

// A function to print the version and other info to command line
void print_version(FILE *stream)
{
    SDL_version sdl_version;
    SDL_GetVersion(&sdl_version);
    SDL_version *img_version = IMG_Linked_Version();
    SDL_version *ttf_version = TTF_Linked_Version();
    fprintf(stream, PROJECT_NAME " version " PROJECT_VERSION ", using:\n");
    fprintf(stream, "  SDL       %u.%u.%u\n", sdl_version.major, sdl_version.minor, sdl_version.patch);
    fprintf(stream, "  SDL_image %u.%u.%u\n", img_version->major, img_version->minor, img_version->patch);
    fprintf(stream, "  SDL_ttf   %u.%u.%u\n", ttf_version->major, ttf_version->minor, ttf_version->patch);
}

int main(int argc, char *argv[]) 
{
    int error;
    char *config_file_path = NULL;
    config.exe_path = SDL_GetBasePath();

    // Handle command line arguments, find config file
    handle_arguments(argc, argv, &config_file_path);

    // Parse config file for settings and menu entries
    parse_config_file(config_file_path);
    free(config_file_path);

    // Initialize SDL, verify all settings are in their allowable range
    init_sdl();
    init_sdl_image();
    init_sdl_ttf();
    validate_settings(&geo);
    
    // Initialize slideshow
    if (config.background_mode == MODE_SLIDESHOW) {
        init_slideshow();
    }

    // Initialize Nanosvg, create window and renderer
    init_svg();
    create_window();

    // Initialize timing
    ticks.main = SDL_GetTicks();
    ticks.last_input = ticks.main;

    // Load gamepad overrides
    if (config.gamepad_enabled && config.gamepad_mappings_file != NULL) {
        error = SDL_GameControllerAddMappingsFromFile(config.gamepad_mappings_file);
        if (error < 0) {
            output_log(LOGLEVEL_ERROR, 
                "Error: Could not load gamepad mappings from %s\n%s\n", 
                config.gamepad_mappings_file,
                SDL_GetError()
            );
        }
    }

    // Render background
    if (config.background_mode == MODE_IMAGE) {
        if (config.background_image == NULL) {
            output_log(LOGLEVEL_ERROR, "Error: BackgroundImage not specified in config file\n");
        }
        else {
            background_texture = load_texture_from_file(config.background_image);
        }

        // Switch to color mode if loading background image failed
        if (background_texture == NULL) {
            config.background_mode = MODE_COLOR;
            output_log(LOGLEVEL_ERROR, "Error: Couldn't load background image, defaulting to color background\n");
            set_draw_color();
        }
    }

    // Render first slideshow image
    else if (config.background_mode == MODE_SLIDESHOW) {
        SDL_Surface *surface = load_next_slideshow_background(slideshow, false);
        background_texture = load_texture(surface);
    }

    // Initialize screensaver
    if (config.screensaver_enabled) {
        init_screensaver();
    }

    // Initialize clock
    if (config.clock_enabled) {
        clk = malloc(sizeof(Clock));
        init_clock(clk);
        ticks.clock_update = ticks.main;
    }
    
    // Render highlight
    int button_height = config.icon_size + config.title_padding + geo.font_height;
    highlight = malloc(sizeof(Highlight));
    highlight->texture = render_highlight(config.icon_size + 2*config.highlight_hpadding,
                             button_height + 2*config.highlight_vpadding,
                             config.highlight_rx,
                             &highlight->rect
                         );

    // Render scroll indicators
    if (config.scroll_indicators) {
        scroll = malloc(sizeof(Scroll));
        scroll->texture = NULL;
        int scroll_indicator_height = (int) ((float) geo.screen_height * SCROLL_INDICATOR_HEIGHT);
        render_scroll_indicators(scroll, scroll_indicator_height, &geo);
    }

    // Render background overlay
    if (config.background_overlay) {
        SDL_Surface *overlay_surface = NULL;
        overlay_surface = SDL_CreateRGBSurfaceWithFormat(0, 
                              geo.screen_width, 
                              geo.screen_height, 
                              32,
                              SDL_PIXELFORMAT_ARGB8888
                          );
        Uint32 overlay_color = SDL_MapRGBA(overlay_surface->format, 
                                   config.background_overlay_color.r, 
                                   config.background_overlay_color.g, 
                                   config.background_overlay_color.b, 
                                   config.background_overlay_color.a
                               );
        SDL_FillRect(overlay_surface, NULL, overlay_color);
        background_overlay = load_texture(overlay_surface);
    }

    // Register exit hotkey with Windows
#ifdef _WIN32
    if (has_exit_hotkey()) {
        register_exit_hotkey();
    }
#endif

    // Print debug info to log
    if (config.debug) {
        debug_video(renderer, &display_mode);
        debug_settings();
        debug_gamepad(gamepad_controls);
        debug_hotkeys(hotkeys);    
        debug_menu_entries(config.first_menu, config.num_menus);
    }

    // Load the default menu and display it
    if (config.default_menu == NULL) {
        output_log(LOGLEVEL_FATAL, "Fatal Error: No default menu defined in config file\n");
    }
    default_menu = get_menu(config.default_menu);
    if (default_menu == NULL) {
        output_log(LOGLEVEL_FATAL, "Fatal Error: Default menu %s not found in config file\n", config.default_menu);
    }
    error = load_menu(default_menu, false, true);
    if (error) {
        output_log(LOGLEVEL_FATAL, "Fatal Error: Could not load default menu %s\n", config.default_menu);
    }

    // Execute startup command
    if (config.startup_cmd != NULL) {
        execute_command(config.startup_cmd);
    }
    
    // Main program loop
    output_log(LOGLEVEL_DEBUG, "Begin program loop\n");
    while (1) {
        ticks.main = SDL_GetTicks();
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    quit(EXIT_SUCCESS);
                    break;

                case SDL_KEYDOWN:
                    ticks.last_input = ticks.main;
                    handle_keypress(&event.key.keysym);
                    break;
                
                case SDL_MOUSEBUTTONDOWN:
                    if (config.mouse_select && event.button.button == SDL_BUTTON_LEFT) {
                        ticks.last_input = ticks.main;
                        execute_command(current_entry->cmd);
                    }
                    break;

                case SDL_JOYDEVICEADDED:
                    if (SDL_IsGameController(event.jdevice.which) == SDL_TRUE) {
                        output_log(LOGLEVEL_DEBUG, 
                            "Gamepad connected with device index %i\n", 
                            event.jdevice.which
                        );
                        if (event.jdevice.which == config.gamepad_device) {
                            connect_gamepad(event.jdevice.which);
                        }
                    }
                    break;

                case SDL_JOYDEVICEREMOVED:
                    if (event.jdevice.which == config.gamepad_device && gamepad != NULL) {
                        output_log(LOGLEVEL_DEBUG, "Gamepad disconnected\n");
                        SDL_GameControllerClose(gamepad);
                        gamepad = NULL;
                    }
                    break;

                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                        output_log(LOGLEVEL_DEBUG, "Lost keyboard focus\n");
                        
                        // Sometimes the launcher loses focus when autostarting on Windows
#ifdef _WIN32
                        set_foreground_window();
#endif
                    }
                    else if (event.window.event == SDL_WINDOWEVENT_LEAVE) {
                        output_log(LOGLEVEL_DEBUG, "Lost mouse focus\n");
                    }
                    break;
            }
        }

        // Post-event loop updates
        if (gamepad != NULL) {
            poll_gamepad();
        }
        if (config.background_mode == MODE_SLIDESHOW) {
            update_slideshow();
        }
        if (config.screensaver_enabled) {
            update_screensaver();
        }        
        if (config.clock_enabled) {
            update_clock(false);
        }

        // Draw the screen
        draw_screen();
    }
    quit(EXIT_SUCCESS);
}