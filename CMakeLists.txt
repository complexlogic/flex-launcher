# Project info
cmake_minimum_required(VERSION 3.18)
project("Flex Launcher" 
        LANGUAGES C 
        VERSION "1.0.1" 
        DESCRIPTION "Customizable HTPC Application Launcher"
        HOMEPAGE_URL "https://github.com/complexlogic/flex-launcher")
set(EXECUTABLE_TITLE "flex-launcher")

#Generate PKGBUILD for Arch packages
if (PACKAGE STREQUAL "ARCH")
  configure_file("${PROJECT_SOURCE_DIR}/config/PKGBUILD.in" "${PROJECT_BINARY_DIR}/PKGBUILD")
  return()
endif ()
        
# Config setting keys
set(SETTING_DEFAULT_MENU "DefaultMenu")
set(SETTING_MAX_BUTTONS "MaxButtons")
set(SETTING_BACKGROUND_MODE "BackgroundMode")
set(SETTING_BACKGROUND_COLOR "BackgroundColor")
set(SETTING_BACKGROUND_IMAGE "BackgroundImage")
set(SETTING_ICON_SIZE "IconSize")
set(SETTING_ICON_SPACING "IconSpacing")
set(SETTING_TITLE_FONT "TitleFont")
set(SETTING_TITLE_FONT_SIZE "TitleFontSize")
set(SETTING_TITLE_COLOR "TitleColor")
set(SETTING_TITLE_OPACITY "TitleOpacity")
set(SETTING_TITLE_OVERSIZE_MODE "TitleOversizeMode")
set(SETTING_TITLE_PADDING "TitlePadding")
set(SETTING_HIGHLIGHT_COLOR "HighlightColor")
set(SETTING_HIGHLIGHT_OPACITY "HighlightOpacity")
set(SETTING_HIGHLIGHT_CORNER_RADIUS "HighlightCornerRadius")
set(SETTING_HIGHLIGHT_VPADDING "HighlightVPadding")
set(SETTING_HIGHLIGHT_HPADDING "HighlightHPadding")
set(SETTING_BUTTON_CENTERLINE "ButtonCenterline")
set(SETTING_SCROLL_INDICATORS "ScrollIndicators")
set(SETTING_SCROLL_INDICATOR_COLOR "ScrollIndicatorColor")
set(SETTING_SCROLL_INDICATOR_OPACITY "ScrollIndicatorOpacity")
set(SETTING_ON_LAUNCH "OnLaunch")
set(SETTING_RESET_ON_BACK "ResetOnBack")
set(SETTING_ESC_QUIT "EscQuit")
set(SETTING_GAMEPAD_ENABLED "Enabled")
set(SETTING_GAMEPAD_DEVICE "DeviceIndex")
set(SETTING_GAMEPAD_MAPPINGS_FILE "ControllerMappingsFile")
set(SETTING_GAMEPAD_LSTICK_XM "LStickX-")
set(SETTING_GAMEPAD_LSTICK_XP "LStickX+")
set(SETTING_GAMEPAD_LSTICK_YM "LStickY-")
set(SETTING_GAMEPAD_LSTICK_YP "LStickY+")
set(SETTING_GAMEPAD_RSTICK_XM "RStickX-")
set(SETTING_GAMEPAD_RSTICK_XP "RStickX+")
set(SETTING_GAMEPAD_RSTICK_YM "RStickY-")
set(SETTING_GAMEPAD_RSTICK_YP "RStickY+")
set(SETTING_GAMEPAD_LTRIGGER "LTrigger")
set(SETTING_GAMEPAD_RTRIGGER "RTrigger")
set(SETTING_GAMEPAD_BUTTON_A "ButtonA")
set(SETTING_GAMEPAD_BUTTON_B "ButtonB")
set(SETTING_GAMEPAD_BUTTON_X "ButtonX")
set(SETTING_GAMEPAD_BUTTON_Y "ButtonY")
set(SETTING_GAMEPAD_BUTTON_BACK "ButtonBack")
set(SETTING_GAMEPAD_BUTTON_GUIDE "ButtonGuide")
set(SETTING_GAMEPAD_BUTTON_START "ButtonStart")
set(SETTING_GAMEPAD_BUTTON_LEFT_STICK "ButtonLeftStick")
set(SETTING_GAMEPAD_BUTTON_RIGHT_STICK "ButtonRightStick")
set(SETTING_GAMEPAD_BUTTON_LEFT_SHOULDER "ButtonLeftShoulder")
set(SETTING_GAMEPAD_BUTTON_RIGHT_SHOULDER "ButtonRightShoulder")
set(SETTING_GAMEPAD_BUTTON_DPAD_UP "ButtonDPadUp")
set(SETTING_GAMEPAD_BUTTON_DPAD_DOWN "ButtonDPadDown")
set(SETTING_GAMEPAD_BUTTON_DPAD_LEFT "ButtonDPadLeft")
set(SETTING_GAMEPAD_BUTTON_DPAD_RIGHT "ButtonDPadRight")

# Default settings
set(DEFAULT_MENU "Main")
set(DEFAULT_MAX_BUTTONS 4)
set(DEFAULT_BACKGROUND_MODE "Color")
set(DEFAULT_BACKGROUND_COLOR_R "00")
set(DEFAULT_BACKGROUND_COLOR_G "00")
set(DEFAULT_BACKGROUND_COLOR_B "00")
set(DEFAULT_ICON_SIZE 256)
set(DEFAULT_ICON_SPACING "5%")
set(DEFAULT_FONT "OpenSans-Regular.ttf")
set(DEFAULT_FONT_SIZE 36)
set(DEFAULT_TITLE_COLOR_R "FF")
set(DEFAULT_TITLE_COLOR_G "FF")
set(DEFAULT_TITLE_COLOR_B "FF")
set(DEFAULT_TITLE_COLOR_A "FF")
set(DEFAULT_TITLE_OPACITY "100%")
set(DEFAULT_TITLE_OVERSIZE_MODE "Shrink")
set(DEFAULT_TITLE_PADDING 20)
set(DEFAULT_HIGHLIGHT_COLOR_R "FF")
set(DEFAULT_HIGHLIGHT_COLOR_G "FF")
set(DEFAULT_HIGHLIGHT_COLOR_B "FF")
set(DEFAULT_HIGHLIGHT_COLOR_A "40")
set(DEFAULT_HIGHLIGHT_OPACITY "25%")
set(DEFAULT_HIGHLIGHT_CORNER_RADIUS 0)
set(DEFAULT_HIGHLIGHT_VPADDING 30)
set(DEFAULT_HIGHLIGHT_HPADDING 30)
set(DEFAULT_BUTTON_CENTERLINE "50%")
set(DEFAULT_SCROLL_INDICATORS "true")
set(DEFAULT_SCROLL_INDICATOR_COLOR_R "FF")
set(DEFAULT_SCROLL_INDICATOR_COLOR_G "FF")
set(DEFAULT_SCROLL_INDICATOR_COLOR_B "FF")
set(DEFAULT_SCROLL_INDICATOR_COLOR_A "FF")
set(DEFAULT_SCROLL_INDICATOR_OPACITY "100%")
set(DEFAULT_ON_LAUNCH "Hide")
set(DEFAULT_RESET_ON_BACK "false")
set(DEFAULT_ESC_QUIT "true")
set(DEFAULT_GAMEPAD_ENABLED "false")
set(DEFAULT_GAMEPAD_DEVICE 0)

# Linux configuration
if (UNIX)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -s")
  set(CMAKE_SHARED_LINKER_FLAGS "--as-needed")
  
  #Find SDL
  set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
  find_package(SDL2 MODULE REQUIRED)
  find_package(SDL2_image MODULE REQUIRED)
  find_package(SDL2_ttf MODULE REQUIRED)
      
  #Set Default application commands
  set(DESKTOP_PATH "/usr/share/applications/")
  set(CMD_KODI "${DESKTOP_PATH}kodi.desktop")
  set(CMD_STEAM "${DESKTOP_PATH}steam.desktop|BigPicture")
  set(CMD_PLEX "${DESKTOP_PATH}plexmediaplayer.desktop|TVF")  
endif ()

# Windows configuration
if (WIN32)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /FORCE:MULTIPLE") # Workaround for Nanosvg and SDL_image header conflicts
  
  #Force static linking for C Runtime
  set(CompilerFlags
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL
        CMAKE_C_FLAGS_RELWITHDEBINFO)
  foreach(CompilerFlag ${CompilerFlags})
    string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    set(${CompilerFlag} "${${CompilerFlag}}" CACHE STRING "msvc compiler flags" FORCE)
  endforeach()

  #Find SDL
  find_package(SDL2 CONFIG REQUIRED)
  find_package(sdl2-image CONFIG REQUIRED)
  find_package(sdl2-ttf CONFIG REQUIRED)
    
  # Application commands
  set(CMD_KODI "C:\\Program Files\\Kodi\\kodi.exe")
  set(CMD_PLEX "C:\\Program Files\\Plex\\Plex Media Player\\PlexMediaPlayer.exe")
  set(CMD_STEAM "\"C:\\Program Files (x86)\\Steam\\steam.exe\" \"steam://open/bigpicture\"")
  
  if (NOT PACKAGE)
    set(PACKAGE "ZIP")
  endif ()
endif () 

#Build src
add_subdirectory("src")

# Install
if (UNIX)
  set(INSTALL_DIR_BIN "${CMAKE_INSTALL_PREFIX}/bin")
  set(INSTALL_DIR_SHARE "${CMAKE_INSTALL_PREFIX}/share/${EXECUTABLE_TITLE}")
  set(INSTALL_DIR_DESKTOP "${CMAKE_INSTALL_PREFIX}/share/applications")
  set(INSTALL_DIR_CONFIGFILE "${INSTALL_DIR_SHARE}")
  set(INSTALL_DIR_ASSETS "${INSTALL_DIR_SHARE}")
else ()
  set(INSTALL_DIR_BIN "./")
  set(INSTALL_DIR_CONFIGFILE "./")
  set(INSTALL_DIR_ASSETS "./")
endif ()
install(TARGETS ${EXECUTABLE_TITLE} DESTINATION "${INSTALL_DIR_BIN}")
install(FILES "${PROJECT_BINARY_DIR}/install/config.ini" DESTINATION "${INSTALL_DIR_CONFIGFILE}")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/assets" DESTINATION "${INSTALL_DIR_ASSETS}")

#Configure icons and desktop file
if (UNIX)
  configure_file("${PROJECT_SOURCE_DIR}/config/launcher.desktop.in" "${PROJECT_BINARY_DIR}/install/${EXECUTABLE_TITLE}.desktop")
  install(FILES "${PROJECT_BINARY_DIR}/install/${EXECUTABLE_TITLE}.desktop" DESTINATION "${INSTALL_DIR_DESKTOP}")
  set(INSTALL_DIR_ICONS "${CMAKE_INSTALL_PREFIX}/share/icons")
  install(FILES "${PROJECT_SOURCE_DIR}/extra/flex-launcher.png" DESTINATION "${INSTALL_DIR_ICONS}/hicolor/48x48/apps")
  install(FILES "${PROJECT_SOURCE_DIR}/extra/flex-launcher.svg" DESTINATION "${INSTALL_DIR_ICONS}/hicolor/scalable/apps")
endif ()

if (WIN32)
  install(FILES "${PROJECT_SOURCE_DIR}/UNLICENSE.txt" DESTINATION "${INSTALL_DIR_BIN}")
  install(FILES "${PROJECT_SOURCE_DIR}/CHANGELOG.txt" DESTINATION "${INSTALL_DIR_BIN}")
endif ()

# Configure CPack for packages
if (PACKAGE STREQUAL "DEB")
  set(CPACK_DEBIAN_PACKAGE_NAME ${EXECUTABLE_TITLE})
  set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
  set(CPACK_DEBIAN_PACKAGE_VERSION ${CMAKE_PROJECT_VERSION})
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsdl2-2.0-0 (>= 2.0.14), libsdl2-image-2.0-0 (>= 2.0.5), libsdl2-ttf-2.0-0 (>= 2.0.15)")
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "complexlogic")
  set(CPACK_DEBIAN_PACKAGE_SECTION "video")
  set(CPACK_DEBIAN_ARCHIVE_TYPE "gnutar")
  set(CPACK_DEBIAN_COMPRESSION_TYPE "gzip")
  set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
endif ()

if (PACKAGE)
  set(CPACK_PACKAGE_NAME ${EXECUTABLE_TITLE})
  set(CPACK_GENERATOR ${PACKAGE})
  include(CPack)
endif ()
  
# Set paths for config files
set(FONT_PREFIX "${PROJECT_BINARY_DIR}/assets/fonts/")
set(ICONS_PREFIX "${PROJECT_BINARY_DIR}/assets/icons/")
if("${PROJECT_SOURCE_DIR}/build" STREQUAL "${PROJECT_BINARY_DIR}")
  set(SVG_RELATIVE "../assets/")
else()
  set(SVG_RELATIVE "../${EXECUTABLE_TITLE}/assets/")
endif()
file(COPY "${PROJECT_SOURCE_DIR}/assets" DESTINATION ${PROJECT_BINARY_DIR})

# Generate build config file
if (WIN32)
  string(REPLACE "/" "\\" FONT_PREFIX ${FONT_PREFIX})
  string(REPLACE "/" "\\" ICONS_PREFIX ${ICONS_PREFIX})
endif ()
configure_file("${PROJECT_SOURCE_DIR}/config/config.ini.in" "${PROJECT_BINARY_DIR}/config.ini")

# Generate install config file
if (WIN32)
  set(PATH_ASSETS_RELATIVE "./assets/")
  set(FONT_PREFIX "${PATH_ASSETS_RELATIVE}fonts/")
  set(ICONS_PREFIX "${PATH_ASSETS_RELATIVE}icons/")
else ()
  set(FONT_PREFIX "${INSTALL_DIR_ASSETS}/assets/fonts/")
  set(ICONS_PREFIX "${INSTALL_DIR_ASSETS}/assets/icons/")
endif ()
  
if (WIN32)
  string(REPLACE "/" "\\" FONT_PREFIX ${FONT_PREFIX})
  string(REPLACE "/" "\\" ICONS_PREFIX ${ICONS_PREFIX})
endif ()
configure_file("${PROJECT_SOURCE_DIR}/config/config.ini.in" "${PROJECT_BINARY_DIR}/install/config.ini")
