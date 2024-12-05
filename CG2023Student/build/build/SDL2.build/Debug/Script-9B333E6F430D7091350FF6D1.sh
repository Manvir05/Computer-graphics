#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/manvirkaur/Desktop/CG2023Student/build/libraries/sdl2
  /usr/local/Cellar/cmake/3.25.2/bin/cmake -E cmake_symlink_library /Users/manvirkaur/Desktop/CG2023Student/build/Debug/libSDL2-2.0d.0.dylib /Users/manvirkaur/Desktop/CG2023Student/build/Debug/libSDL2-2.0d.0.dylib /Users/manvirkaur/Desktop/CG2023Student/build/Debug/libSDL2-2.0d.dylib
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/manvirkaur/Desktop/CG2023Student/build/libraries/sdl2
  /usr/local/Cellar/cmake/3.25.2/bin/cmake -E cmake_symlink_library /Users/manvirkaur/Desktop/CG2023Student/build/Release/libSDL2-2.0.0.dylib /Users/manvirkaur/Desktop/CG2023Student/build/Release/libSDL2-2.0.0.dylib /Users/manvirkaur/Desktop/CG2023Student/build/Release/libSDL2-2.0.dylib
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/manvirkaur/Desktop/CG2023Student/build/libraries/sdl2
  /usr/local/Cellar/cmake/3.25.2/bin/cmake -E cmake_symlink_library /Users/manvirkaur/Desktop/CG2023Student/build/MinSizeRel/libSDL2-2.0.0.dylib /Users/manvirkaur/Desktop/CG2023Student/build/MinSizeRel/libSDL2-2.0.0.dylib /Users/manvirkaur/Desktop/CG2023Student/build/MinSizeRel/libSDL2-2.0.dylib
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/manvirkaur/Desktop/CG2023Student/build/libraries/sdl2
  /usr/local/Cellar/cmake/3.25.2/bin/cmake -E cmake_symlink_library /Users/manvirkaur/Desktop/CG2023Student/build/RelWithDebInfo/libSDL2-2.0.0.dylib /Users/manvirkaur/Desktop/CG2023Student/build/RelWithDebInfo/libSDL2-2.0.0.dylib /Users/manvirkaur/Desktop/CG2023Student/build/RelWithDebInfo/libSDL2-2.0.dylib
fi

