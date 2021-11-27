 # Configuration File Overview
Flex Launcher uses an [INI file](https://en.wikipedia.org/wiki/INI_file) to configure settings and menus. The INI file consists of sections enclosed in brackets, and in each section there are entries which consist of a key and a value. Example:
```
[Section]
Key1=value
Key2=value
...
```
A line can be commented out by using the # character at the beginning of the line, which will cause the line to be ignored by the program. Here are a few things to note about the configuration settings for Flex Launcher:
- All keys and values are case sensitive.
- Color is specified in HEX format, *without* the 0x prefix e.g. the color red in 24 bit RGB should be FF0000. HEX color pickers can be easily found online to assist color choices.
- The color settings that support transparency have a separate opacity setting which allows the user to specify the opacity as a percentage. This is for convenience purposes because many users prefer to specify opacity as a percentage instead of 0-255. The opacity settings may be commented out if they are not desired, in which case the alpha bits will be used to determine the opacity.
- Spaces in paths are acceptable. Do not enclose these paths in quotation marks unless otherwise specified, as this may interfere with the parsing.
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
