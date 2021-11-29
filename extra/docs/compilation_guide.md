 # Compiling Flex Launcher
 Flex Launcher builds natively on Linux and Windows, and features a cross-platform CMake build system. The following external dependencies are required:
 - SDL
 - SDL_image
 - SDL_ttf

## Linux
Flex Launcher on Linux builds with GCC. This guide assumes you already have the tools Git, CMake, and GCC installed on your system. If not, consult your distro's documentation. 

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
Clone the master repo and create a build directory:
```
git clone https://github.com/complexlogic/flex-launcher.git
cd flex-launcher
mkdir build && cd build
```
Generate the Makefile:
```
cmake -Wno-dev .. 
```
Build and test the program:
```
make
./flex-launcher
```
Optionally, install it into your system directories:
```
sudo make install
```
By default, this will install the program and assets with a prefix of /usr/local. If you wish to use a different prefix, re-run the cmake generation step with the -DCMAKE_INSTALL_PREFIX flag.

## Windows
Flex Launcher on Windows builds with Visual Studio, and uses [vcpkg](https://vcpkg.io/en/index.html) to manage the dependencies. Before starting, make sure the following steps are completed:
- Visual Studio is installed. The Community Edition is available for download without charge from Microsoft's website. The following tools and features for Visual Studio are required:
  - C++ core desktop features
  - Latest MSVC
  - Latest Windows SDK
  - C++ CMake tools
- Git is installed and in your PATH environment variable
- CMake is installed and in your PATH environment variable

### Building
Clone the master repo and create a build directory:
```
git clone https://github.com/complexlogic/flex-launcher.git
cd flex-launcher
mkdir build && cd build
```
Build the dependencies with vcpkg:
```
git clone https://github.com/microsoft/vcpkg
.\vcpkg\bootstrap-vcpkg.bat -disableMetrics
.\vcpkg\vcpkg install sdl2 sdl2-image[libjpeg-turbo] sdl2-ttf --triplet=x64-windows-static
```
Generate the Visual Studio project files:
```
cmake -G "Visual Studio 16 2019" -DCMAKE_TOOLCHAIN_FILE=".\vcpkg\scripts\buildsystems\vcpkg.cmake" -DVCPKG_TARGET_TRIPLET="x64-windows-static" ..
```
If you're using a different version of Visual Studio than above, then change the generator output.

Build and test the program:
```
cmake --build .
.\Debug\flex-launcher.exe
```
Optionally, generate a clean zipped install package which may then be extracted to a directory of your choosing:
```
cmake --build . --config Release --target package
```
