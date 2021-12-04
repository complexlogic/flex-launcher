#Build instructions
if (UNIX)
  add_executable(${EXECUTABLE_TITLE} "launcher.c")
endif ()
if (WIN32)
  set(APP_ICON_RESOURCE_WINDOWS "${PROJECT_SOURCE_DIR}/extra/${EXECUTABLE_TITLE}.rc")
  add_executable(${EXECUTABLE_TITLE} "launcher.c" ${APP_ICON_RESOURCE_WINDOWS})
endif()
set (EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}")
target_include_directories(${EXECUTABLE_TITLE} PUBLIC "${PROJECT_BINARY_DIR}")
add_library(util "util.c")
target_include_directories(util PUBLIC "${PROJECT_BINARY_DIR}")

# Configure main header file  
configure_file("${PROJECT_SOURCE_DIR}/src/launcher.h.in" "${PROJECT_BINARY_DIR}/launcher.h")

#SDL includes and libs
if (UNIX)
  target_include_directories(util PUBLIC "${SDL2_INCLUDE_DIRS}")
  target_include_directories(${EXECUTABLE_TITLE} PUBLIC "${SDL2_INCLUDE_DIRS}" "${SDL2_IMAGE_INCLUDE_DIRS}" "${SDL2_TTF_INCLUDE_DIRS}")
  target_link_libraries(${EXECUTABLE_TITLE} platform inih util SDL2::Main SDL2::Image SDL2::TTF m)
endif ()
if (WIN32)
  target_link_libraries(util SDL2::SDL2 SDL2::SDL2main SDL2::SDL2-static)
  target_link_libraries(${EXECUTABLE_TITLE} platform inih util SDL2::SDL2 SDL2::SDL2main SDL2::SDL2-static SDL2::SDL2_image SDL2::SDL2_ttf)
endif ()

add_subdirectory("external")
add_subdirectory("platform")
