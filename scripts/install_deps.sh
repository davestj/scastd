#!/bin/bash
set -e

# Detect platform
UNAME="$(uname)"
ARCH="$(uname -m)"

if [[ "$UNAME" == "Darwin" && "$ARCH" == "arm64" ]]; then
  if ! command -v brew >/dev/null 2>&1; then
    echo "Homebrew is required on macOS" >&2
    exit 1
  fi
  BREW_PKGS=(libxml2 curl mariadb libpq libmicrohttpd)
  MISSING=()
  for pkg in "${BREW_PKGS[@]}"; do
    if ! brew list "$pkg" >/dev/null 2>&1; then
      MISSING+=("$pkg")
    fi
  done
  if [[ ${#MISSING[@]} -gt 0 ]]; then
    brew update
    brew install "${MISSING[@]}"
  else
    echo "Homebrew packages already installed"
  fi
  BREW_PREFIX="$(brew --prefix)"
  PATHS=(
    "$BREW_PREFIX/opt/libxml2/lib/pkgconfig"
    "$BREW_PREFIX/opt/curl/lib/pkgconfig"
    "$BREW_PREFIX/opt/mariadb/lib/pkgconfig"
    "$BREW_PREFIX/opt/libpq/lib/pkgconfig"
    "$BREW_PREFIX/opt/libmicrohttpd/lib/pkgconfig"
  )
  for p in "${PATHS[@]}"; do
    if [[ -d "$p" ]]; then
      PKG_CONFIG_PATH="$p:${PKG_CONFIG_PATH:-}"
    fi
  done
  export PKG_CONFIG_PATH
  echo "PKG_CONFIG_PATH set to: $PKG_CONFIG_PATH"
elif [[ "$UNAME" == "Linux" ]]; then
  if ! command -v lsb_release >/dev/null 2>&1; then
    echo "lsb_release is required on Linux" >&2
    exit 1
  fi
  DISTRO="$(lsb_release -is)"
  VERSION="$(lsb_release -rs)"
  if [[ "$DISTRO" == "Debian" && "$VERSION" == "12" ]] ||
     [[ "$DISTRO" == "Ubuntu" && ( "$VERSION" == "22.04" || "$VERSION" == "24.04" ) ]]; then
    APT_PKGS=(libxml2-dev curl libcurl4-openssl-dev mariadb-client libmariadb-dev libpq-dev libmicrohttpd-dev)
    MISSING=()
    for pkg in "${APT_PKGS[@]}"; do
      if ! dpkg -s "$pkg" >/dev/null 2>&1; then
        MISSING+=("$pkg")
      fi
    done
    if [[ ${#MISSING[@]} -gt 0 ]]; then
      if [[ $EUID -ne 0 ]]; then
        sudo apt-get update
        sudo apt-get install -y "${MISSING[@]}"
      else
        apt-get update
        apt-get install -y "${MISSING[@]}"
      fi
    else
      echo "APT packages already installed"
    fi
  else
    echo "Unsupported Linux distribution: $DISTRO $VERSION" >&2
    exit 1
  fi
else
  echo "Unsupported platform: $UNAME $ARCH" >&2
  exit 1
fi
