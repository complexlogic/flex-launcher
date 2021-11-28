 # Compiling Flex Launcher
 Flex Launcher builds natively on Linux and Windows, and features a cross-platform cmake build system. The following external dependencies are required:
 - SDL
 - SDL_image
 - SDL_ttf

## Linux
Flex Launcher on Linux builds with GCC. This guide assumes you already have git, cmake, and GCC installed on your system for building. If not, consult your distro's documentation. 

First, install the dependencies. The steps to do so are dependent on your distro:

### APT-based Distributions (Debian, Ubuntu, Mint, etc.)
```
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev
```

### Pacman-based Distributions (Arch, Manjaro, etc.)
```
sudo pacman -S sdl2 sdl2_image sdl2_ttf
```

### Building
Clone the master repo and create a build directory
```
git clone https://github.com/complexlogic/flex-launcher.git
cd flex-launcher
mkdir build && cd build
```
Generate the Makefile
```
cmake -Wno-dev .. 
```
Build and test the program
```
make
./flex-launcher
```
Optionally, install it into your system directories
```
sudo make install
```
By default, this will install the program and assets with a prefix of /usr/local. If you wish to use a different prefix, re-run the cmake generation step with the -DCMAKE_INSTALL_PREFIX flag.

## Windows
Flex Launcher on Windows builds with Visual Studio, and uses [vcpkg](https://vcpkg.io/en/index.html) to manage the dependencies.
