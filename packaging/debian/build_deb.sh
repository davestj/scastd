#!/bin/bash
set -euo pipefail

VERSION=${1:-1.0}
DISTRO=${2:-bookworm}
PREFIX=/usr/local
BUILD_DIR=$(pwd)/deb_build
OUT_DIR=$(pwd)/dist

rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR" "$OUT_DIR"

# Build and stage files
./autogen.sh
./configure --prefix="$PREFIX"
make
make DESTDIR="$BUILD_DIR" install

# Control metadata
mkdir -p "$BUILD_DIR/DEBIAN"
cat > "$BUILD_DIR/DEBIAN/control" <<CTRL
Package: scastd
Version: ${VERSION}
Section: net
Priority: optional
Architecture: $(dpkg --print-architecture)
Depends: libcurl4, libmicrohttpd12, libmariadb3, libpq5, certbot
Maintainer: unknown
Description: Scast Daemon
CTRL

cp packaging/debian/postinst "$BUILD_DIR/DEBIAN/postinst"
chmod 755 "$BUILD_DIR/DEBIAN/postinst"

dpkg-deb --build "$BUILD_DIR" "$OUT_DIR/scastd_${VERSION}_${DISTRO}_$(dpkg --print-architecture).deb"
