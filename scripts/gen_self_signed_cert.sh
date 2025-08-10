#!/bin/bash
set -e
DIR=${1:-.}
mkdir -p "$DIR"
KEY="$DIR/scastd.key"
CERT="$DIR/scastd.crt"
openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
  -keyout "$KEY" -out "$CERT" -subj "/CN=scastd" \
  -addext "subjectAltName=IP:127.0.0.1,IP:0.0.0.0"
echo "Certificate: $CERT"
echo "Key: $KEY"
