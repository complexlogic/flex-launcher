---
layout: default
title: Setup Guide
---
# Setup Guide
## Table of Contents
1. [Overview](#overview)
2. [Selecting Menu Icons](#selecting-menu-icons)
3. [Maintaining Controls](#maintaining-contrast)
4. [Launching a Web Browser](#launching-a-web-browser)
5. [Watching YouTube](#watching-youtube)
6. [Directly Launching Steam Games](#directly-launching-steam-games)

## Overview
This page contains tips for setting up Flex Launcher, and HTPCs in general. The recommendations herein are broadly applicable to all platforms supported by Flex Launcher. Additionally, see the platform setup guides for platform-specfic advice:
- [Windows Setup Guide](https://complexlogic.github.io/flex-launcher/setup_windows)
- [Linux Setup Guide](https://complexlogic.github.io/flex-launcher/setup_linux)

Make that you are generally familiar with the [configuration options](https://complexlogic.github.io/flex-launcher/configuration) as well.

## Selecting Menu Icons
Transparency is essential for menu icons. Therefore, you should not use JPEG images for icons, since the JPEG format does not support transparency. Use PNG or WebP instead. PNG icons for most popular applications are easily found online in common sizes up to 256x256.

Any icon that is not the same resolution as the `IconSize` setting in your config file will be stretched. If your `IconSize` setting is not a common icon resolution (e.g. 256), then it is advisable to find SVG icons instead. However, Flex Launcher does not currently support SVGs for menu icons, so you will need to rasterize them into PNG or WebP using a tool such as [Inkscape](https://inkscape.org/). An example command can quickly rasterize an SVG into your desired resolution:
```bash
inkscape --export-width=<width> --export-type=png /path/to/file.svg
```
You can easily write a script to rasterize all SVGs in a directory to PNG at a given resolution. Here is an example in Python:
```python
import glob
import subprocess
WIDTH=300 # Width of the PNG in pixels

svg_files = glob.glob("*.svg")
for file in svg_files:
    subprocess.run(['inkscape', f'--export-width={WIDTH}', '--export-type=png', file])
```

## Maintaining Contrast
When using an image as the background, it is often difficult to read the text that is displayed on top. This is particularly true if the image is a photograph and the text is white. Flex Launcher has several features that will improve the contrast between the background and the objects on top.

The background overlay feature draws a solid color, typically black, over the background. This will darken the background to improve the contrast ratio. The user can adjust how much to darken the background with the `OverlayOpacity` setting.

Text shadows will give displayed text a textured, 3 dimensional appearance, which helps it stand out from the background.

The highlight and scroll indicators each have an outline setting. The user can choose how thick and which color the outline should be, to improve the contrast with the background.

## Launching a Web Browser
My recommended web browser for HTPC use is Chrome/Chromium. This browser has many command line launch options which make it more flexible to configure than Firefox and its derivatives. Some launch options that have particular relevance to HTPC use:
- `--start-fullscreen`: This starts the browser in a fullscreen mode. However, do note that the address bar will be hidden, so make sure to include the URL of the website you want to launch as an argument.
- `--force-device-scale-factor=n`: This can be used to make web pages rendered larger for viewing from a distance. For example, try, 1.1 or 1.2 as `n`.
- `--user-agent`: Sets a custom HTML user-agent string. This is necessary for [watching YouTube](#watching-youtube).

## Watching YouTube
There is currently no desktop application for YouTube. However, there is a TV-friendly web interface located at [youtube.com/tv](https://www.youtube.com/tv) that is intended for use by Smart TVs . Google recently blocked access to this interface for desktop web browsers, but the block can be easily circumvented by spoofing the user-agent string of a Smart TV. A list of valid Smart TV user-agent strings is easily found online by search engine. The following example menu entries will launch an app-like YouTube experience in a browser:

**Windows:**
```ini
Entry=YouTube;C:\icons\youtube.png;"C:\Program Files\Google\Chrome\Application\chrome.exe" --start-fullscreen --user-agent="Mozilla/5.0 (Linux; Tizen 2.3; SmartHub; SMART-TV; SmartTV; U; Maple2012) AppleWebKit/538.1+ (KHTML, like Gecko) TV Safari/538.1+" youtube.com/tv
```

**Linux:**
```ini
Entry=YouTube;/path/to/icons/youtube.png;chromium --start-fullscreen --user-agent="Mozilla/5.0 (Linux; Tizen 2.3; SmartHub; SMART-TV; SmartTV; U; Maple2012) AppleWebKit/538.1+ (KHTML, like Gecko) TV Safari/538.1+" youtube.com/tv
```

This method is far superior to other HTPC YouTube options, such as Kodi's YouTube add-on. You can install a browser extension such as [uBlock Origin](https://ublockorigin.com/) to prevent ads from being shown before videos.

The web interface also supports casting videos from the YouTube app on your smartphone to your TV. You can pair your phone in the settings. You can also sign into your YouTube account in the settings if you wish.

### Exiting
The one caveat to this method is that the exit button in the menu doesn't work. As such, you will need to provide an alternative method to close the web browser after you've finished watching so you can return back to the launcher. For Windows users, the most straightforward solution is to configure an [exit hotkey](https://complexlogic.github.io/flex-launcher/configuration#exit-hotkey-windows-only) on your remote. Linux users should set up a hotkey with their DE/WM to close the active window.

## Directly Launching Steam Games
Steam users may desire to launch their most frequently played games directly from Flex Launcher to avoid having to navigate through the Steam client UI first. Valve provides a [protocol](https://developer.valvesoftware.com/wiki/Steam_browser_protocol) to directly launch games, among other actions. To do so, pass `steam://run/<id>` as an argument to Steam, where `<id>` is replaced by the id of the game you want to watch. You can find the id of a game by searching [steamdb](https://steamdb.info/). For example, the id of Portal 2 is 620. You would structure your menu entry to launch Portal 2 like so:

**Windows:**
```ini
Entry=Portal 2;C:\icons\portal_2.png;"C:\Program Files (x86)\Steam\steam.exe" steam://run/620
```

**Linux:**
```ini
Entry=Portal 2;/path/to/icons/portal_2.png;steam steam://run/620
```

Make sure you have autologin configured in Steam, otherwise you will be prompted for your password before the game launches.
