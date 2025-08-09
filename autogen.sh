#!/bin/sh
# Run this to set up the build system: configure, makefiles, etc.

set -e

package="scastd"

olddir=$(pwd)
srcdir=$(dirname "$0")
test -z "$srcdir" && srcdir=.
cd "$srcdir"

echo "Generating configuration files for $package, please wait...."
autoreconf -I m4 --install --force --verbose

cd "$olddir"

echo
echo "Now run ./configure [options]"

