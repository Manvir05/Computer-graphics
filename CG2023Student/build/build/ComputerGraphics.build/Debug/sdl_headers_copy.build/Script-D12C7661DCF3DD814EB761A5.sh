#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/manvirkaur/Desktop/CG2023Student/build/libraries/sdl2
  /usr/local/Cellar/cmake/3.25.2/bin/cmake -E copy_if_different /Users/manvirkaur/Desktop/CG2023Student/libraries/sdl2/include/SDL_opengl_glext.h /Users/manvirkaur/Desktop/CG2023Student/build/libraries/sdl2/include/SDL_opengl_glext.h
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/manvirkaur/Desktop/CG2023Student/build/libraries/sdl2
  /usr/local/Cellar/cmake/3.25.2/bin/cmake -E copy_if_different /Users/manvirkaur/Desktop/CG2023Student/libraries/sdl2/include/SDL_opengl_glext.h /Users/manvirkaur/Desktop/CG2023Student/build/libraries/sdl2/include/SDL_opengl_glext.h
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/manvirkaur/Desktop/CG2023Student/build/libraries/sdl2
  /usr/local/Cellar/cmake/3.25.2/bin/cmake -E copy_if_different /Users/manvirkaur/Desktop/CG2023Student/libraries/sdl2/include/SDL_opengl_glext.h /Users/manvirkaur/Desktop/CG2023Student/build/libraries/sdl2/include/SDL_opengl_glext.h
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/manvirkaur/Desktop/CG2023Student/build/libraries/sdl2
  /usr/local/Cellar/cmake/3.25.2/bin/cmake -E copy_if_different /Users/manvirkaur/Desktop/CG2023Student/libraries/sdl2/include/SDL_opengl_glext.h /Users/manvirkaur/Desktop/CG2023Student/build/libraries/sdl2/include/SDL_opengl_glext.h
fi
