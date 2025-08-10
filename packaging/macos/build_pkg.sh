#!/bin/bash
set -euo pipefail

VERSION=${1:-1.0}
PREFIX=/usr/local
BUILD_DIR=$(pwd)/pkg_build

rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

./autogen.sh
./configure --prefix="$PREFIX"
make
make DESTDIR="$BUILD_DIR" install

pkgbuild --root "$BUILD_DIR" \
  --identifier org.scastd.pkg \
  --version "$VERSION" \
  --install-location / \
  "scastd-${VERSION}.pkg"
