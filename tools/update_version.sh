#!/bin/bash
set -e

# Get the latest tag
VERSION=$(git describe --tags --abbrev=0 2>/dev/null || echo "UNKNOWN")

if [ "$VERSION" = "UNKNOWN" ]; then
    echo "No git tag found. Skipping README update."
    exit 0
fi

# Prepend 'v' if not present (e.g. if tag is '0.8', make it 'v0.8')
if [[ "$VERSION" != v* ]]; then
    DISPLAY_VERSION="v$VERSION"
else
    DISPLAY_VERSION="$VERSION"
fi

TARGET_FILE="README.md"

if [ ! -f "$TARGET_FILE" ]; then
    echo "Error: $TARGET_FILE not found."
    exit 1
fi

echo "Updating $TARGET_FILE version to $DISPLAY_VERSION..."

# Use a local temporary file to avoid permission issues with /tmp in some environments
TMP_FILE="${TARGET_FILE}.tmp"
sed "s/^# BASIC Interpreter.*/# BASIC Interpreter $DISPLAY_VERSION/" "$TARGET_FILE" > "$TMP_FILE"
mv "$TMP_FILE" "$TARGET_FILE"

echo "Success: $TARGET_FILE updated."
