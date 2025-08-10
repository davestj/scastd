#!/bin/sh
# Run this to set up the build system: configure, makefiles, etc.

set -e

package="scastd"

olddir=$(pwd)
srcdir=$(dirname "$0")
test -z "$srcdir" && srcdir=.
cd "$srcdir"

OS=$(uname -s)
case "$OS" in
  Darwin)
    export PKG_CONFIG_PATH="/opt/homebrew/lib/pkgconfig:/opt/homebrew/share/pkgconfig:${PKG_CONFIG_PATH}"
    export CPPFLAGS="-I/opt/homebrew/include ${CPPFLAGS}"
    export LDFLAGS="-L/opt/homebrew/lib ${LDFLAGS}"
    ;;
  Linux)
    export PKG_CONFIG_PATH="/usr/local/lib/pkgconfig:/usr/lib/pkgconfig:${PKG_CONFIG_PATH}"
    ;;
esac

echo "Generating configuration files for $package, please wait...."
autoreconf -I m4 --install --force --verbose

cd "$olddir"

echo
echo "Now run ./configure [options]"

