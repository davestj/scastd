#!/bin/bash
set -e

usage() {
  echo "Usage: $0 -e EMAIL -d DOMAIN [-d DOMAIN...]" >&2
  exit 1
}

if [[ $EUID -ne 0 ]]; then
  echo "This script must be run as root" >&2
  exit 1
fi

EMAIL=""
DOMAINS=()
while [[ $# -gt 0 ]]; do
  case "$1" in
    -e|--email)
      EMAIL="$2"
      shift 2
      ;;
    -d|--domain)
      DOMAINS+=("$2")
      shift 2
      ;;
    *)
      usage
      ;;
  esac
done

if [[ -z "$EMAIL" || ${#DOMAINS[@]} -eq 0 ]]; then
  usage
fi

if ! command -v certbot >/dev/null 2>&1; then
  if [[ "$(uname)" == "Darwin" ]]; then
    if ! command -v brew >/dev/null 2>&1; then
      echo "Homebrew is required on macOS" >&2
      exit 1
    fi
    brew update
    brew install certbot
  elif [[ -f /etc/debian_version ]]; then
    apt-get update
    apt-get install -y certbot
  else
    echo "Unsupported platform" >&2
    exit 1
  fi
fi

DOMAIN_ARGS=()
for d in "${DOMAINS[@]}"; do
  DOMAIN_ARGS+=("-d" "$d")
done

certbot certonly --standalone --agree-tos -m "$EMAIL" "${DOMAIN_ARGS[@]}" --non-interactive

if [[ "$(uname)" == "Darwin" ]]; then
  brew services start certbot
else
  systemctl enable --now certbot.timer
fi

echo "Certificates stored under /etc/letsencrypt/"
