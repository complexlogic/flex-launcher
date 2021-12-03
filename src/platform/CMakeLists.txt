if (UNIX)
  add_library(platform "unix.c")
  target_include_directories(platform PUBLIC "${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/src/external")
endif ()

if (WIN32)
  add_library(platform "windows.c")
endif ()
