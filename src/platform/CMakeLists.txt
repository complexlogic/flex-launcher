# Build platform-specific library
if (UNIX)
  add_library(platform unix.c unix.h platform.h)
  target_link_libraries(platform PkgConfig::SDL2)
endif ()
if (WIN32)
  add_library(platform win32.c platform.h)
  target_link_libraries(platform
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
  )
endif ()
