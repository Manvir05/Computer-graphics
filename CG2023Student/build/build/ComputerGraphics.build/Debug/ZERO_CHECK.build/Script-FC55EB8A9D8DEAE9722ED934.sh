#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/manvirkaur/Desktop/CG2023Student/build
  make -f /Users/manvirkaur/Desktop/CG2023Student/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/manvirkaur/Desktop/CG2023Student/build
  make -f /Users/manvirkaur/Desktop/CG2023Student/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/manvirkaur/Desktop/CG2023Student/build
  make -f /Users/manvirkaur/Desktop/CG2023Student/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/manvirkaur/Desktop/CG2023Student/build
  make -f /Users/manvirkaur/Desktop/CG2023Student/build/CMakeScripts/ReRunCMake.make
fi

