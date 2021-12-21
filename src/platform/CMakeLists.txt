if (UNIX)
  add_library(platform "unix.c")
  target_include_directories(platform PUBLIC "${SDL2_INCLUDE_DIRS}" "${PROJECT_BINARY_DIR}")
endif ()

if (WIN32)
  add_library(platform "win32.c")
endif ()
