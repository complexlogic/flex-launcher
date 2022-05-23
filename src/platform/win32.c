#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>
#include <psapi.h>
#include <SDL.h>
#include <SDL_syswm.h>
#include "../launcher.h"
#include <launcher_config.h>
#include "platform.h"
#include "win32.h"
#include "../util.h"
#include "../debug.h"
#include "slideshow.h"

extern config_t config;
extern SDL_SysWMinfo wm_info;
bool web_browser;
bool browser_launched;
bool has_shutdown_privilege     = false;
char *browser_process           = NULL;
HANDLE child_process            = NULL;
UINT exit_hotkey                = 0;

// A function to get the basename of a file
static char *path_basename(const char *path)
{
    size_t length = strlen(path);
    if (length) {
        // Find first path separator, count back from end of string
        char *p = path + length - 1;
        while (*p != '\\' && *p != '/' && p > path) {
            p -= 1;
        } 
        return p + 1;
    }
    return NULL;
}

// A function to determine if a process name is a web browser
static bool is_browser(const char *exe_basename)
{
    static const char *browsers[] = {
        "chrome.exe",
        "msedge.exe",
        "firefox.exe"
    };
    for (int i = 0; i < sizeof(browsers) / sizeof(browsers[0]); i++) {
        if (!strcmp(exe_basename, browsers[i])) {
            return true;
        }
    }
    return false;
}

// A function to determine if a file exists on the filesystem
bool file_exists(const char *path)
{
    if (_access(path, 4) == 0) {
        return true;
    }
    else {
        return false;
    }
}

// A function to determine if a directory exists on the filesystem
bool directory_exists(const char *path)
{
    if (!file_exists(path)) {
        return false;
    }
    else {
        DWORD attributes = GetFileAttributesA(path);
        if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
            return true;
        }
        else {
            return false;
        }
    }
}

// A function that parses the command string into a file and parameters
static void parse_command(char *cmd, char *file, size_t file_size, char **params)
{
    char *start = NULL;
    char *quote_begin = NULL;
    char *quote_end = NULL;
    char *p = cmd;
    file[0] = '\0';

    // Skip any whitespace at beginning of command
    while (*p == ' ') {
        p++;
    }
    start = p;

    // Check for quote, in which case ignore spaces until the end quote is detecetd
    if (*p == '"') {
        quote_begin = p;
    }

    while (*p != '\0') {
        // If the quote is complete, copy the file
        if (*p == '"' && p != quote_begin) {
            quote_end = p;
            *p = '\0';
            copy_string(file, quote_begin + 1, file_size);
        }

        else if (*p == ' ') {
            // If a space was detected but there hasn't been a quote detected yet, this is the end of the file
            if (!quote_begin) {
                *p = '\0';
                copy_string(file, start, file_size);
                p++;

                // Skip any preceding white space for parameters
                while (*p == ' ') {
                    p++;
                }

                // Copy parameters
                if (*p != '\0') {
                    copy_string_alloc(params, p);
                    break;
                }
            }

            // If a space was detected after the quote
            else if (quote_begin && quote_end) {
                // Skip any preceding white space for parameters
                while (*p == ' ') {
                    p++;
                }

                // Copy rest of command as parameters
                if (*p != '\0') {
                    copy_string_alloc(params, p);
                }
                break;
            }
        }
        p++;
    }

    // If there were no quotes or spaces, copy whole command into file buffer
    if (start && file[0] == '\0') {
        copy_string(file, start, file_size);
    }
}

// A function to determine if a process of a given name is running on the system
static bool process_running_name(const char *target_process)
{
    static Uint32 ticks = 0;
    Uint32 current_ticks = SDL_GetTicks();

    // Only check the process if we've exceeded the process check interval
    if (current_ticks - ticks < BROWSER_CHECK_PERIOD) {
        return true;
    }
    else {
        ticks = current_ticks;

        // Generate an array of process IDs
        DWORD processes[1024], needed, num_processes;
        if (!EnumProcesses(processes, sizeof(processes), &needed)) {
            return true;
        }
        num_processes = needed / sizeof(DWORD);
        char current_process[MAX_PATH + 1];
        HANDLE process = NULL;
        HMODULE module = NULL;
        DWORD flags = PROCESS_QUERY_INFORMATION | PROCESS_VM_READ;

        // Get the image name of each running process
        for (int i = 0; i < num_processes; i++) {
            process = OpenProcess(flags, FALSE, processes[i]);
            if (process != NULL && 
            EnumProcessModulesEx(process, &module, sizeof(module), &needed, LIST_MODULES_ALL)) {
                GetModuleBaseNameA(process, module, current_process, sizeof(current_process));
                
                // Check if the target process's name is the same as the current process
                if (!strcmp(current_process, target_process)) {
                    browser_launched = true;
                    CloseHandle(process);
                    return true;
                }
                CloseHandle(process);
            }
        }
        if (browser_launched) {
            return false;
        }
    }
    return true;
}

// A function to determine if the previously launched process is still running
bool process_running()
{
    if (web_browser) {
        bool running = process_running_name(browser_process);
        if (!running) {
            free(browser_process);
            browser_process = NULL;
        }
        return running;
    }
    else {
        DWORD status = WaitForSingleObject(child_process, 0);
        if (status == WAIT_OBJECT_0) {
            return false;
        }
        else {
            return true;
        }
    }
}

// A function to launch an application
bool start_process(char *cmd, bool application)
{
    char file[MAX_PATH_CHARS + 1];
    char *params = NULL;
    int cmd_show = application ? SW_SHOWMAXIMIZED : SW_HIDE;
    
    // Parse command into file and parameters strings
    parse_command(cmd, file, sizeof(file), &params);

    // Set up info struct
    SHELLEXECUTEINFOA info = {
        .cbSize = sizeof(SHELLEXECUTEINFOA),
        .fMask = SEE_MASK_NOCLOSEPROCESS,
        .hwnd = NULL,
        .lpVerb = "open",
        .lpFile = file,
        .lpParameters = params,
        .lpDirectory = NULL,
        .nShow = cmd_show,
        .lpIDList = NULL,
        .lpClass = NULL,
    };

    BOOL successful = ShellExecuteExA(&info);
    if (!application) 
        return true;
    
    if (successful) {
        child_process = info.hProcess;

        // Check if launched application is a web browser
        char process_name[MAX_PATH_CHARS + 1];
        DWORD ret = GetProcessImageFileNameA(child_process, process_name, sizeof(process_name));
        if (ret) {
            char *process_basename = path_basename(process_name);
            web_browser = is_browser(process_basename);
            if (web_browser) {
                copy_string_alloc(&browser_process, process_basename);
                browser_launched = false;
            }
        }

        // Go down in the window stack so the launched application can take focus
        if (config.on_launch == MODE_NONE || config.on_launch == MODE_BLANK) {
            HWND hwnd = wm_info.info.win.window;
            SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOREDRAW | SWP_NOSIZE | SWP_NOMOVE);
        }
    }
    else {
        output_log(LOGLEVEL_DEBUG, "Failed to launch command\n");
        free(params);
        return false;
    }
    free(params);
    return true;
}

// A function to scan the slideshow directory for image files
int scan_slideshow_directory(slideshow_t *slideshow, const char *directory)
{
    WIN32_FIND_DATAA data;
    HANDLE handle;
    char file_search[MAX_PATH_CHARS + 1];
    char file_output[MAX_PATH_CHARS + 1];
    char extension[10];
    int num_images = 0;

    // Generate a wildcard file search string for all supported image file extensions
    for (int i = 0; i < NUM_IMAGE_EXTENSIONS && num_images < MAX_SLIDESHOW_IMAGES; i++) {
        copy_string(extension, "*", sizeof(extension));
        strcat(extension, extensions[i]);
        join_paths(file_search, sizeof(file_search), 2, directory, extension);

        // Store every result into the slideshow struct
        handle = FindFirstFileA(file_search, &data);
        if (handle != INVALID_HANDLE_VALUE) {
            do {
                join_paths(file_output, sizeof(file_output), 2, directory, data.cFileName);
                copy_string_alloc(&slideshow->images[num_images], file_output);
                num_images++;
            } while (FindNextFileA(handle, &data) != 0 && num_images < MAX_SLIDESHOW_IMAGES);
        }
    }
    slideshow->num_images = num_images;
    return num_images;
}

// A function to get the 2 letter region code
void get_region(char *buffer)
{
    GEOID geo_id = GetUserGeoID(GEOCLASS_NATION);
    GetGeoInfoA(geo_id, GEO_ISO2, buffer, 3, 0);
}

// A function to shutdown the computer
void scmd_shutdown()
{
    if (!has_shutdown_privilege) {
        bool successful = get_shutdown_privilege();
        if (!successful) 
            return;
    }
    InitiateShutdownA(NULL, 
        NULL, 
        0, 
        SHUTDOWN_FORCE_OTHERS | SHUTDOWN_POWEROFF | SHUTDOWN_HYBRID,
        SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_MINOR_OTHER
    );
}

// A function to restart the computer
void scmd_restart()
{
    if (!has_shutdown_privilege) {
        bool successful = get_shutdown_privilege();
        if (!successful) 
            return;
    }
    InitiateShutdownA(NULL,
        NULL,
        0,
        SHUTDOWN_FORCE_OTHERS | SHUTDOWN_RESTART | SHUTDOWN_HYBRID,
        SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_MINOR_OTHER
    );
}

// A function to put the computer to sleep
void scmd_sleep()
{
    if (!has_shutdown_privilege) {
        bool successful = get_shutdown_privilege();
        if (!successful) 
            return;
    }
    SetSuspendState(FALSE, FALSE, FALSE);
}

// A function to get the shutdown privilege from Windows
static bool get_shutdown_privilege()
{
    HANDLE token = NULL;
    BOOL ret = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &token);
    if (!ret) {
        output_log(LOGLEVEL_ERROR, "Error: Could not open process token\n");
        return false;
    }
    LUID luid;
    ret = LookupPrivilegeValueA(NULL, SE_SHUTDOWN_NAME, &luid);
    if (!ret) {
        output_log(LOGLEVEL_ERROR, "Error: Failed to lookup privilege\n");
        CloseHandle(token);
        return false;
    }
    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    ret = AdjustTokenPrivileges(token, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
    if (!ret) {
        output_log(LOGLEVEL_ERROR, "Error: Failed to adjust token privileges\n");
        CloseHandle(token);
        return false;
    }
    has_shutdown_privilege = true;
    CloseHandle(token);
    return true;
}

// A function to check if there is an exit hotkey
bool has_exit_hotkey()
{
    if (exit_hotkey) 
        return true;
    return false;
}

// A function to store an exit hotkey
void set_exit_hotkey(SDL_Keycode keycode)
{
    if (exit_hotkey) 
        return;
    exit_hotkey = sdl_to_win32_keycode(keycode);
    if (!exit_hotkey) {
        output_log(LOGLEVEL_ERROR, "Error: Invalid exit hotkey keycode %X\n", keycode);
    }
}

// A function to register the exit hotkey with Windows
void register_exit_hotkey()
{
    BOOL ret = RegisterHotKey(wm_info.info.win.window, 1, 0, exit_hotkey);
    if (!ret) {
        exit_hotkey = 0;
        output_log(LOGLEVEL_ERROR, "Error: Failed to register exit hotkey with Windows\n");
    }
}

// A function to check if the exit hotkey was pressed, and close the active window if so
void check_exit_hotkey(SDL_SysWMmsg *msg)
{
    if (msg->msg.win.msg == WM_HOTKEY) {
        output_log(LOGLEVEL_DEBUG, "Exit hotkey detected\n");
        HWND hwnd = GetForegroundWindow();
        if (hwnd == NULL) {
            output_log(LOGLEVEL_ERROR, "Error: Could not get top window\n");
            return;
        }
        PostMessage(hwnd, WM_CLOSE, 0, 0);
    }
}

// A function to convert an SDL keycode to a WIN32 virtual keycode
static UINT sdl_to_win32_keycode(SDL_Keycode keycode)
{
#include "keycode_convert.h" // Import the conversion table
    for (int i = 0; i < sizeof(table) / sizeof(table[0]); i++) {
        if (table[i].sdl == keycode) {
            return table[i].win;
        }
    }
    return 0;
}