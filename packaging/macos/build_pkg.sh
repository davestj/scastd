#!/bin/bash
set -euo pipefail

VERSION=${1:-1.0}
PREFIX=/usr/local
BUILD_DIR=$(pwd)/pkg_build
DIST_DIR="dist"

# Create dist directory
mkdir -p "$DIST_DIR"

# Clean up previous build
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

echo "Building SCASTD macOS package v$VERSION..."

# Build the package
./autogen.sh
./configure --prefix="$PREFIX"
make
make DESTDIR="$BUILD_DIR" install

# Create the .pkg file
echo "Creating macOS installer package..."
pkgbuild --root "$BUILD_DIR" \
  --identifier org.scastd.pkg \
  --version "$VERSION" \
  --install-location / \
  "$DIST_DIR/scastd-${VERSION}.pkg"

echo "Package created: $DIST_DIR/scastd-${VERSION}.pkg"

# Generate Homebrew formula
echo "Generating Homebrew formula..."
if [[ -x "packaging/macos/build_formula.sh" ]]; then
    ./packaging/macos/build_formula.sh "$VERSION"
else
    echo "Warning: Formula build script not found or not executable"
fi

echo "macOS packaging completed!"
echo "Files created:"
echo "  - $DIST_DIR/scastd-${VERSION}.pkg (macOS installer)"
echo "  - $DIST_DIR/scastd.rb (Homebrew formula)"
