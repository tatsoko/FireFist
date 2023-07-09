#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd "/Users/admin/Documents/Dokumente - Administrators MacBook Pro/Documents/Uni/Master/SoSe23/AR/Repo/FireFist/Project/build"
  make -f /Users/admin/Documents/Dokumente\ -\ Administrators\ MacBook\ Pro/Documents/Uni/Master/SoSe23/AR/Repo/FireFist/Project/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "Release"; then :
  cd "/Users/admin/Documents/Dokumente - Administrators MacBook Pro/Documents/Uni/Master/SoSe23/AR/Repo/FireFist/Project/build"
  make -f /Users/admin/Documents/Dokumente\ -\ Administrators\ MacBook\ Pro/Documents/Uni/Master/SoSe23/AR/Repo/FireFist/Project/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd "/Users/admin/Documents/Dokumente - Administrators MacBook Pro/Documents/Uni/Master/SoSe23/AR/Repo/FireFist/Project/build"
  make -f /Users/admin/Documents/Dokumente\ -\ Administrators\ MacBook\ Pro/Documents/Uni/Master/SoSe23/AR/Repo/FireFist/Project/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd "/Users/admin/Documents/Dokumente - Administrators MacBook Pro/Documents/Uni/Master/SoSe23/AR/Repo/FireFist/Project/build"
  make -f /Users/admin/Documents/Dokumente\ -\ Administrators\ MacBook\ Pro/Documents/Uni/Master/SoSe23/AR/Repo/FireFist/Project/build/CMakeScripts/ReRunCMake.make
fi

