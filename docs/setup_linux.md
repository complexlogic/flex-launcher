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
6. [Kiosk Mode Setup](#kiosk-mode-setup)
7. [HTPC as Audio Receiver](#htpc-as-audio-receiver)
8. [Using an IR Remote](#using-an-ir-remote)

## Overview
This page contains tips for setting up Flex Launcher on Linux-based systems, as well as general HTPC setup tips.

## Autostarting
In a typical HTPC setup, Flex Launcher is autostarted after boot. On Linux, this can be accomplished in multiple ways. The most widely implemented is [XDG Autostart](https://specifications.freedesktop.org/autostart-spec/autostart-spec-latest.html). Application .desktop files in `~/.config/autostart` will be autostarted upon user login. The .desktop file for Flex Launcher is installed to `/usr/share/applications`. Copy it to your autostart directory:
```Shell
mkdir -p ~/.config/autostart
cp /usr/share/flex-launcher.desktop ~/.config/autostart
```
Additionally, some desktop enviornments have their own, separate autostart protocol. Consult your DE's documentation for more details.

## Choosing A Distro
My preferred distribution for a Linux HTPC is [Arch](https://archlinux.org/). It has a minimal base installation and very up-to-date packages, which make it well suited for HTPC use.

If you dislike Arch's rolling release model, another good choice is [Debian](https://www.debian.org/). When installing, make sure to deselect a desktop environment so you can have a minimal base install, similar to Arch.

## Desktop Environment
I recommend [Openbox](http://openbox.org/wiki/Main_Page), which is not a full-fledged desktop enviornment, but rather a standalone window manager. An HTPC is a very simple device, and most of the features of desktop environments are not needed and add unnecessary bloat. Since HTPC applications are always in fullscreen, not even compositing is necessary. 

Openbox is lightweight and highly customizable. The basic install of Openbox provides only a black root window, over which Flex Launcher and your desired applications can be drawn.

## Display Protocol
Being based on SDL, Flex Launcher has support for both X11 and Wayland display protocols. I recommend using X11 for now. The benefits that Wayland offers aren't broadly applicable to an HTPC, and Wayland support is still lacking in many areas. GNOME's Mutter seems to be the only well-implemented Wayland compositor, but GNOME is poorly-suited for HTPC use. 

[Labwc](https://github.com/labwc/labwc) looks to be a good Openbox replacement candidate for Wayland, but it needs more development before it is ready for daily driving.

### Wayland in SDL
If your HTPC is running Wayland, and the Wayland compositor has an XWayland connection available, SDL may try to use that over native Wayland. If you are unsure if Flex Launcher is running in native Wayland or XWayland, run it in debug mode, then check the log. Under "Video Information", check the "Video Driver" entry. If it's running under XWayland, the value will be "x11".

To force Flex Launcher to run natively under Wayland, you can use the environment variable `SDL_VIDEODRIVER` like so:
```Shell
SDL_VIDEODRIVER=wayland flex-launcher
```

If you choose to run Flex Launcher in Wayland, it is strongly recommended to use it with the latest stable release of SDL, as later versions have seen significantly improved support.

## Kiosk Mode Setup
"Kiosk Mode" typically refers to a user interface which resembles an embedded-style device that performs only a single function (as opposed to a multitasking PC with a desktop interface). This section contains instructions to set up my interpretation of a "Kiosk Mode" interface for a Linux HTPC. The design is based on my recommendations in the previous sections, consisting of Xorg, Openbox, and Flex Launcher.

### Prerequisites
Before starting I assume that you have one of the following operating systems installed *without* a desktop evironnment (only a console login):
- Arch Linux
- Debian Bullseye or later
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

```INI
[Service]
ExecStart=
ExecStart=-/sbin/agetty --autologin <your_username> --noclear %I $TERM
```
Replace `<your_username>` with your username, then save the file. Reboot your HTPC and verify that it logs you into the console automatically.



### Install Packages
Install the necessary packages.

**Arch:**
```Shell
sudo pacman -S xorg xorg-xinit openbox unclutter pulseaudio wget
```
**Debian/Raspberry Pi:**
```Shell
sudo apt install xorg openbox pulseaudio wget
```
Then, install Flex Launcher according to the instructions on the [README](https://github.com/complexlogic/flex-launcher#linux). Also, make sure to [copy the assets to your home directory](https://github.com/complexlogic/flex-launcher#copying-assets-to-home-directory).

### Configure Xorg
Configure X to start after user login with `.bash_profile` and `startx`:
```Shell
nano ~/.bash_profile
```
Paste the following into the file and save it:
```Shell
if [ "x${SSH_TTY}" = "x" ]; then
  startx
fi
```
The `.bash_profile` script will execute every time the user logs in, *including remote logins via SSH*. The purpose of the `if` block is to ensure that `startx` will only be executed during a local login, not remote logins via SSH.

The `startx` program will look for an `xinitrc` script to run: first in the user's home directory, then in the system directory. We will define a basic user `xinitrc` script to configure X and start Openbox:
```Shell
nano ~/.xinitrc
```
Paste the following into the file:
```Shell
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

#### Unclutter
The `unclutter` program that starts in the second to last line of the `xinitrc` script makes the mouse cursor invisible by default, and it only becomes visible when it is being used. This prevents the user from seeing a mouse cursor when the Openbox root window is visible before Flex Launcher starts, and between applications.

For Arch users, this program was already installed via pacman in a previous step. This program is also in the Debian/Raspbian package repos, but it is an outdated version that doesn't have the `--start-hidden` option. Therefore, I strongly recommend Debian/Raspberry Pi users build it from source rather than using the packaged version:
```Shell
sudo apt install libev-dev libx11-dev libxi-dev asciidoc-base git
git clone https://github.com/Airblader/unclutter-xfixes
cd unclutter-xfixes
make
sudo make install
```

### Configure Openbox
Openbox ships with default configuration files installed to `/etc/xdg/openbox`. These files should be copied to your home directory:
```Shell
mkdir -p ~/.config/openbox
cp -a /etc/xdg/openbox/ ~/.config/
```
Among these configuration files is `autostart`, which Openbox will execute after initialization. This is the best way to autostart Flex Launcher:
```Shell
nano ~/.config/openbox/autostart
```
Add `flex-launcher` to the file, then save it.

#### Application Menu
You can access a basic application menu by right clicking anywhere on Openbox's root window. The menu entries are populated from `~/.config/openbox/menu.xml`. On Arch, this is a static menu that was pre-populated with programs, most of which you won't have installed. See the [Openbox Wiki](http://openbox.org/wiki/Help:Menus#Static_menus) for editing instructions. On Debian/Raspberry Pi, this is a dynamic menu that is automatically populated from your installed .desktop files, so you shouldn't need to edit anything.

You can also install taskbars and various other graphical interfaces for Openbox, however, this is not recommended. The default interface is clean and the right-click menu provides enough basic functionality for maintenance of your HTPC.

#### Keybinds
[Keybinds](http://openbox.org/wiki/Help:Bindings) can be set in the `rc.xml` file, which maps a keypress to an [Action](http://openbox.org/wiki/Help:Actions). The "Close" action closes the active window, which is very useful for an HTPC. It allows you to quit the current application and return back to the launcher by using a button on your remote. For example, you can use the F10 key to quit the current application like so:
```Shell
nano ~/.config/openbox/rc.xml
```
In the `<keyboard>` section, paste the following:
```XML
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
```Shell
mkdir -p ~/.config/systemd/user
nano ~/.config/systemd/user/librespot.service
```
Paste the following into the file:
```INI
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
```Shell
systemctl --user enable librespot
```
If you want Librespot to stop when you launch an application, you will have to use scripts and manually start and stop it:
```Shell
systemctl --user start librespot
systemctl --user stop librespot
```
Verify Librespot is running with:
```Shell
systemctl --user status librespot
```
Then, try to connect to it with the Spotify app on your phone.

## Using an IR Remote
Some mini PCs have an integrated IR receiver, which can receive signals from a conventional TV remote. The Linux kernel has built-in support for decoding IR signals, which allows you to translate them into keyboard presses. Before starting, make sure the IR receiver is enabled in your motherboard settings.

The IR signal decoding can be controlled with the `ir-keytable` utility. This program is packaged with `v4l-utils` on most distros. First, check to see that your IR receiver is recognized:
```Shell
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
```Shell
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
```Shell
irrecord -l
```
Save your mapping file as `/etc/rc_keymaps/default.txt`. Then, test the mapping:
```Shell
sudo ir-keytable -c -w /etc/rc_keymaps/default.txt
```
Open a program on your HTPC, press the mapped buttons on your remote, and verify that the proper keys are being triggered on the HTPC.

### Make the Mapping Persistent
The mapping will not persist between reboots. Therefore, you will need to set up a way to run the `ir-keytable` command automatically after boot. One way to do that is with a systemd service.
```Shell
sudo nano /etc/systemd/system/remote-setup.service
```
Paste the following into the file:
```INI
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
```Shell
sudo systemctl enable remote-setup
```
Reboot your HTPC and verify that the remote still works.
