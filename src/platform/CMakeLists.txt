# Build platform-specific library
if (UNIX)
  add_library(platform "unix.c")
endif ()
if (WIN32)
  add_library(platform "win32.c")
  target_link_libraries(platform SDL2::SDL2main)
endif ()
