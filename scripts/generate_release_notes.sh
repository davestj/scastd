#!/bin/bash
set -euo pipefail

START_REF=${1:-}
END_REF=${2:-HEAD}

if [ -z "$START_REF" ]; then
  START_REF=$(git describe --tags --abbrev=0 "$END_REF^" 2>/dev/null || git rev-list --max-parents=0 HEAD)
fi

RANGE="$START_REF..$END_REF"

echo "## Changes"
git log --no-merges --pretty=format:"* %h %s (%an)" "$RANGE"
echo

CONTRIBUTORS=$(git log --format='%an' "$RANGE" | sort -u)
COUNT=$(echo "$CONTRIBUTORS" | wc -l | tr -d ' ')

echo "## Contributors"
echo "Total contributors: $COUNT"
echo "$CONTRIBUTORS" | sed 's/^/- /'
echo

STATS=$(git diff --shortstat "$RANGE")
FILES=$(echo "$STATS" | grep -o '[0-9]\+ file' | grep -o '[0-9]\+' || echo 0)
ADDED=$(echo "$STATS" | grep -o '\([0-9]\+\) insertions' | grep -o '[0-9]\+' || echo 0)
REMOVED=$(echo "$STATS" | grep -o '\([0-9]\+\) deletions' | grep -o '[0-9]\+' || echo 0)

echo "## Stats"
echo "- Files changed: $FILES"
echo "- Lines added: $ADDED"
echo "- Lines removed: $REMOVED"
