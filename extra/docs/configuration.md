 # Configuration File Overview
Flex Launcher uses an [INI file](https://en.wikipedia.org/wiki/INI_file) to configure settings and menus. The INI file consists of sections enclosed in square brackets, and in each section there are entries which consist of a key and a value. Example:
```
[Section]
Key1=value
Key2=value
...
```
A line can be commented out by using the # character at the beginning of the line, which will cause the line to be ignored by the program. Here are a few things to note about the configuration settings for Flex Launcher:
- All keys and values are case sensitive.
- Full UTF-8 text encoding is supported for titles.
- Relative paths are evaluated with respect to the *current working directory*, which may not be the same as the directory that the config file is located in. It is recommended to use absolute paths whenever possible to eliminate any confusion.
- Spaces in paths are acceptable. Do not enclose these paths in quotation marks unless otherwise specified, as this may interfere with the parsing.
- Color is specified in HEX format, *without* the 0x prefix e.g. the color red in 24 bit RGB should be FF0000. HEX color pickers can be easily found online to assist color choices.
- The color settings that support transparency have a separate opacity setting which allows the user to specify the opacity as a percentage. This is for convenience purposes because many users prefer to specify opacity as a percentage instead of 0-255. The opacity settings may be commented out if they are not desired, in which case the alpha bits will be used to determine the opacity.
- Several settings allow for values to be specified in pixels *or* as a percentage of another value. In this case, if no percent sign is detected it will be interpreted as pixels, and if the percent sign is present, than it will be interpreted as a percent value e.g. "5" means 5 pixels and "5%" means 5 percent.
## Settings
Every config file must have a section titled "Settings". Within this section, the following keys may be used to control the behavior of Flex Launcher

#### DefaultMenu
This is the title of the main menu that shows when Flex Launcher is started. The value *must* match the name of one of your menu sections, or there will be an error and Flex Launcher will refuse to start. See the creating menus section for more information

#### MaxButtons
The maximum number of buttons that can be displayed on the screen. If your menu has more entries than this value, they will be split into multiple pages. A value of 3-5 is sensible for a typical TV size and viewing distance.

Default: 4
#### BackgroundMode
Defines what mode the background will be. Possible values: "Color" and "Image"
- Color: The background will be a solid color.
- Image: The background will be an image.

Default: Color
#### BackgroundColor
When ```BackgroundMode``` is set to "Color", this setting defines the color of the background. Specified in 24 bit RGB HEX format.

Default: 000000 (Black)

#### BackgroundImage
When ```BackgroundMode``` is set to "Image", this setting defines the image to be displayed in the background. The value should be a path to a JPEG or PNG image. If the image is not the same resolution as your desktop, it will be stretched accordingly.

#### IconSize
The width and height of icons on the screen in pixels. If an icon is not the same resolution, it will be stretched accordingly.

Default: 256

#### IconSpacing
Distance between the menu entry icons, in pixels or percent of the screen width

Default: 5%

#### TitleFont
Defines the font to use for the titles of the menu entries. The value should be the path to a TrueType (TTF) font file. Non-TTF font formats are not supported. Flex Launcher ships with a handful of libre fonts.

Default: OpenSans

#### TitleFontSize
Defines the font size of each menu entry title.

Default: 36

#### TitleColor
Defines the color of the menu entry titles. Specified in 32 bit RGBA HEX format.

Default: FFFFFFFF (Opaque White)

#### TitleOpacity
When present, this setting overrides the alpha bits of ```TitleColor```. Must be a percent value.

#### TitleOversizeMode
Defines the behavior when the width of a menu entry title exceeds the width of its icon (which is defined in ```IconSize```). Possible values: "Truncate", "Shrink", and "None"
- Truncate: Truncates the title at the maximum width and adds "..." to the end
- Shrink: Shrinks oversized titles to a smaller font size than ```TitleFontSize``` so that the entire title fits within the maximum width
- None: No action is taken to limit the width of titles. Overlaps with other titles may occur, and it is the user's responsibility to manually handle any such case

Default: Truncate

#### TitlePadding
Defines the space between the icon and its title, in pixels

Default: 20

#### HighlightColor
Defines the color of the highlight cursor. Specified in 32 bit RGBA HEX format.

Default: FFFFFF40 (White with 25% Opacity)

#### HighlightOpacity
When present, this setting overrides the alpha bits of ```HighlightColor```. Must be a percent value.

#### HighlightCornerRadius
Defines the corner radius of the highlight cursor, in pixels. A value of 0 will yield a plain rectangle. Increasing the value will yield a rounded rectangle with increasingly round corners.

Default: 0

#### HighlightVPadding
Defines the amount of distance that the highlight cursor extends beyond the top and bottom of the menu entry icon, in pixels.

Default: 30

#### HighlightHPaddding
Defines the amount of distance that the highlight cursor extends beyond the left and right of the menu entry icon, in pixels.

Default: 30

#### ButtonCenterline
Defines the vertical centering of the menu entries in percent of the screen height. A value of 50% will cause the buttons to be centered halfway in the screen. Increasing the value will lower the buttons, and lowering it will raise them.

Default: 50%

#### ScrollIndicators
Defines whether scroll indicators will be enabled in the event that a menu has multiple pages of entries. The scroll indicators are arrows which appear in the bottom left or bottom right of the screen to inform the user that the are other pages of buttons to scroll to. This setting is a boolean "true" or "false".

Default: true

#### ScrollIndicatorColor
Defines the color of the scroll indicators. Specified in 32 bit RGBA.

Default: FFFFFFFF (Opaque White)

#### ScrollIndicatorOpacity
When present, this setting overrides the alpha bits of ```ScrollIndicatorColor```. Must be a percent value.

#### ResetOnBack
Defines whether Flex Launcher will remember the previous entry position when going back to a previous menu. If set to true, the highlight will be reset to the first entry in the menu when going back. This setting is a boolean "true" or "false".

Default: false

#### EscQuit
Defines whether the user can exit Flex Launcher by pressing the Esc key. When set to false, it prevents a regular user from exiting to the desktop, so Flex Launcher will operate in a "Kiosk Mode" of sorts. This setting is a boolean "true" or "false".

Default: true

## Creating Menus
At least one menu must be defined in the configuration file, and the title must match the ```DefaultMenu``` setting value. The title of the menu is the section name. Any title may be used that is not reserved for another section, such as "Settings", and "Gamepad". The entries of the menu are implemented as key=value pairs. The name of the key will be ignored by the program, and is therefore arbtrary. However, it is recommended to pick something intutitive such as Entry1, Entry2, Entry3, etc. The entry information is contained in the value.

Each entry value contains 3 parts of information in order: the title, the icon image path, and the command to run when the button is clicked. These are delimited by semicolons. The command is typically the path to the program executable that you want to launch, or a special command. Windows users may also use a path to a shortcut to the program (.lnk file). A simple example menu titled ```Media``` is shown below:
```
[Media]
Entry1=Kodi;C:\Pictures\Icons\kodi.png;C:\Program Shortcuts\kodi.lnk
Entry2=Netflix;C:\Pictures\Icons\netflix.png;C:\Program Shortcuts\netflix.lnk
Entry3=Plex;C:\Pictures\Icons\plex.png;C:\Program Shortcuts\plex.lnk
```

### Special Commands
Special commands are commands that are internal to Flex Launcher and begin with a colon. Here is a list of special commands:

#### :submenu
Change to a different menu. Requires a menu title as an argument. For example, the command ```:submenu Games``` will change to the menu ```Games```. The argument be a valid menu title that is defined elsewhere in the config file.

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
Press enter on the current selection. This special command is only available as a gamepad command, it is forbidden for menu entries.

#### :shutdown
Shut down the computer. For Linux: only works in systemd distros. Non-systemd distro users need to implement the command manually for their init system

#### :restart
Restart the computer. For Linux: only works in systemd distros. Non-systemd distro users need to implement the command manually for their init system

#### :sleep
Put the computer to sleep. For Linux: only works in systemd distros. Non-systemd distro users need to implement the command manually for their init system

### Desktop Files (Linux Only)
If the application you want to launch was installed via your distro's package manager, a .desktop file was most likely provided. The command to launch a Linux application can simply be a link to the .desktop file, and Flex Launcher will run the Exec command that the developers have specified in the file. Desktop files are located in /usr/share/applications.

#### Desktop Actions
Some .desktop files contain "Actions", which affect how the program is launched. An action may be specified by delimiting it from the path to the .desktop file with the pipe character |. For example, Steam has a mode called "Big Picture Mode", which provides an interface similar to a game console and is ideal for a living room PC. The action in the .desktop file is called "BigPicture". A sample menu entry to launch Steam in Big Picture mode is shown below:
```
Entry1=Steam;/path/to/steamicon.png;/usr/share/applications/steam.desktop|BigPicture
```

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
The controls are defined in key=value pairs, where the key is the name of the axis or button that is pressed, and the value is the command that is to be run, which is typically a special command. An axis is an analog stick or a trigger. For analog sticks, negative (-) represents left for the x axis and up for the y axis, and postive (+) represents right for the x axis and down for the y axis. 

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
