#!/bin/sh
# Run this to set up the build system: configure, makefiles, etc.

set -e

# Ensure required autotools are available before running autoreconf
command -v aclocal >/dev/null 2>&1 || {
  echo "Error: 'aclocal' not found. Please install automake." >&2
  exit 1
}
LIBTOOLIZE=$(command -v libtoolize 2>/dev/null || command -v glibtoolize)
[[ -z "$LIBTOOLIZE" ]] && { echo "libtoolize/glibtoolize not found"; exit 1; }
"$LIBTOOLIZE" --force --copy

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

