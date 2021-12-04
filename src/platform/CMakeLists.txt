if (UNIX)
  add_library(platform "unix.c")
endif ()

if (WIN32)
  add_library(platform "win32.c")
endif ()
