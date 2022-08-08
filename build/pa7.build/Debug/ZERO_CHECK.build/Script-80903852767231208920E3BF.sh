#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/yifengyu/Desktop/itp435/pa7-yifeng-yu/build
  make -f /Users/yifengyu/Desktop/itp435/pa7-yifeng-yu/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/yifengyu/Desktop/itp435/pa7-yifeng-yu/build
  make -f /Users/yifengyu/Desktop/itp435/pa7-yifeng-yu/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/yifengyu/Desktop/itp435/pa7-yifeng-yu/build
  make -f /Users/yifengyu/Desktop/itp435/pa7-yifeng-yu/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/yifengyu/Desktop/itp435/pa7-yifeng-yu/build
  make -f /Users/yifengyu/Desktop/itp435/pa7-yifeng-yu/build/CMakeScripts/ReRunCMake.make
fi

