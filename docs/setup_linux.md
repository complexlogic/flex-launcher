---
layout: default
title: Linux Setup Guide
---
# Linux Setup Guide
## Table of Contents
1. [Overview](#overview)
2. [Autostarting](#autostarting)
3. [Choosing a Distro](#choosing-a-distro)
4. [Desktop Enviornment](#desktop-environment)
5. [Display Protocol](#display-protocol)
6. [Transparent Backgrounds](#transparent-backgrounds)
7. [Kiosk Mode Setup](#kiosk-mode-setup)
8. [HTPC as Audio Receiver](#htpc-as-audio-receiver)
9. [Using an IR Remote](#using-an-ir-remote)

## Overview
This page contains tips for setting up Flex Launcher on Linux-based systems, as well as general HTPC setup tips.

## Autostarting
In a typical HTPC setup, Flex Launcher is autostarted after boot. On Linux, this can be accomplished in multiple ways. The most widely implemented is [XDG Autostart](https://specifications.freedesktop.org/autostart-spec/autostart-spec-latest.html). Application .desktop files in `~/.config/autostart` will be autostarted upon user login. The .desktop file for Flex Launcher is installed to `/usr/share/applications`. Copy it to your autostart directory:
```bash
mkdir -p ~/.config/autostart
cp /usr/share/applications/flex-launcher.desktop ~/.config/autostart
```
Additionally, some desktop enviornments have their own, separate autostart protocol. Consult your DE's documentation for more details.

## Choosing A Distro
My preferred distribution for a Linux HTPC is [Arch](https://archlinux.org/). It has a minimal base installation and very up-to-date packages, which make it well suited for HTPC use.

If you dislike Arch's rolling release model, another good choice is [Debian](https://www.debian.org/). When installing, make sure to deselect a desktop environment so you can have a minimal base install, similar to Arch.

## Desktop Environment
I recommend [Openbox](http://openbox.org/wiki/Main_Page), which is not a full-fledged desktop enviornment, but rather a standalone window manager. An HTPC is a very simple device, and most of the features of desktop environments are not needed and add unnecessary bloat. Since HTPC applications are always in fullscreen, not even compositing is necessary. 

Openbox is lightweight and highly customizable. The basic install of Openbox provides only a black root window, over which Flex Launcher and your desired applications can be drawn.

## Display Protocol
Being based on SDL, Flex Launcher has support for both X11 and Wayland display protocols. I recommend using X11. The benefits that Wayland offers aren't broadly applicable to an HTPC, and Wayland support is still lacking in many areas.

If you choose to run Flex Launcher in Wayland, it is strongly recommended to use it with the latest stable release of SDL, as later versions have seen significantly improved support.

## Transparent Backgrounds
Transparent backgrounds requires compositor support. I recommend [picom](https://github.com/yshui/picom), which supports transparency via GLSL shaders. The method described below requires version 10 or later which, as of this writing, is not yet packaged for most Linux distros. If this is this case for your distro, you will need to build it from source yourself.

The picom option `--window-shader-fg` can be used to specify a custom GLSL shader to apply to the windows. The below shader program can be used as a starting point to implement transparency with Flex Launcher. The macros should be changed to match the values in your Flex Launcher config file, if necessary.

```glsl
#version 330

// Set this to 1 to restore a semi-transparent highlight
#define RESTORE_HIGHLIGHT 1

// Set this to 1 to use a background overlay to darken the video
#define BACKGROUND_OVERLAY 0

#define BACKGROUND_OVERLAY_R 0x00
#define BACKGROUND_OVERLAY_G 0x00
#define BACKGROUND_OVERLAY_B 0x00
#define BACKGROUND_OVERLAY_OPACITY 0.25

// Replace with the values from your Flex Launcher config
#define CHROMA_R 0x01
#define CHROMA_G 0x01
#define CHROMA_B 0x01
#define HIGHLIGHT_R 0xFF
#define HIGHLIGHT_G 0xFF
#define HIGHLIGHT_B 0xFF
#define HIGHLIGHT_OPACITY 0.25

#define BLEND(src, dst) vec4(                \
    src.x * src.w + (dst.x * (1.0 - src.w)), \
    src.y * src.w + (dst.y * (1.0 - src.w)), \
    src.z * src.w + (dst.z * (1.0 - src.w)), \
    src.w + (dst.w * (1 - src.w))            \
)

in vec2 texcoord;
uniform sampler2D tex;
uniform vec4 chroma_key = vec4(float(CHROMA_R) / 255.0, float(CHROMA_G) / 255.0, float(CHROMA_B) / 255.0, 1.0);
uniform vec4 highlight_color = vec4(float(HIGHLIGHT_R) / 255.0, float(HIGHLIGHT_G) / 255.0, float(HIGHLIGHT_B) / 255.0, float(int(HIGHLIGHT_OPACITY * 255.0)) / 255.0);
#if BACKGROUND_OVERLAY
uniform vec4 overlay_color = vec4(float(BACKGROUND_OVERLAY_R) / 255.0, float(BACKGROUND_OVERLAY_G) / 255.0, float(BACKGROUND_OVERLAY_B) / 255.0, float(int(BACKGROUND_OVERLAY_OPACITY * 255.0)) / 255.0);
#endif

vec4 window_shader() 
{
    vec4 c = texelFetch(tex, ivec2(texcoord), 0);

    // Remove background
    vec4 vdiff = abs(chroma_key - c);
    float diff = max(max(max(vdiff.r, vdiff.g), vdiff.b), vdiff.a);
    if (diff < 0.0001)
#if BACKGROUND_OVERLAY
        c = overlay_color;
#else
        c.w = 0.0;
#endif
       
    // Restore highlight
#if RESTORE_HIGHLIGHT
    else {
       vec4 blend = BLEND(highlight_color, chroma_key);
        vdiff = abs(blend - c);
        diff = max(max(max(vdiff.r, vdiff.g), vdiff.b), vdiff.a);
        if (diff < 0.01) {
#if BACKGROUND_OVERLAY
            c = BLEND(highlight_color, overlay_color);
#else
            c = highlight_color;
            c *= c.w;
#endif
        }
    }
#endif

    return c;
}

```
Save the shader program to a .glsl file in a directory of your choice.

Since you will typically not want to run the compositor while your launched applications are running, you will need to start and stop it frequently. This is best accomplished with a systemd service. Create a new user unit file:
```bash
nano /etc/systemd/user/picom-transparent.service
```
Paste the following into the file:
```ini
[Unit]
Description=X11 compositor with alpha transparency

[Service]
ExecStart=/usr/bin/picom --backend glx --force-win-blend --window-shader-fg=/path/to/shader.glsl

[Install]
WantedBy=multi-user.target
```
Replace `</path/to/shader.glsl>` with the appropriate path, then save the file. The compositor can be enabled/disabled with:
```bash
systemctl --user start picom-transparency.service
systemctl --user stop picom-transparency.service
```
Use shell scripts to launch your applications and make sure to stop the compositor prior to launch, and start it again after the application has completed.

### Animated Backgrounds
For an animated transparent background implementation, I recommend [anipaper](https://github.com/Theldus/anipaper), which will play a video of your choice in a loop. anipaper should be run with the following options enabled:
- `-b` (Borderless Fullscreen): Since this method requires a compositor, we will need to run anipaper as a borderless fullscreen window rather than as a wallpaper
- `-p` (Pause): We will need to pause the playback when the application launches, and resume it after it finishes
- `-d` (Hardware decoding): This will keep the CPU use as low as possible, and consequently fan noise and power consumption. Requires the hardware device name as an argument. This option is not absolutely required, but you should use it if your hardware is supported.

In your autostart setup, make sure that anipaper starts *before* Flex Launcher. This ensures that Flex Launcher will have the window focus.

#### Pausing anipaper
When the `-p` option is enabled, anipaper can be paused when applications launch, and resumed when they finish. This is necessary to prevent anipaper from unecessarily consuming resources when the video is not visible. Use the following command in your scripts to pause and resume anipaper
```bash
pkill -SIGUSR1 anipaper
```

#### Example Script
To handle the advanced functionality of starting and stopping various services when applications are launched, you will need to use a shell script. Here is a simple example:
```bash
# Pre-launch
systemctl --user stop picom-transparent.service
pkill -SIGUSR1 anipaper

# Launch application
if [[ "$1" == "kodi" ]]; then
  ...
elif [[ "$1" == "youtube" ]]; then
 ...
fi

# Post-launch
systemctl --user start picom-transparent.service
pkill -SIGUSR1 anipaper
```
The above example script takes an argument which determines which application to launch. The script executes pre and post launch commands which are common to all applications, as well as commands that are specific to the particular application being launched. In your Flex Launcher config, your menu entry command should be the path to the script with the application you want to launch as the first argument.

## Kiosk Mode Setup
"Kiosk Mode" typically refers to a user interface which resembles an embedded-style device that performs only a single function (as opposed to a multitasking PC with a desktop interface). This section contains instructions to set up my interpretation of a "Kiosk Mode" interface for a Linux HTPC. The design is based on my recommendations in the previous sections, consisting of Xorg, Openbox, and Flex Launcher.

### Prerequisites
Before starting I assume that you have one of the following operating systems installed *without* a desktop evironnment (only a console login):
- Arch Linux
- Debian Bookworm or later
- Raspberry Pi OS Lite

I also assume that you can use the Linux command line at an intermediate level or higher.

### First Steps
The first thing to do is configure a network connection and SSH server. On Raspberry Pi, both of those tasks are easily accomplished with the `raspi-config` utility. Otherwise, the exact steps will vary based on which operating system you are on (Arch/Debian) and what backends you choose. Refer to the [Arch Wiki](https://wiki.archlinux.org/) and [Debian Wiki](https://wiki.debian.org/) for more detailed help.

It is recommended to set up a static IP on your LAN. Since you will be doing most of your setup and maintenance remotely via SSH, you will want to have a consistent login.

You will also need to install a terminal-based text editor, since we don't have any graphical interface yet. I recommend nano, and that is what will be used in subsequent sections. You can use a different text editor if you wish, just make sure to substitute commands where appropriate.

### Set Up Autologin
Configure the TTY console to log you in automatically without user or password prompt. On Raspberry Pi, you can use `raspi-config`, or you can also follow the instructions for Arch/Debian below which will also work.

Create a systemd drop-in file for the getty TTY1 service:
```
sudo nano /etc/systemd/system/getty@tty1.service.d/autologin.conf
```
Paste the following into the file:

```ini
[Service]
ExecStart=
ExecStart=-/sbin/agetty --autologin <your_username> --noclear %I $TERM
```
Replace `<your_username>` with your username, then save the file. Reboot your HTPC and verify that it logs you into the console automatically.

### Install Packages
Install the necessary packages.

**Arch:**
```bash
sudo pacman -S xorg xorg-xinit openbox unclutter pulseaudio wget
```
**Debian/Raspberry Pi:**
```bash
sudo apt install xorg openbox unclutter-xfixes pulseaudio wget
```
Then, install Flex Launcher according to the instructions on the [README](https://github.com/complexlogic/flex-launcher#linux). Also, make sure to [copy the assets to your home directory](https://github.com/complexlogic/flex-launcher#copying-assets-to-home-directory).

### Configure Xorg
Configure X to start after user login with `.bash_profile` and `startx`:
```bash
nano ~/.bash_profile
```
Paste the following into the file and save it:
```bash
if [ "x${SSH_TTY}" = "x" ]; then
  startx
fi
```
The `.bash_profile` script will execute every time the user logs in, *including remote logins via SSH*. The purpose of the `if` block is to ensure that `startx` will only be executed during a local login, not remote logins via SSH.

The `startx` program will look for an `xinitrc` script to run: first in the user's home directory, then in the system directory. We will define a basic user `xinitrc` script to configure X and start Openbox:
```bash
nano ~/.xinitrc
```
Paste the following into the file:
```bash
#!/bin/sh
userresources=$HOME/.Xresources
usermodmap=$HOME/.Xmodmap
sysresources=/etc/X11/xinit/.Xresources
sysmodmap=/etc/X11/xinit/.Xmodmap

# merge in defaults and keymaps
if [ -f $sysresources ]; then
    xrdb -merge $sysresources
fi

if [ -f $sysmodmap ]; then
    xmodmap $sysmodmap
fi

if [ -f "$userresources" ]; then
    xrdb -merge "$userresources"
fi

if [ -f "$usermodmap" ]; then
    xmodmap "$usermodmap"
fi

# Startup scripts
if [ -d /etc/X11/xinit/xinitrc.d ] ; then
 for f in /etc/X11/xinit/xinitrc.d/?*.sh ; do
  [ -x "$f" ] && . "$f"
 done
 unset f
fi

unclutter --start-hidden &
exec openbox-session
```
You can make additions to this script if you wish. For example, the screen resolution can be forced using the `xrandr` utility. However, note that the `exec openbox-session` line of the script will never return. Therefore, any additions you make must come *before* this line.

### Configure Openbox
Openbox ships with default configuration files installed to `/etc/xdg/openbox`. These files should be copied to your home directory:
```bash
mkdir -p ~/.config/openbox
cp -a /etc/xdg/openbox/ ~/.config/
```
Among these configuration files is `autostart`, which Openbox will execute after initialization. This is the best way to autostart Flex Launcher:
```bash
nano ~/.config/openbox/autostart
```
Add `flex-launcher` to the file, then save it.

#### Application Menu
You can access a basic application menu by right clicking anywhere on Openbox's root window. The menu entries are populated from `~/.config/openbox/menu.xml`. On Arch, this is a static menu that was pre-populated with programs, most of which you won't have installed. See the [Openbox Wiki](http://openbox.org/wiki/Help:Menus#Static_menus) for editing instructions. On Debian/Raspberry Pi, this is a dynamic menu that is automatically populated from your installed .desktop files, so you shouldn't need to edit anything.

You can also install taskbars and various other graphical interfaces for Openbox, however, this is not recommended. The default interface is clean and the right-click menu provides enough basic functionality for maintenance of your HTPC.

#### Keybinds
[Keybinds](http://openbox.org/wiki/Help:Bindings) can be set in the `rc.xml` file, which maps a keypress to an [Action](http://openbox.org/wiki/Help:Actions). The "Close" action closes the active window, which is very useful for an HTPC. It allows you to quit the current application and return back to the launcher by using a button on your remote. For example, you can use the F10 key to quit the current application like so:
```bash
nano ~/.config/openbox/rc.xml
```
In the `<keyboard>` section, paste the following:
```xml
<keybind key="F10">
  <action name="Close"/>
</keybind>
```
This assumes that you have a key on your TV remote that maps to F10.

### Install Applications
The last step is to install your desired applications. Edit your Flex Launcher configuration file to add menu entries for each of the applications. See the [configuration file documentation](https://complexlogic.github.io/flex-launcher/configuration) for more details.

## HTPC as Audio Receiver
You can use your HTPC as a smart audio receiver for listening to music or podcasts on your living room speakers.

### Bluetooth
If your HTPC has Bluetooth connectivity, you can use it as a receiver and play audio from your smartphone or other device. PulseAudio has support for the A2DP Bluetooth profile via the BlueZ stack. Make sure you have the module installed. It is packaged as `pulseaudio-bluetooth` on Arch and `pulseaudio-module-bluetooth` on Debian/Raspberry Pi. Also, make sure that the systemd bluetooth service is enabled.

You will need to pair your device with your HTPC. This is best done with the `bluetoothctl` utility while you are logged in via SSH. Instructions can be found on the [Arch Wiki](https://wiki.archlinux.org/title/Bluetooth#Pairing) (applicable to all distros, not just Arch).

The pairing only needs to be performed once. Subsequent connections can be initiated from the Bluetooth settings on your mobile device. Make sure to check on your device that audio output is enabled for the connection. Then, you can test playing some audio. PulseAudio should send the audio to your default sink without any additional configuration.

### Spotify Connect
Spotify has a feature called Spotify Connect that allows Premium subscribers to stream audio to another device from the app. [Librespot](https://github.com/librespot-org/librespot) implements Spotify Connect as a Linux daemon. With Librespot, you can play Spotify audio on your HTPC, but control it from your smartphone.

First, install Librespot. I recommend building it from source. The instructions are available on GitHub. 

I manage Librespot with a systemd user service:
```bash
mkdir -p ~/.config/systemd/user
nano ~/.config/systemd/user/librespot.service
```
Paste the following into the file:
```ini
[Unit]
Description=Librespot Spotify Connect daemon
After=network-online.target
Wants=network-online.target
Wants=pulseaudio.service
After=pulseaudio.service

[Service]
ExecStart=/usr/bin/librespot \
            --backend pulseaudio \
            --bitrate 320 \
            --device-type computer \
            --disable-audio-cache \
            --name "HTPC"

[Install]
WantedBy=network-online.target
```
Verify that the path of the `librespot` executable is the same on your system, or change it if necessary. The `--bitrate` controls the bitrate of the streamed audio in kbps. The `--name` controls which name your HTPC will show up as in the Spotify app devices menu. Change them if desired.

If you want Librespot to always run, you can simply enable the service:
```bash
systemctl --user enable librespot
```
If you want Librespot to stop when you launch an application, you will have to use scripts and manually start and stop it:
```bash
systemctl --user start librespot
systemctl --user stop librespot
```
Verify Librespot is running with:
```bash
systemctl --user status librespot
```
Then, try to connect to it with the Spotify app on your phone.

## Using an IR Remote
Some mini PCs have an integrated IR receiver, which can receive signals from a conventional TV remote. The Linux kernel has built-in support for decoding IR signals, which allows you to translate them into keyboard presses. Before starting, make sure the IR receiver is enabled in your motherboard settings.

The IR signal decoding can be controlled with the `ir-keytable` utility. This program is packaged with `v4l-utils` on most distros. First, check to see that your IR receiver is recognized:
```bash
sudo ir-keytable
```
The result should look something like this:
```
Found /sys/class/rc/rc0/ with:
        Name: ITE8708 CIR transceiver
        Driver: ite-cir
        Default keymap: rc-rc6-mce
        Input device: /dev/input/event3
        LIRC device: /dev/lirc0
        Supported kernel protocols: lirc rc-5 rc-5-sz jvc sony nec sanyo mce_kbd rc-6 sharp xmp imon rc-mm 
        Enabled kernel protocols: lirc nec rc-6 
        bus: 25, vendor/product: 1283:0000, version: 0x0000
        Repeat delay = 500 ms, repeat period = 125 ms
```
Take note of the supported kernel protocols. Test the receiving with `-t`:
```bash
ir-keytable -v -t -p irc,rc-5,rc-5-sz,jvc,sony,nec,sanyo,mce_kbd,rc-6,sharp,xmp,imon,rc-mm
```
The protocols specified with `-p` should be comma delimited, and must *not* contain a space. You should update the protocols of the above command based on your output of `sudo ir-keytable` in the previous step.

The `-t` mode will output information for each received signal. Point your remote at the receiver and press various buttons to verify that the signals are being received. Take note of the protocol and the hex values for each button. You will need to create a mapping file that contains the corresponding hex value for each button name. Here is a simple example:
```
#table RCA,  type: rc-6
0x800f741e   KEY_UP
0x800f741f   KEY_DOWN
0x800f7420   KEY_LEFT
0x800f7421   KEY_RIGHT
0x800f7422   KEY_ENTER
0x800f7423   KEY_BACKSPACE
0x800f7425   KEY_ESC
```
The table name is arbitrary, but the `type` must match one of the supported protocols from the previous step. You can find a comprehensive list of valid button names by running:
```bash
irrecord -l
```
Save your mapping file as `/etc/rc_keymaps/default.txt`. Then, test the mapping:
```bash
sudo ir-keytable -c -w /etc/rc_keymaps/default.txt
```
Open a program on your HTPC, press the mapped buttons on your remote, and verify that the proper keys are being triggered on the HTPC.

### Make the Mapping Persistent
The mapping will not persist between reboots. Therefore, you will need to set up a way to run the `ir-keytable` command automatically after boot. One way to do that is with a systemd service.
```bash
sudo nano /etc/systemd/system/remote-setup.service
```
Paste the following into the file:
```ini
[Unit]
Description=Set up IR remotes
After=multi-user.target

[Service]
Type=oneshot
ExecStart=/usr/bin/ir-keytable -c -w /etc/rc_keymaps/default.txt

[Install]
WantedBy=multi-user.target
```
Save the file, then enable the service:
```bash
sudo systemctl enable remote-setup
```
Reboot your HTPC and verify that the remote still works.
