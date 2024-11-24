---
layout: default
title: Configuration
---
# Configuring Flex Launcher
## Table of Contents

1. [Overview](#overview)
2. [Settings](#settings)
    - [General](#general)
    - [Background](#background)
    - [Layout](#layout)
    - [Titles](#titles)
    - [Highlight](#highlight)
    - [Scroll Indicators](#scroll-indicators)
3. [Creating Menus](#creating-menus)
    - [Special Commands](#special-commands)
    - [Desktop Files (Linux Only)](#desktop-files-linux-only)
4. [Clock](#clock)
5. [Screensaver](#screensaver)
6. [Hotkeys](#hotkeys)
7. [Gamepad Controls](#gamepad-controls)
8. [Transparent Backgrounds](#transparent-backgrounds)

## Overview
Flex Launcher uses an [INI file](https://en.wikipedia.org/wiki/INI_file) to configure settings and menus. The INI file consists of sections enclosed in square brackets, and in each section there are entries which consist of a key and a value. Example:
```ini
[Section]
Key1=value
Key2=value
...
```
A line can be commented out by using the # character at the beginning of the line, which will cause the line to be ignored by the program. In-line comments are not allowable. Here are a few things to note about the configuration settings for Flex Launcher:
- All keys and values are case sensitive.
- Full UTF-8 character set is supported for titles.
- The following image formats are supported: JPEG, PNG, and WebP
- Relative paths are evaluated with respect to the *current working directory*, which may not be the same as the directory that the config file is located in. It is recommended to use absolute paths whenever possible to eliminate any confusion.
- Color is specified in 24 bit RGB HEX format prefixed with the # character, e.g. the color red should be `#FF0000`. The letters can be uppercase or lowercase. HEX color pickers can be easily found online to assist color choices.
- Several settings allow for values to be specified in pixels *or* as a percentage of another value. In this case, if no percent sign is detected it will be interpreted as pixels, and if the percent sign is present, than it will be interpreted as a percent value e.g. "5" means 5 pixels and "5%" means 5 percent.
- Shell variable expansion is generally not supported, e.g. you cannot use the ~ character to refer to your home directory. The exception is for commands, since those are passed through to your system shell.

## Settings
The following sections contain settings that control the look and behavior of the launcher:
- [General](#general)
- [Background](#background)
- [Layout](#layout)
- [Titles](#titles)
- [Highlight](#highlight)
- [Scroll Indicators](#scroll-indicators)

#### General
The settings in this section control the general behavior of the launcher.

- [DefaultMenu](#defaultmenu)
- [VSync](#vsync)
- [FPSLimit](#fpslimit)
- [OnLaunch](#onlaunch)
- [ResetOnBack](#resetonback)
- [MouseSelect](#mouseselect)
- [InhibitOSScreensaver](#inhibitosscreensaver)
- [StartupCmd](#startupcmd)
- [QuitCmd](#quitcmd)

##### DefaultMenu
This is the title of the main menu that shows when Flex Launcher is started. The value *must* match the name of one of your menu sections, or there will be an error and Flex Launcher will refuse to start. See the [Creating Menus](#creating-menus) section for more information.

##### VSync
Defines whether VSync will be used to synchronize the frame rate with the refresh rate of your monitor. This setting is a boolean "true" or "false"

Default: true

##### FPSLimit
When `VSync` is set to false, this setting defines the maximum number of frames per second that Flex Launcher will render. The minimum is 10, and the maximum is the same as the refresh rate of your monitor.

##### ApplicationTimeout
Defines the time in seconds that the launcher will wait for an application to launch. If the launcher does not lose the window focus before the timeout occurs, it assumes there was an error with the launched application.

Default: 15

##### OnLaunch
Defines the action that Flex Launcher will take upon the launch of an application. Possible values: "None", "Blank", and "Quit"
- None: Flex Launcher will maintain its window while waiting for the launched application to initialize.
- Blank: Flex Launcher will change to a blank, black screen while waiting for the launched application to initialize.
- Quit: Flex Launcher will quit immediately after the successful launch of an application.

Default: Blank

##### WrapEntries
Defines whether the highlight will wrap to the other side of the screen after reaching its leftmost or rightmost position. This setting is a boolean "true" or "false".

Default: false

##### ResetOnBack
Defines whether Flex Launcher will remember the previous entry position when going back to a previous menu. If set to true, the highlight will be reset to the first entry in the menu when going back. This setting is a boolean "true" or "false".

Default: false

##### MouseSelect
Defines whether the left mouse button can be used to select the highlighted entry. This setting is intended to support gyroscopic mouse devices where the enter/ok button functions as a mouse left click instead of the keyboard enter button. This setting is a boolean "true" or "false".

Default: false

##### InhibitOSScreensaver
Defines whether Flex Launcher will prevent your default OS screensaver from activating while it is running. On Windows, this will also inhibit any power saving features as well (e.g. autosleep). This setting is a boolean "true" or "false".

Default: true

##### StartupCmd
Defines a command that Flex Launcher will execute immediately upon startup. This can be used to autostart your favorite application.

##### QuitCmd
Defines a command that Flex Launcher will execute immediately before quitting. This can be used to do any mode switching or appplication starting to prepare your desktop, e.g. for maintenance.

#### Background
The settings in this section control what Flex Launcher will display in the background.

- [Mode](#mode)
- [Color](#color)
- [Image](#image)
- [SlideshowDirectory](#slideshowdirectory)
- [SlideshowImageDuration](#slideshowimageduration)
- [SlideshowTransitionTime](#slideshowtransitiontime)
- [ChromaKeyColor](#chromakeycolor)
- [Overlay](#overlay)
- [OverlayColor](#overlaycolor)
- [OverlayOpacity](#overlayopacity)

##### Mode
Defines what mode the background will be. Possible values: "Color", "Image", and "Slideshow"
- Color: The background will be a solid color.
- Image: The background will be an image.
- Slideshow: The background will be a series of images displayed in random order, with a fading transition between each image.
- Transparent: The background will be transparent. This is an advanced feature; users should read the [Transparent Backgrounds](#transparent-backgrounds) section before proceeding.

Default: Color

##### Color
When `Mode` is set to "Color", this setting defines the color of the background.

Default: #000000 (Black)

##### Image
When `Mode` is set to "Image", this setting defines the image to be displayed in the background. The value should be a path to an image file. If the image is not the same resolution as your desktop, it will be stretched accordingly.

##### SlideshowDirectory
When `Mode` is set to "Slideshow", this setting defines the directory (folder) which contains the images to display in the background. The value should be a path to a directory on your filesystem. The number of images that may be scanned from the directory is limited to 250.

##### SlideshowImageDuration
When `Mode` is set to "Slideshow", this setting defines the amount of time in seconds to display each image. Must be an integer value.

Default: 30

##### SlideshowTransitionTime
When `Mode` is set to "Slideshow", this setting defines the amount of time in seconds that the next background image will fade in. The fading transition may be disabled by setting this to 0, which will yield a "hard" transition between images. Decimal values are acceptable.

Default: 3

##### ChromaKeyColor
When `Mode` is set to "Transparent", this setting defines the color that will be applied to the background for chroma key transparency.

Default: #010101

##### Overlay
Defines whether the background overlay feature is enabled. The background overlay is a solid color, typically black, that is painted over your background to improve the contrast between the background and the text/icons. This setting is a boolean "true" or "false".

Default: false

##### OverlayColor
Defines the color of the background overlay.

Default: #000000 (Black)

##### OverlayOpacity
Defines the opacity of the background overlay. Must be a percent value.

Default: 50%

#### Layout
The settings in this section define the geometric layout of the launcher.

- [MaxButtons](#maxbuttons)
- [IconSize](#iconsize)
- [IconSpacing](#iconspacing)
- [VCenter](#vcenter)

##### MaxButtons
The maximum number of buttons that can be displayed on the screen. If a menu has more entries than this value, it will be split into multiple pages. A value of 3-5 is sensible for a typical TV size and viewing distance.

Default: 4

##### IconSize
The width and height of icons on the screen in pixels. If an icon is not the same resolution, it will be stretched accordingly.

Default: 256

##### IconSpacing
Distance between the menu entry icons, in pixels or percent of the screen width.

Default: 5%

##### VCenter
Defines the vertical centering of the menu entries in percent of the screen height. A value of 50% will cause the buttons to be centered halfway in the screen. Increasing the value will lower the buttons, and lowering it will raise them.

Default: 50%

#### Titles
The settings in this section affect the application titles that display below the icons.

- [Enabled](#enabled)
- [Font](#font)
- [FontSize](#fontsize)
- [Color](#color-1)
- [Shadows](#shadows)
- [ShadowColor](#shadowcolor)
- [Opacity](#opacity)
- [OversizeMode](#oversizemode)
- [Padding](#padding)

##### Enabled
Defines whether or not application titles are enabled. This setting is a boolean "true" or "false".

Default: true

##### Font
Defines the font to use for the titles of the menu entries. The value should be the path to a TrueType (TTF) font file. Non-TTF font formats are not supported. Flex Launcher ships with a handful of libre fonts.

Default: OpenSans

##### FontSize
Defines the font size of each menu entry title.

Default: 36

##### Color
Defines the color of the menu entry titles.

Default: #FFFFFF (White)

##### Shadows
Defines whether shadows are enabled for the menu titles. Shadows give a 3D textured appearance to the text to improve the contrast from the background. This setting is a boolean "true" or "false".

Default: false

##### ShadowColor
Defines the color of the title shadows.

Default: #000000 (Black)

##### Opacity
Defines the opacity of the menu entry titles. Must be a percent value.

Default: 100%

##### OversizeMode
Defines the behavior when the width of a menu entry title exceeds the width of its icon (which is defined in `IconSize`). Possible values: "Truncate", "Shrink", and "None"
- Truncate: Truncates the title at the maximum width and adds "..." to the end.
- Shrink: Shrinks oversized titles to a smaller font size than `TitleFontSize` so that the entire title fits within the maximum width.
- None: No action is taken to limit the width of titles. Overlaps with other titles may occur, and it is the user's responsibility to manually handle any such case.

Default: Truncate

##### Padding
Defines the vertical spacing between an icon and its title, in pixels.

Default: 20

#### Highlight
The settings in this section control the menu highlight.

- [Enabled](#enabled-1)
- [FillColor](#fillcolor)
- [FillOpacity](#fillopacity)
- [OutlineSize](#outlinesize)
- [OutlineColor](#outlinecolor)
- [OutlineOpacity](#outlineopacity)
- [CornerRadius](#cornerradius)
- [VPadding](#vpadding)
- [HPadding](#hpadding)

##### Enabled
Defines whether or not the highlight is enabled. If a user disables the highlight, it is assumed that they will be using the [Sected Icon Overrides](#selected-icon-overrides) feature instead. This setting is a boolean "true" or "false".

Default: true

##### FillColor
Defines the fill color of the highlight cursor.

Default: #FFFFFF (White)

##### FillOpacity
Defines the fill opacity of the highlight cursor. Must be a percent value.

Default: 25%

##### OutlineSize
Defines the stroke width in pixels of the outline of the highlight cursor. Setting this to 0 will disable the outline.

Default: 0

##### OutlineColor
Defines the outline color of the highlight cursor.

Default: #0000FF (Blue)

##### OutlineOpacity
Defines the outline opacity of the highlight cursor. Must be a percent value.

Default: 100%

##### CornerRadius
Defines the corner radius of the highlight cursor, in pixels. A value of 0 will yield a plain rectangle. Increasing the value will yield a rounded rectangle with increasingly round corners. The value of `HighlightOutlineSize` must be 0, otherwise this setting will be ignored.

Default: 0

##### VPadding
Defines the amount of vertical distance that the highlight cursor extends beyond the top and bottom of the menu entry icon, in pixels.

Default: 30

##### HPadding
Defines the amount of horizontal distance that the highlight cursor extends beyond the left and right of the menu entry icon, in pixels.

Default: 30

#### Scroll Indicators
The settings in this section pertain to scroll indicators. Scroll indicators are arrows which appear in the bottom left and/or bottom right of the screen to inform the user that there are additional pages of applications to scroll to.

- [Enabled](#enabled-2)
- [FillColor](#fillcolor-1)
- [OutlineSize](#outlinesize-1)
- [OutlineColor](#outlinecolor-1)
- [Opacity](#opacity-1)

##### Enabled
Defines whether scroll indicators will be enabled in the event that a menu has multiple pages of entries. This setting is a boolean "true" or "false".

Default: true

##### OutlineSize
Defines the stroke width in pixels of the scroll indicator outline. Setting this to 0 will disable the outline.

Default: 0

##### FillColor
Defines the fill color of the scroll indicators.

Default: #FFFFFF (White)

##### OutlineColor
Defines the color of the scroll indicator outline.

Default: #000000 (Black)

##### Opacity
Defines the opacity of the scroll indicators. Must be a percent value.

Default: 100%

## Creating Menus
At least one menu must be defined in the configuration file, and the title must match the `DefaultMenu` setting value. The title of a menu is its section name. Any title may be used that is not reserved for another section, such as "Settings", "Gamepad", etc. The entries of the menu are implemented as key=value pairs. The name of the key will be ignored by the program, and is therefore arbtrary. However, it is recommended to pick something intutitive such as Entry1, Entry2, Entry3, etc. The entry information is contained in the value.

Each entry value contains 3 parts of information in order: the title, the icon image path, and the command to run when the button is clicked. These are delimited by semicolons:
```ini
Entry=title;icon_path;command
```
The command is typically one of the following:
1. The path to the program executable that you want to launch 
2. Windows: the path to a program shortcut (.lnk file)
3. Linux: the path to a [.desktop file](#desktop-files-linux-only)
4. A [special command](#special-commands)
5. The path to an executable script, in the case that you want to perform multiple actions upon program launch.

 A simple example menu titled `Media` is shown below:
```ini
[Media]
Entry1=Kodi;C:\Pictures\Icons\kodi.png;"C:\Program Shortcuts\kodi.lnk"
Entry2=Netflix;C:\Pictures\Icons\netflix.png;"C:\Program Shortcuts\netflix.lnk"
Entry3=Plex;C:\Pictures\Icons\plex.png;"C:\Program Shortcuts\plex.lnk"
Entry4=Back;C:\Pictures\Icons\back.png;:back
```

### Selected Icon Overrides
The Selected Icon Override feature allows the user to define a different icon for the launcher to display when an entry is highlighted. To use this feature, name the path of the selected icon the same as the default entry icon path, but with a suffix of `_selected` (not including the file extension).

For example, if the icon path for an entry is defined as `C:\icons\kodi.png`, then the program will check for the existence of `C:\icons\kodi_selected.png` and, if it exists, this icon will be shown when the entry is selected instead of the default. This feature allows the user to implement custom highlight effects such as glowing, color changes, etc.

### Special Commands
Special commands are commands that are internal to Flex Launcher and begin with a colon. The following is a list of special commands:

#### :submenu
Change to a different menu. Requires a menu title as an argument. For example, the command `:submenu Games` will change to the menu `Games`. The argument must be a valid menu title that is defined elsewhere in the config file.

#### :fork
Forks a new process and executes a command in it without exiting the launcher. This is typically used in combination with a [hotkey](#hotkeys). Use this special command when you want to execute a command on your system for some reason other than launching a graphical application. Example use cases:
- Change a Wi-Fi connection
- Pair or connect a Bluetooth device
- Start or stop some system service/daemon

The :fork special command requires a command as an argument. For example `:fork command arguments` will execute `command arguments` without leaving the launcher.

Windows users should invoke a command line interpreter such as Command Prompt and pass the command to run as an argument, e.g. `:fork cmd.exe /c "command arguments"`

#### :exit
Windows only. Quits the currently running application. This special command is only available as a hotkey command. See the [Exit Hotkey](#exit-hotkey-windows-only) section for more information.

#### :back
Go back to the previous menu.

#### :home
Change to the menu defined in the `DefaultMenu` setting.

#### :quit
Quit Flex Launcher.

#### :left
Move the highlight cursor left.

#### :right
Move the highlight cursor right.

#### :select
Press enter on the current selection. This special command is only available as a gamepad or hotkey command, it is forbidden for menu entries.

#### :shutdown
Shut down the computer.<sup>1</sup>

#### :restart
Restart the computer.<sup>1</sup>

#### :sleep
Put the computer to sleep.<sup>1</sup>

<sup>1</sup> *Linux: Works in systemd-based distros only. Non-systemd distro users need to implement the command manually for their init system.*

### Desktop Files (Linux Only)
If the application you want to launch was installed via your distro's package manager, a .desktop file was most likely provided. The command to launch a Linux application can simply be the path to its .desktop file, and Flex Launcher will run the Exec command that the developers have specified in the file. Desktop files are located in /usr/share/applications.

#### Desktop Actions
Some .desktop files contain "Actions", which affect how the program is launched. An action may be specified by delimiting it from the path to the .desktop file with a semicolon. For example, Steam has a mode called "Big Picture Mode", which provides an interface similar to a game console and is ideal for a living room PC. The action in the .desktop file is called "BigPicture". A sample menu entry to launch Steam in Big Picture mode is shown below:
```
Entry1=Steam;/path/to/steamicon.png;/usr/share/applications/steam.desktop;BigPicture
```

## Clock
Flex Launcher contains a clock widget, which displays the current time, and, optionally, the current date. The following settings may be used to control the behavior of the clock.

#### Enabled
Defines whether or not the clock is enabled. This setting is a boolean "true" or "false".

Default: false

#### ShowDate
Defines whether or not the current date should be shown in addition to the current time. This setting is a boolean "true" or "false".

Default: false

#### Alignment
Defines which side of the screen the clock text should align to. Possible values: "Left" and "Right"

Default: Left

#### Font
Defines the font to use for the clock text. The value should be the path to a TrueType (TTF) font file.

Default: SourceSansPro

#### FontSize
Defines the font size of the clock text

Default: 50

#### Margin
Defines the distance of the clock text from the top and side of the screen, in pixels or percent of the screen height.

Default: 5%

#### FontColor
Defines the color of the clock text.

Default: #FFFFFF (White)

#### Shadows
Defines whether shadows are enabled for the clock text. Shadows give a 3D textured appearance to the text to improve the contrast from the background. This setting is a boolean "true" or "false".

Default: false

#### ShadowColor
Defines the color of the clock text shadows.

Default: #000000 (Black)

#### Opacity
Defines the opacity of the clock text. Must be a percent value.

Default: 100%

#### TimeFormat
Defines the format of the current time. Possible values: "24hr", "12hr", and "Auto"
- 24hr: The clock will be a 24 hour format.
- 12hr: The clock will be a 12 hour format, including AM/PM designation in your locale.
- Auto: Automatically determine the time format based on your system locale.

Default: Auto

#### DateFormat
Defines the order of the month and day in the date. Possible values: "Little", "Big", "Auto"
- Little: The day will come before the month.
- Big: The month will come before the day.
- Auto: Automatically determine the date format based on your system locale.

#### IncludeWeekday
Defines whether the date format should include the abbreviated weekday in your system locale. This setting is a boolean "true" or "false"

Default: true

## Screensaver
Flex Launcher contains a screensaver feature, which will dim the screen after the input has been idle for the specified amount of time. Here are the settings that control the behavior of the screensaver

#### Enabled
Defines whether or not the screensaver is enabled. This setting is a boolean "true" or "false".

Default: false

#### IdleTime
Defines the amount of time in seconds that the input should be idle before activating the screensaver

Default: 300 (5 minutes)

#### Intensity
Defines the amount to dim the screen. Must be a percent value.

Default: 70%

#### PauseSlideshow
When `BackgroundMode` is set to "Slideshow", this setting defines whether or not the slideshow should be paused while the screensaver is active. This setting is a boolean "true" or "false".

Default: true

## Hotkeys
Flex Launcher supports configurable hotkeys, which executes a command when a specified key is pressed. Each hotkey consists of a key=value pair, where the key is an arbitrary name, and the value contains the SDL keycode of the hotkey and the command to run when it is pressed, delimited by a semicolon:
```ini
Hotkey=keycode;command
```
The keycode is a HEX prefixed with the # character. There are two ways to find a keycode for a given key. The first is to use the [lookup table provided by SDL](https://wiki.libsdl.org/SDLKeycodeLookup). The name of each key is in the right column of the table, and the corresponding HEX keycode is in the center column. The second is to run Flex Launcher in debug mode, press the key, then check the log. For each keystroke, the name of the key will be printed and the HEX value will be in parenthesis next to it.

Any key can be set as a hotkey, except keys that are reserved for the default controls: the left and right arrow keys, enter/return, and backspace. Hotkeys may be used to "speed dial" your favorite applications, or to add controls via [special commands](#special-commands). As an example configuration below, the first hotkey is mapped to F1 and will launch Kodi when it is pressed, and the second hotkey is mapped to F12 and will cause Flex Launcher to quit when it is pressed:
```ini
[Hotkeys]
Hotkey1=#4000003A;"C:\Program Shortcuts\kodi.lnk"
Hotkey2=#40000045;:quit
```

### Exit Hotkey (Windows only)
The exit hotkey feature allows a user to quit the running application using a button on their remote. This is especially useful for applications that don't have a quit button, such as a web browser operating in fullscreen mode.

Only the function keys F1-F24 may be used as an exit hotkey, with the exception of F12 which is forbidden by Windows. Pressing an exit hotkey is functionally equivalent to using the Alt+F4 keyboard shortcut on the active window; it is not a forceful method, so the application is able to close cleanly. However, the application could also choose to ignore it, display a confirmation dialog, or not respond if it's hung.

The following example maps F10 as an exit hotkey:
```ini
Hotkey=#40000043;:exit
```
Linux users that desire similar functionality should check the documentation of their desktop environment and/or window manager. Most support global hotkeys that can be configured to close the active window.

## Gamepad Controls
Flex Launcher has built-in support for gamepad controls through SDL. All settings for gamepads will be in a section titled `Gamepad`. Within the section, there are key=value pairs which define the gamepad settings and the commands to be run when a button or axis is pressed.

### Settings
The following settings are available in the `Gamepad` section to define the behavior of gamepads

#### Enabled
Defines whether or not gamepad controls are enabled. This setting is a boolean "true" or "false".

Default: false

#### DeviceIndex
Defines the device index of the gamepad in SDL. If this value is negative, any gamepad may be used to control the launcher.

Default: -1

#### ControllerMappingsFile
A path to a text file that contains 1 or more controller mappings to override the default. This is usually not necessary, but if you want to change the mapping for your controller, or there is no default mapping for your controller in SDL, it can be specified via this interface. A community database of mappings for many common controllers can be found [here](https://github.com/gabomdq/SDL_GameControllerDB). Alternatively, you may create a custom mapping using a GUI tool such as the [SDL2 Gamepad Tool](https://generalarcade.com/gamepadtool/).

### Controls
The controls are defined in key=value pairs, where the key is the name of the axis or button that is pressed, and the value is the command that is to be run, which is typically a [special command](#special-commands). An axis is an analog stick or a trigger. For analog sticks, negative (-) represents left for the x axis and up for the y axis, and postive (+) represents right for the x axis and down for the y axis. 

The [SDL GameController](https://wiki.libsdl.org/CategoryGameController) interface is an abstraction which conceptualizes a controller as having an Xbox-style layout. The mapping names in SDL are based on the *location* of the buttons on an Xbox controller, and may not correspond to the actual labelling of the buttons on your controller. For example, `ButtonA` is for the "bottom" button, `ButtonB` is for the "right" button of the 4 main control buttons. If you have a Playstation-style controller, those mapping names will correspond to the X button and the Circle button, respectively. 

The default controls in Flex Launcher allow the user to move the highlight cursor left and right by using the left stick or the DPad, select an entry by pressing A, and go back to the previous menu by pressing B. These controls are simple and will suffice for the vast majority of use cases.

The following axis and buttons are available for control in Flex Launcher:
- LStickX-
- LStickX+
- LStickY-
- LStickY+
- RStickX-
- RStickX+
- RStickY-
- RStickY+
- LTrigger
- RTrigger
- ButtonA
- ButtonB
- ButtonX
- ButtonY
- ButtonBack
- ButtonGuide
- ButtonStart
- ButtonLeftStick
- ButtonRightStick
- ButtonLeftShoulder
- ButtonRightShoulder
- ButtonDPadUp
- ButtonDPadDown
- ButtonDPadLeft
- ButtonDPadRight

## Transparent Backgrounds
*Note for Linux users only: this feature requires compositor implementation. See the [Linux Setup Guide](https://complexlogic.github.io/flex-launcher/setup_linux#transparent-backgrounds) for details.*

Flex Launcher supports transparent backgrounds using the chroma key technique. This method works by setting a strategically chosen color to the background, which is removed later. In film production, this technique is often refered to as "blue screening" or "green screening".

The chosen chroma key color should be one that is not found in your icons/text, as this would cause them to become transparent. The color is set with the `ChromaKeyColor` setting in the Background section. The default is `#010101`, a slight off-shade of black. 

A limitation of this method is that your icons and text must be fully opqaue or fully transparent. Any semi-transparent pixels will blend with the chroma key background so that it does not produce a color match, and consequently will not be removed from the background.

Flex Launcher's text rendering is anti-aliased, which gives the text a "feathered" look with semi-transparent pixels on the edges. Normally, this is desirable, but in the case of chroma keying, semi-transparent pixels will cause your chroma key background color to "bleed through" on the edges of the text. If bright blue or bright green is chosen as the chroma key, this will result in a blue or green glowing effect around the text, which is usually undesirable. This is the reason why a dark color is chosen as the default chroma key. The bleed though appears as a dark outline rather than as a bright glowing.

Some icons have shadows which are intended to provide a textured look. The shadows are usually semi-transparent, which will cause them to not render properly with the chroma key technique. You should choose icons without shadows, or manually erase the shadows from an icon if there are no other icons available for the given application. Some icons are also heavily anti-aliased, which can give a glowing or outline effect similar to the text rendering described above.

Another common issue is the highlight. The default highlight is semi-transparent, which will not render properly when blended with the chroma key background. There are a few ways to address with this:
- Set the `FillOpacity` setting to 0%, and use an outline-only highlight instead
- Use custom [Selected Icons](#selected-icon-overrides) in place of Flex Launcher's highlight
- Linux only: use a shader to recover the highlight's transparency. See the [Linux Setup Guide](https://complexlogic.github.io/flex-launcher/setup_linux#transparent-backgrounds) for details.

Transparent backgrounds will require some effort to obtain a setup that looks good and works well. Be prepared to do a significant amount of tinkering if you wish to use this feature.

### Windows Implenetation 
The Windows implementation of transparency is not hardware accelerated. If your refresh rate is very high, this can result in a signficant load on the CPU. If you find that the trasparent background is causing a high load on your system, consider changing the `VSync` setting to false and setting `FPSLimit` to 30 or lower, which will reduce the amount of computation required.

### Animated Backgrounds
Transparent backgrounds can be used to implement animated backgrounds in combination with another program. On Windows, [Wallpaper Engine](https://www.wallpaperengine.io) and [Lively](https://github.com/rocksdanister/lively) are popular choices. For Linux, I recommend [anipaper](https://github.com/Theldus/anipaper); see the [Linux Setup Guide](https://complexlogic.github.io/flex-launcher/setup_linux#animated-backgrounds) for details.

### Custom Widgets
Flex Launcher offers a simple clock widget which can show the current time and date. For more advanced functionality, you can combine a transparent background with a third party widget program. For example, you can have a widget that displays weather, news, etc. in addition to the time. [Rainmeter](https://www.rainmeter.net/) is a popular option on Windows, and [Conky](https://github.com/brndnmtthws/conky) for Linux.
