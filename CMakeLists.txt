# Project info
cmake_minimum_required(VERSION 3.18)
project("Flex Launcher" 
  LANGUAGES C
  VERSION 2.2
  DESCRIPTION "Customizable HTPC Application Launcher"
  HOMEPAGE_URL "https://github.com/complexlogic/flex-launcher"
)
set(EXECUTABLE_TITLE "flex-launcher")
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}")
include_directories(${PROJECT_BINARY_DIR})

option(EXTRA_WARNINGS "Enable extra compiler warnings" OFF)
if (EXTRA_WARNINGS)
  if (MSVC)
    add_compile_options(/W4 /WX)
  else ()
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion)
  endif ()
endif ()

# Set Visual Studio solution startup project
if (WIN32)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${EXECUTABLE_TITLE})
endif ()

# Minimum library versions for Linux
set(MIN_SDL_VERSION "2.0.14")
set(MIN_SDL_IMAGE_VERSION "2.0.5")
set(MIN_SDL_TTF_VERSION "2.0.15")
set(MIN_GLIBC_VERSION "2.31") # Enforced for .deb packages only

# Generate PKGBUILD script for Arch packages
if (PACKAGE STREQUAL "ARCH")
  configure_file("${PROJECT_SOURCE_DIR}/config/PKGBUILD.in" "${PROJECT_BINARY_DIR}/PKGBUILD")
  return()
endif ()
        
# Import settings
include(${PROJECT_SOURCE_DIR}/config/config_settings.cmake)

# Find dependencies - Linux
if (UNIX)
  find_package(PkgConfig MODULE REQUIRED)
  pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2>=${MIN_SDL_VERSION})
  pkg_check_modules(SDL2_IMAGE REQUIRED IMPORTED_TARGET SDL2_image>=${MIN_SDL_IMAGE_VERSION})
  pkg_check_modules(SDL2_TTF REQUIRED IMPORTED_TARGET SDL2_ttf>=${MIN_SDL_TTF_VERSION})
  pkg_check_modules(INIH REQUIRED IMPORTED_TARGET inih)
endif ()

# Find dependencies - Windows
if (WIN32)
  find_package(SDL2 CONFIG REQUIRED)
  find_package(sdl2_image CONFIG REQUIRED)
  find_package(SDL2_ttf CONFIG REQUIRED)
  
  find_path(INIH_INCLUDE_DIR "ini.h" REQUIRED)
  find_library(INIH inih REQUIRED)
  find_path(GETOPT_INCLUDE_DIR "getopt.h" REQUIRED)
  find_library(GETOPT getopt REQUIRED)
endif () 

# Build configuration - Linux
if (UNIX)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -s")
  set(CMAKE_SHARED_LINKER_FLAGS "--as-needed")
endif ()

# Build configuration - Windows
if (WIN32)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /FORCE:MULTIPLE") # Workaround for Nanosvg and SDL_image header conflicts
 endif ()
 
 # Configure default applications - Linux
 if (UNIX)
  set(DESKTOP_PATH "/usr/share/applications")
  set(CMD_KODI "${DESKTOP_PATH}/kodi.desktop")
  set(CMD_PLEX "${DESKTOP_PATH}/plexmediaplayer.desktop;TVF")
  if (RPI)
    set(TITLE_GAMES "RetroArch")
    set(ICON_GAMES "retroarch.png")
    set(CMD_GAMES "${DESKTOP_PATH}/retroarch.desktop")
  else ()
    set(TITLE_GAMES "Steam")
    set(ICON_GAMES "steam.png")
    set(CMD_GAMES "${DESKTOP_PATH}/steam.desktop;BigPicture")
  endif ()
endif ()

# Configure default applications - Windows
if (WIN32)
  set(CMD_KODI "\"C:\\Program Files\\Kodi\\kodi.exe\"")
  set(CMD_PLEX "\"C:\\Program Files\\Plex\\Plex Media Player\\PlexMediaPlayer.exe\"")
  set(TITLE_GAMES "Steam")
  set(ICON_GAMES "steam.png")
  set(CMD_GAMES "\"C:\\Program Files (x86)\\Steam\\steam.exe\" steam://open/bigpicture")
endif ()

# Generate Windows application manifest
if (WIN32)
  set(VERSION_M ${PROJECT_VERSION_MAJOR})
  if (PROJECT_VERSION_MINOR)
    set(VERSION_N ${PROJECT_VERSION_MINOR})
  else ()
    set(VERSION_N 0)
  endif()
  if (PROJECT_VERSION_PATCH)
    set(VERSION_O ${PROJECT_VERSION_PATCH})
  else ()
    set(VERSION_O 0)
  endif()
  if (PROJECT_VERSION_TWEAK)
    set(VERSION_P ${PROJECT_VERSION_TWEAK})
  else ()
    set(VERSION_P 0)
  endif()
  configure_file(
    ${PROJECT_SOURCE_DIR}/config/${EXECUTABLE_TITLE}.manifest.in 
	${PROJECT_BINARY_DIR}/${EXECUTABLE_TITLE}.manifest
  )
endif()

# Set time format strings
if (WIN32)
  set(REMOVE_ZERO "#")
else()
  set(REMOVE_ZERO "-")
endif ()
set(TIME_STRING_12HR "%${REMOVE_ZERO}I:%M %p")
set(TIME_STRING_24HR "%H:%M")
set(DATE_STRING_LITTLE "%d %b")
set(DATE_STRING_BIG "%b %${REMOVE_ZERO}d")

# Configure main header file  
configure_file("${PROJECT_SOURCE_DIR}/config/launcher_config.h.in" "${PROJECT_BINARY_DIR}/launcher_config.h")

#Build source files
add_subdirectory("src")

# Installation - Linux
if (UNIX)
  set(INSTALL_DIR_BIN "${CMAKE_INSTALL_PREFIX}/bin")
  set(INSTALL_DIR_SHARE "${CMAKE_INSTALL_PREFIX}/share/${EXECUTABLE_TITLE}")
  set(INSTALL_DIR_DESKTOP "${CMAKE_INSTALL_PREFIX}/share/applications")
  set(INSTALL_DIR_CONFIGFILE "${INSTALL_DIR_SHARE}")
  set(INSTALL_DIR_ASSETS "${INSTALL_DIR_SHARE}")
  
  # Configure Debian packages
  if (PACKAGE STREQUAL "DEB")
    set(CPACK_DEBIAN_PACKAGE_NAME ${EXECUTABLE_TITLE})
    set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
    set(CPACK_DEBIAN_PACKAGE_VERSION ${CMAKE_PROJECT_VERSION})
    if (NOT CPACK_DEBIAN_PACKAGE_ARCHITECTURE)
      set (CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
    endif ()
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsdl2-2.0-0 (>= ${MIN_SDL_VERSION}), libsdl2-image-2.0-0 (>= ${MIN_SDL_IMAGE_VERSION}), libsdl2-ttf-2.0-0 (>= ${MIN_SDL_TTF_VERSION}), libc6 (>= ${MIN_GLIBC_VERSION}), libinih1")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "complexlogic")
    set(CPACK_DEBIAN_PACKAGE_SECTION "video")
    set(CPACK_DEBIAN_ARCHIVE_TYPE "gnutar")
    set(CPACK_DEBIAN_COMPRESSION_TYPE "gzip")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
  endif ()
endif ()

# Installation - Windows
if (WIN32)
  set(INSTALL_DIR_BIN "./")
  set(INSTALL_DIR_CONFIGFILE "./")
  set(INSTALL_DIR_ASSETS "./")

  configure_file(${PROJECT_SOURCE_DIR}/CHANGELOG ${PROJECT_BINARY_DIR}/CHANGELOG.txt)
  install(FILES ${PROJECT_BINARY_DIR}/CHANGELOG.txt DESTINATION "${INSTALL_DIR_BIN}")
  
  # Copy the VC Runtime DLL in case user doesn't have it installed
  set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
  include(InstallRequiredSystemLibraries)
  foreach(lib ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS})
    string(FIND ${lib} "vcruntime140.dll" vcruntime)
    if (NOT vcruntime EQUAL -1)
      install(FILES ${lib} DESTINATION "./")
    endif ()
  endforeach()
  
  # Set package
  if (NOT PACKAGE)
    set(PACKAGE "ZIP")
  endif ()
endif ()

# Installation - Common
install(TARGETS ${EXECUTABLE_TITLE} DESTINATION "${INSTALL_DIR_BIN}")
install(FILES "${PROJECT_BINARY_DIR}/install/config.ini" DESTINATION "${INSTALL_DIR_CONFIGFILE}")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/assets" DESTINATION "${INSTALL_DIR_ASSETS}")
if (PACKAGE)
  set(CPACK_PACKAGE_NAME ${EXECUTABLE_TITLE})
  set(CPACK_GENERATOR ${PACKAGE})
  include(CPack)
endif ()

#Configure Linux application icons and .desktop file
if (UNIX)
  configure_file("${PROJECT_SOURCE_DIR}/config/launcher.desktop.in" "${PROJECT_BINARY_DIR}/install/${EXECUTABLE_TITLE}.desktop")
  install(FILES "${PROJECT_BINARY_DIR}/install/${EXECUTABLE_TITLE}.desktop" DESTINATION "${INSTALL_DIR_DESKTOP}")
  set(INSTALL_DIR_ICONS "${CMAKE_INSTALL_PREFIX}/share/icons")
  install(FILES "${PROJECT_SOURCE_DIR}/docs/flex-launcher.png" DESTINATION "${INSTALL_DIR_ICONS}/hicolor/48x48/apps")
  install(FILES "${PROJECT_SOURCE_DIR}/docs/flex-launcher.svg" DESTINATION "${INSTALL_DIR_ICONS}/hicolor/scalable/apps")
endif ()
  
# Set paths for config files
set(FONT_PREFIX "${PROJECT_BINARY_DIR}/assets/fonts/")
set(ICONS_PREFIX "${PROJECT_BINARY_DIR}/assets/icons/")
file(COPY "${PROJECT_SOURCE_DIR}/assets" DESTINATION ${PROJECT_BINARY_DIR})

# Generate build config file
if (WIN32)
  string(REPLACE "/" "\\" FONT_PREFIX ${FONT_PREFIX})
  string(REPLACE "/" "\\" ICONS_PREFIX ${ICONS_PREFIX})
endif ()
configure_file("${PROJECT_SOURCE_DIR}/config/config.ini.in" "${PROJECT_BINARY_DIR}/config.ini")

# Generate install config file
if (WIN32)
  set(PATH_ASSETS_RELATIVE "./assets")
  set(FONT_PREFIX "${PATH_ASSETS_RELATIVE}/fonts/")
  set(ICONS_PREFIX "${PATH_ASSETS_RELATIVE}/icons/")
else ()
  set(FONT_PREFIX "${INSTALL_DIR_ASSETS}/assets/fonts/")
  set(ICONS_PREFIX "${INSTALL_DIR_ASSETS}/assets/icons/")
endif ()
  
if (WIN32)
  string(REPLACE "/" "\\" FONT_PREFIX ${FONT_PREFIX})
  string(REPLACE "/" "\\" ICONS_PREFIX ${ICONS_PREFIX})
endif ()
configure_file("${PROJECT_SOURCE_DIR}/config/config.ini.in" "${PROJECT_BINARY_DIR}/install/config.ini")
