---
layout: default
title: Compilation Guide
---
# Compilation Guide
## Table of Contents
1. [Overview](#overview)
2. [Linux](#linux)
3. [Windows](#windows)

## Overview
 Flex Launcher builds natively on Linux and Windows, and features a cross-platform CMake build system. The following external dependencies are required:
 - SDL ≥ 2.0.14
 - SDL_image ≥ 2.0.5
 - SDL_ttf ≥ 2.0.15

## Linux
Flex Launcher on Linux builds with GCC. This guide assumes you already have the development tools Git, CMake, pkg-config, and GCC installed on your system. If not, consult your distro's documentation. 

First, install the dependencies. The steps to do so are dependent on your distro:

#### APT-based Distributions (Debian, Ubuntu, Mint, Raspberry Pi OS etc.)
```bash
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libinih-dev
```

#### Pacman-based Distributions (Arch, Manjaro, etc.)
```bash
sudo pacman -S sdl2 sdl2_image sdl2_ttf libinih
```

#### DNF-based Distributions (Fedora)
```bash
sudo dnf install SDL2-devel SDL2_image-devel SDL2_ttf-devel inih-devel
```

### Building
Clone the master repo and create a build directory:
```bash
git clone https://github.com/complexlogic/flex-launcher.git
cd flex-launcher
mkdir build && cd build
```
Generate the Makefile:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```
If you're building on Raspberry Pi, it's recommended to pass `-DRPI=1` to cmake, which tweaks the default configuration to be more Pi-centric.

Build and test the program:
```bash
make
./flex-launcher
```
Optionally, install it into your system directories:
```bash
sudo make install
```
By default, this will install the program and assets with a prefix of `/usr/local`. If you wish to use a different prefix, re-run the cmake generation step with `-DCMAKE_INSTALL_PREFIX=prefix`.

## Windows
Flex Launcher on Windows builds with Visual Studio, and uses [vcpkg](https://vcpkg.io/en/index.html) to manage the dependencies. Before starting, make sure the following steps are completed:
- Visual Studio is installed. The free Community Edition is available for download from Microsoft's website. The following tools and features for Visual Studio are required:
  - C++ core desktop features
  - Latest MSVC
  - Latest Windows SDK
  - C++ CMake tools
- Git is installed and in your `Path` environment variable
- CMake is installed and in your `Path` environment variable

### Building
Clone the master repo and create a build directory:
```
git clone https://github.com/complexlogic/flex-launcher.git
cd flex-launcher
mkdir build
cd build
```
Build the dependencies and generate the Visual Studio project files:
```
git clone https://github.com/microsoft/vcpkg
cmake .. -DCMAKE_TOOLCHAIN_FILE=".\vcpkg\scripts\buildsystems\vcpkg.cmake" -DVCPKG_TARGET_TRIPLET="x64-windows-static"
```
Build and test the program:
```
cmake --build .
.\Debug\flex-launcher.exe
```
Optionally, generate a clean zipped install package which may then be extracted to a directory of your choosing:
```
cmake --build . --config Release --target package
```
