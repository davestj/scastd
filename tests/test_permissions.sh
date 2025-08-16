#!/bin/sh
set -e

# Verify directory ownership and permissions after installation
TMPDIR=$(mktemp -d)
trap "rm -rf \"$TMPDIR\"" EXIT

make -C .. DESTDIR="$TMPDIR" install-data-local >/dev/null

check_mode() {
    expected=$1
    path=$2
    mode=$(stat -c '%a' "$path")
    test "$mode" = "$expected"
}

check_owner() {
    expected=$1
    path=$2
    owner=$(stat -c '%U:%G' "$path")
    test "$owner" = "$expected"
}

check_mode 755 "$TMPDIR/etc/scastd"
check_mode 640 "$TMPDIR/etc/scastd/scastd.conf"
check_mode 640 "$TMPDIR/etc/scastd/scastd.db"
check_mode 750 "$TMPDIR/var/log/scastd"

check_owner root:root "$TMPDIR/etc/scastd"
check_owner root:root "$TMPDIR/var/log/scastd"
