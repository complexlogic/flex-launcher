---
layout: default
title: Windows Setup Guide
---
# Windows Setup Guide
## Table of Contents
1. [Overview](#overview)
2. [Editing Your Config File](#editing-your-config-file)
3. [Autostarting](#autostarting)
4. [Autologin](#autologin)
5. [Disable Lock Screen](#disable-lock-screen)
6. [Kiosk Mode Setup](#kiosk-mode-setup)

## Overview
This page contains tips for setting up Flex Launcher on Windows-based systems. These instructions are tailored for Windows 10, but should apply to Windows 11 as well.

## Editing Your Config File
I recommend editing your configuration file with a development-oriented text editor such as [Notepad++](https://notepad-plus-plus.org/) instead of the default Windows Notepad program. These text editors have color syntax highlighting for the INI format which makes sections, keys, values, and comments visually distinct.

Several settings require a path as a value. In Windows Explorer, hold shift on your keyboard, then right click on a file or folder and select "Copy as path" from the context menu to copy the file path. You can then paste the path into your configuration file so you don't have to type the path manually.

## Autostarting
In a typical HTPC setup, Flex Launcher should be configured to autostart after login. Right click `flex-launcher.exe` and selct "Create shortcut" in the context menu. Use Windows key + R to bring up the run box, then type `shell:startup` and press enter. This will bring up a Windows Explorer window of your autostart folder. Move the shortcut created in the previous step into this folder, which will cause Flex Launcher to autostart.

Make sure that no other graphical programs are set to autostart. The window creation order of autostarted programs is not easily predictable, so if there is another program that creates a window *after* Flex Launcher, it could cause Flex Launcher to lose the window focus.

## Autologin
Autologin should be configured to prevent users from having to enter a PIN/password with a keyboard after boot. Use Windows key + R to bring up the run box, then type `netplwiz` and press enter. In the resulting window, uncheck the box that says "Users must enter a user name and password to use this computer", then click apply. In the dialog box that pops up, enter the user name that you want to sign in automatically, and the password for it.

## Disable Lock Screen
If you plan to use the `:sleep` special command to put your HTPC to sleep, you will be greeted by the Windows lock screen by default after your HTPC wakes up. This requires you to enter your PIN/password with a keyboard, even if autologin was configured. The lock screen should be disabled so that you return back to Flex Launcher immediately after waking from sleep.

Open the Settings app, then select Accounts, then select Sign-in options. Under the "Require sign-in" section, change the drop down menu value to "Never".

## Kiosk Mode Setup
"Kiosk Mode" typically refers to a user interface which resembles an embedded-style device that performs only a single function (as opposed to a multitasking PC with a desktop interface). This section contains instructions to set up my interpretation of a "Kiosk Mode" interface for a Windows HTPC. Before starting, make sure you have [set up autologin](#autologin) and [disabled your lock screen](#disable-lock-screen) per the previous sections.

In Windows Explorer, navigate to the folder that contains Flex Launcher. Hold shift on your keyboard, then right click on `flex-launcher.exe` and select "Copy as path" from the context menu. Use Windows key + R to bring up the run box, then type `gpedit.msc` and press enter. In the left pane under "User Configuration", select "Administrative Templates", then "System". In the right pane, double click "Custom User Interface". Change the radio button to "Enabled". Then, under "Interface file name", paste the path to `flex-launcher.exe` that was copied in the previous step. Press OK to confirm the changes. This will cause Flex Launcher to replace the Windows desktop as your default graphical user interface. 

Running Flex Launcher in this manner changes the working directory. The default config file that ships with Flex Launcher uses relative paths from the directory containing `flex-launcher.exe`, which will no longer be valid. Therefore, you should convert all relative paths in your config file to absolute paths, e.g. `.\assets\icons\kodi.png` should instead be `C:\path\to\assets\icons\kodi.png`, otherwise images will not load properly.

Reboot your HTPC for the change to take effect.

### Restoring the Desktop
It may be desirable to use the desktop interface occasionally, e.g. during maintenance or installation of new software. You can get your desktop back by running `explorer.exe`. This can be accomplished in a few ways.

Using the `QuitCmd` setting in your config file, you can automatically restore your desktop after you quit Flex Launcher:
```ini
[General]
...
QuitCmd=:fork explorer.exe
```

To manually restore the desktop, hold Ctrl + Shift + Esc to bring up Task Manager. Then, select File->Run new task from the toolbar. Type `explorer.exe` and press OK.

You can permanently switch back to the desktop as your default interface by navigating to Custom User Interface in the Group Policy editor as above, and disabling it.