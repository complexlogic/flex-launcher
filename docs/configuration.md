---
layout: default
title: Configuration
---
# Configuring Flex Launcher
## Table of Contents

1. [Overview](#overview)
2. [Settings](#settings)
3. [Creating Menus](#creating-menus)
    - [Special Commands](#special-commands)
    - [Desktop Files (Linux Only)](#desktop-files-linux-only)
4. [Clock](#clock)
5. [Screensaver](#screensaver)
6. [Hotkeys](#hotkeys)
7. [Gamepad Controls](#gamepad-controls)

## Overview
Flex Launcher uses an [INI file](https://en.wikipedia.org/wiki/INI_file) to configure settings and menus. The INI file consists of sections enclosed in square brackets, and in each section there are entries which consist of a key and a value. Example:
```
[Section]
Key1=value
Key2=value
...
```
A line can be commented out by using the # character at the beginning of the line, which will cause the line to be ignored by the program. Here are a few things to note about the configuration settings for Flex Launcher:
- All keys and values are case sensitive.
- Full UTF-8 character set is supported for titles.
- The following image formats are supported: JPEG, PNG, and WebP
- Relative paths are evaluated with respect to the *current working directory*, which may not be the same as the directory that the config file is located in. It is recommended to use absolute paths whenever possible to eliminate any confusion.
- Color is specified in 24 bit RGB HEX format, *without* a 0x or # prefix e.g. the color red should be FF0000. HEX color pickers can be easily found online to assist color choices.
- Several settings allow for values to be specified in pixels *or* as a percentage of another value. In this case, if no percent sign is detected it will be interpreted as pixels, and if the percent sign is present, than it will be interpreted as a percent value e.g. "5" means 5 pixels and "5%" means 5 percent.

## Settings
Every config file must have a section titled "Settings". Within this section, the following keys may be used to control the behavior of Flex Launcher:
- [DefaultMenu](#defaultmenu)
- [MaxButtons](#maxbuttons)
- [BackgroundMode](#backgroundmode)
- [BackgroundColor](#backgroundcolor)
- [BackgroundImage](#backgroundimage)
- [SlideshowDirectory](#slideshowdirectory)
- [SlideshowImageDuration](#slideshowimageduration)
- [SlideshowTransitionTime](#slideshowtransitiontime)
- [BackgroundOverlay](#backgroundoverlay)
- [BackgroundOverlayColor](#backgroundoverlaycolor)
- [BackgroundOverlayOpacity](#backgroundoverlayopacity)
- [IconSize](#iconsize)
- [IconSpacing](#iconspacing)
- [TitleFont](#titlefont)
- [TitleFontSize](#titlefontsize)
- [TitleColor](#titlecolor)
- [TitleShadows](#titleshadows)
- [TitleShadowColor](#titleshadowcolor)
- [TitleOpacity](#titleopacity)
- [TilteOversizeMode](#titleoversizemode)
- [TitlePadding](#titlepadding)
- [HighlightFillColor](#highlightfillcolor)
- [HighlightFillOpacity](#highlightfillopacity)
- [HighlightOutlineSize](#highlightoutlinesize)
- [HighlightOutlineColor](#highlightoutlinecolor)
- [HighlightOutlineOpacity](#highlightoutlineopacity)
- [HighlightCornerRadius](#highlightcornerradius)
- [HighlightVPadding](#highlightvpadding)
- [HighlightHPadding](#highlighthpadding)
- [ButtonCenterline](#buttoncenterline)
- [ScrollIndicators](#scrollindicators)
- [ScrollIndicatorFillColor](#scrollindicatorfillcolor)
- [ScrollIndicatorOutlineSize](#scrollindicatoroutlinesize)
- [ScrollIndicatorOutlineColor](#scrollindicatoroutlinecolor)
- [ScrollIndicatorOpacity](#scrollindicatoropacity)
- [OnLaunch](#onlaunch)
- [ResetOnBack](#resetonback)
- [MouseSelect](#mouseselect)

#### DefaultMenu
This is the title of the main menu that shows when Flex Launcher is started. The value *must* match the name of one of your menu sections, or there will be an error and Flex Launcher will refuse to start. See the [Creating Menus](#creating-menus) section for more information.

#### MaxButtons
The maximum number of buttons that can be displayed on the screen. If a menu has more entries than this value, it will be split into multiple pages. A value of 3-5 is sensible for a typical TV size and viewing distance.

Default: 4

#### BackgroundMode
Defines what mode the background will be. Possible values: "Color", "Image", and "Slideshow"
- Color: The background will be a solid color.
- Image: The background will be an image.
- Slideshow: The background will be a series of images displayed in random order, with a fading transition between each image.

Default: Color

#### BackgroundColor
When ```BackgroundMode``` is set to "Color", this setting defines the color of the background.

Default: 000000 (Black)

#### BackgroundImage
When ```BackgroundMode``` is set to "Image", this setting defines the image to be displayed in the background. The value should be a path to an image file. If the image is not the same resolution as your desktop, it will be stretched accordingly.

#### SlideshowDirectory
When ```BackgroundMode``` is set to "Slideshow", this setting defines the directory (folder) which contains the images to display in the background. The value should be a path to a directory on your filesystem. The number of images that may be scanned from the directory is limited to 250.

#### SlideshowImageDuration
When ```BackgroundMode``` is set to "Slideshow", this setting defines the amount of time in seconds to display each image. Must be an integer value.

Default: 30

#### SlideshowTransitionTime
When ```BackgroundMode``` is set to "Slideshow", this setting defines the amount of time in seconds that the next background image will fade in. The fading transition may be disabled by setting this to 0, which will yield a "hard" transition between images. Decimal values are acceptable.

Default: 1.5

#### BackgroundOverlay
Defines whether the background overlay feature is enabled. The background overlay is a solid color that is painted over your background, typically used improve the contrast between the background and the text/icons. This setting is a boolean "true" or "false".

Default: false

#### BackgroundOverlayColor
Defines the color of the background overlay.

Default: 000000 (Black)

#### BackgroundOverlayOpacity
Defines the opacity of the background overlay. Must be a percent value.

Default: 50%

#### IconSize
The width and height of icons on the screen in pixels. If an icon is not the same resolution, it will be stretched accordingly.

Default: 256

#### IconSpacing
Distance between the menu entry icons, in pixels or percent of the screen width.

Default: 5%

#### TitleFont
Defines the font to use for the titles of the menu entries. The value should be the path to a TrueType (TTF) font file. Non-TTF font formats are not supported. Flex Launcher ships with a handful of libre fonts.

Default: OpenSans

#### TitleFontSize
Defines the font size of each menu entry title.

Default: 36

#### TitleColor
Defines the color of the menu entry titles.

Default: FFFFFF (White)

#### TitleShadows
Defines whether shadows are enabled for the menu titles. Shadows give a 3D textured appearance to the text to improve the contrast from the background. This setting is a boolean "true" or "false".

Default: false

#### TitleShadowColor
Defines the color of the title shadows.

Default: 000000 (Black)

#### TitleOpacity
Defines the opacity of the menu entry titles. Must be a percent value.

Default: 100%

#### TitleOversizeMode
Defines the behavior when the width of a menu entry title exceeds the width of its icon (which is defined in ```IconSize```). Possible values: "Truncate", "Shrink", and "None"
- Truncate: Truncates the title at the maximum width and adds "..." to the end.
- Shrink: Shrinks oversized titles to a smaller font size than ```TitleFontSize``` so that the entire title fits within the maximum width.
- None: No action is taken to limit the width of titles. Overlaps with other titles may occur, and it is the user's responsibility to manually handle any such case.

Default: Truncate

#### TitlePadding
Defines the vertical spacing between an icon and its title, in pixels.

Default: 20

#### HighlightFillColor
Defines the fill color of the highlight cursor.

Default: FFFFFF (White)

#### HighlightFillOpacity
Defines the fill opacity of the highlight cursor. Must be a percent value.

Default: 25%

#### HighlightOutlineSize
Defines the stroke width in pixels of the outline of the highlight cursor. Setting this to 0 will disable the outline.

Default: 0

#### HighlightOutlineColor
Defines the outline color of the highlight cursor.

Default: 0000FF (Blue)

#### HighlightOutlineOpacity
Defines the outline opacity of the highlight cursor. Must be a percent value.

Default: 100%

#### HighlightCornerRadius
Defines the corner radius of the highlight cursor, in pixels. A value of 0 will yield a plain rectangle. Increasing the value will yield a rounded rectangle with increasingly round corners. The value of ```HighlightOutlineSize``` must be 0, otherwise this setting will be ignored.

Default: 0

#### HighlightVPadding
Defines the amount of vertical distance that the highlight cursor extends beyond the top and bottom of the menu entry icon, in pixels.

Default: 30

#### HighlightHPadding
Defines the amount of horizontal distance that the highlight cursor extends beyond the left and right of the menu entry icon, in pixels.

Default: 30

#### ButtonCenterline
Defines the vertical centering of the menu entries in percent of the screen height. A value of 50% will cause the buttons to be centered halfway in the screen. Increasing the value will lower the buttons, and lowering it will raise them.

Default: 50%

#### ScrollIndicators
Defines whether scroll indicators will be enabled in the event that a menu has multiple pages of entries. The scroll indicators are arrows which appear in the bottom left or bottom right of the screen to inform the user that the are other pages of buttons to scroll to. This setting is a boolean "true" or "false".

Default: true

#### ScrollIndicatorFillColor
Defines the fill color of the scroll indicators.

Default: FFFFFF (White)

#### ScrollIndicatorOutlineSize
Defines the stroke width in pixels of the scroll indicator outline. Setting this to 0 will disable the outline.

Default: 0

#### ScrollIndicatorOutlineColor
Defines the color of the scroll indicator outline.

Default: 000000 (Black)

#### ScrollIndicatorOpacity
Defines the opacity of the scroll indicators. Must be a percent value.

Default: 100%

#### OnLaunch
Defines the action that Flex Launcher will take upon the launch of an application. Possible values: "Hide", "None", and "Blank"
- Hide: Flex Launcher will hide its window while the application is running, and then show itself again after the application has closed. This avoids window focus conflicts between the launcher and the application, but will cause the desktop to be briefly visible when an application is launched.
- None: Flex Launcher will maintain its window, and the launched application will have to draw itself over Flex Launcher. The desktop will never be visible, but this mode could cause window focusing conflicts depending on your configuration. This setting should only be used if all your applications launch in a fullscreen mode.
- Blank: Same as "None", except Flex Launcher will change to a blank, black screen.

Default: Hide

#### ResetOnBack
Defines whether Flex Launcher will remember the previous entry position when going back to a previous menu. If set to true, the highlight will be reset to the first entry in the menu when going back. This setting is a boolean "true" or "false".

Default: false

#### MouseSelect
Defines whether the left mouse button can be used to select the highlighted entry. This setting is intended to support gyroscopic mouse devices where the enter/ok button functions as a mouse left click instead of the keyboard enter button.

Default: false

## Creating Menus
At least one menu must be defined in the configuration file, and the title must match the ```DefaultMenu``` setting value. The title of a menu is its section name. Any title may be used that is not reserved for another section, such as "Settings", "Gamepad", etc. The entries of the menu are implemented as key=value pairs. The name of the key will be ignored by the program, and is therefore arbtrary. However, it is recommended to pick something intutitive such as Entry1, Entry2, Entry3, etc. The entry information is contained in the value.

Each entry value contains 3 parts of information in order: the title, the icon image path, and the command to run when the button is clicked. These are delimited by semicolons:
```
Entry=title;icon_path;command
```
The command is typically one of the following:
1. The path to the program executable that you want to launch 
2. Windows: the path to a program shortcut (.lnk file)
3. Linux: the path to a [.desktop file](#desktop-files-linux-only)
4. A [special command](#special-commands)
5. The path to an executable script, in the case that you want to perform multiple actions upon program launch.

 A simple example menu titled ```Media``` is shown below:
```
[Media]
Entry1=Kodi;C:\Pictures\Icons\kodi.png;"C:\Program Shortcuts\kodi.lnk"
Entry2=Netflix;C:\Pictures\Icons\netflix.png;"C:\Program Shortcuts\netflix.lnk"
Entry3=Plex;C:\Pictures\Icons\plex.png;"C:\Program Shortcuts\plex.lnk"
Entry4=Back;C:\Pictures\Icons\back.png;:back
```

### Special Commands
Special commands are commands that are internal to Flex Launcher and begin with a colon. The following is a list of special commands:

#### :submenu
Change to a different menu. Requires a menu title as an argument. For example, the command ```:submenu Games``` will change to the menu ```Games```. The argument must be a valid menu title that is defined elsewhere in the config file.

#### :fork
Forks a new process and executes a command in it without exiting the launcher. This is typically used in combination with a [hotkey](#hotkeys). Use this special command when you want to execute a command on your system for some reason other than launching a graphical application. Example use cases:
- Change a Wi-Fi connection
- Pair or connect a Bluetooth device
- Start or stop some system service/daemon

The :fork special command requires a command as an argument. For example ```:fork command arguments``` will execute ```command arguments``` without leaving the launcher.

Windows users should invoke a command line interpreter such as Command Prompt and pass the command to run as an argument, e.g. ```:fork cmd.exe /c "command arguments"```

#### :exit
Windows only. Quits the currently running application. This special command is only available as a hotkey command. See the [Exit Hotkey](#exit-hotkey-windows-only) section for more information.

#### :back
Go back to the previous menu.

#### :home
Change to the menu defined in the ```DefaultMenu``` setting.

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

Default: FFFFFF (White)

#### Shadows
Defines whether shadows are enabled for the clock text. Shadows give a 3D textured appearance to the text to improve the contrast from the background. This setting is a boolean "true" or "false".

Default: false

#### ShadowColor
Defines the color of the clock text shadows.

Default: 000000 (Black)

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
When ```BackgroundMode``` is set to "Slideshow", this setting defines whether or not the slideshow should be paused while the screensaver is active. This setting is a boolean "true" or "false".

Default: true

## Hotkeys
Flex Launcher supports configurable hotkeys, which executes a command when a specified key is pressed. Each hotkey consists of a key=value pair, where the key is an arbitrary name, and the value contains the SDL keycode of the hotkey and the command to run when it is pressed, delimited by a semicolon:
```
Hotkey=keycode;command
```
The keycode is a HEX value *without* any 0x or # prefix. There are two ways to find a keycode for a given key. The first is to use the [lookup table provided by SDL](https://wiki.libsdl.org/SDLKeycodeLookup). The name of each key is in the right column of the table, and the corresponding HEX keycode is in the center column. The second is to [run Flex Launcher in debug mode](../../README.md#debugging), press the key, then check the log. For each keystroke, the name of the key will be printed and the HEX value will be in parenthesis next to it.

Any key can be set as a hotkey, except keys that are reserved for the default controls: the left and right arrow keys, enter/return, and backspace. Hotkeys may be used to "speed dial" your favorite applications, or to add controls via [special commands](#special-commands). As an example configuration below, the first hotkey is mapped to F1 and will launch Kodi when it is pressed, and the second hotkey is mapped to F12 and will cause Flex Launcher to quit when it is pressed:
```
[Hotkeys]
Hotkey1=4000003A;"C:\Program Shortcuts\kodi.lnk"
Hotkey2=40000045;:quit
```

### Exit Hotkey (Windows only)
The exit hotkey feature allows a user to quit the running application using a button on their remote. This is especially useful for applications that don't have a quit button, such as a web browser operating in fullscreen mode.

Only the function keys F1-F24 may be used as an exit hotkey, with the exception of F12 which is forbidden by Windows. Pressing an exit hotkey is functionally equivalent to using the Alt+F4 keyboard shortcut on the active window; it is not a forceful method, so the application is able to close cleanly. However, the application could also choose to ignore it, display a confirmation dialog, or not respond if it's hung.

The following example maps F10 as an exit hotkey:
```
Hotkey=40000043;:exit
```
Linux users that desire similar functionality should check the documentation of their desktop environment and/or window manager. Most support global hotkeys that can be configured to close the active window.

## Gamepad Controls
Flex Launcher has built-in support for gamepad controls through SDL. All settings for gamepads will be in a section titled ```Gamepad```. Within the section, there are key=value pairs which define the gamepad settings and the commands to be run when a button or axis is pressed.

### Settings
The following settings are available in the ```Gamepad``` section to define the behavior of gamepads

#### Enabled
Defines whether or not gamepad controls are enabled. This setting is a boolean "true" or "false".

Default: false

#### DeviceIndex
Defines the device index of the gamepad in SDL. In most cases this is not needed and should be left commented out.

Default: 0

#### ControllerMappingsFile
A path to a text file that contains 1 or more controller mappings to override the default. This is usually not necessary, but if you want to change the mapping for your controller, or there is no default mapping for your controller in SDL, it can be specified via this interface. A community database of mappings for many common controllers can be found [here](https://github.com/gabomdq/SDL_GameControllerDB). Alternatively, you may create a custom mapping using a GUI tool such as the [SDL2 Gamepad Tool](https://generalarcade.com/gamepadtool/).

### Controls
The controls are defined in key=value pairs, where the key is the name of the axis or button that is pressed, and the value is the command that is to be run, which is typically a [special command](#special-commands). An axis is an analog stick or a trigger. For analog sticks, negative (-) represents left for the x axis and up for the y axis, and postive (+) represents right for the x axis and down for the y axis. 

The [SDL GameController](https://wiki.libsdl.org/CategoryGameController) interface is an abstraction which conceptualizes a controller as having an Xbox-style layout. The mapping names in SDL are based on the *location* of the buttons on an Xbox controller, and may not correspond to the actual labelling of the buttons on your controller. For example, ```ButtonA``` is for the "bottom" button, ```ButtonB``` is for the "right" button of the 4 main control buttons. If you have a Playstation-style controller, those mapping names will correspond to the X button and the Circle button, respectively. 

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
