#Build main launcher executable file
if (UNIX)
  add_executable(${EXECUTABLE_TITLE} "launcher.c" "util.c" "image.c" "debug.c" "clock.c")
endif ()
if (WIN32)
  set(APP_ICON_RESOURCE_WINDOWS "${PROJECT_SOURCE_DIR}/config/${EXECUTABLE_TITLE}.rc")
  set(MANIFEST_FILE "${PROJECT_BINARY_DIR}/${EXECUTABLE_TITLE}.manifest")
  add_executable(${EXECUTABLE_TITLE} WIN32 "launcher.c" "util.c" "image.c" "debug.c" "clock.c" ${MANIFEST_FILE} ${APP_ICON_RESOURCE_WINDOWS})
endif()

# Configure main header file  
configure_file("launcher_config.h.in" "${PROJECT_BINARY_DIR}/launcher_config.h")

# Build libraries
add_subdirectory("external")
add_subdirectory("platform")

# Link libraries together 
target_link_libraries(${EXECUTABLE_TITLE} platform inih)
if (WIN32)
  target_link_libraries(${EXECUTABLE_TITLE} SDL2::SDL2main SDL2::SDL2_image SDL2::SDL2_ttf)
endif ()


