#!/bin/bash
set -e

# --- Configuration ---
readonly APP_ICON="assets/icon.png"
readonly DEFAULT_TARGET="bin/basic"

# --- Script ---
TARGET_FILE=""

# Create a temporary directory and set a trap to clean it up on exit.
readonly TMP_DIR=$(mktemp -d)
trap 'rm -rf "$TMP_DIR"' EXIT

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <target_file>"
    # If no argument, try to default to bin/basic if it exists
    if [ -f "$DEFAULT_TARGET" ]; then
        TARGET_FILE="$DEFAULT_TARGET"
        echo "No target specified, defaulting to $TARGET_FILE"
    else
        exit 1
    fi
else
    TARGET_FILE="$1"
fi

if [ ! -f "$APP_ICON" ]; then
    echo "Error: Icon file $APP_ICON not found."
    exit 1
fi

if [ ! -f "$TARGET_FILE" ]; then
    echo "Error: Target file $TARGET_FILE not found. Please build the project first."
    exit 1
fi

echo "Creating iconset..."
readonly ICONSET_DIR="$TMP_DIR/basic.iconset"
mkdir -p "$ICONSET_DIR"

# Resize images
sips -s format png -z 16 16     "$APP_ICON" --out "$ICONSET_DIR/icon_16x16.png" > /dev/null
sips -s format png -z 32 32     "$APP_ICON" --out "$ICONSET_DIR/icon_16x16@2x.png" > /dev/null
sips -s format png -z 32 32     "$APP_ICON" --out "$ICONSET_DIR/icon_32x32.png" > /dev/null
sips -s format png -z 64 64     "$APP_ICON" --out "$ICONSET_DIR/icon_32x32@2x.png" > /dev/null
sips -s format png -z 128 128   "$APP_ICON" --out "$ICONSET_DIR/icon_128x128.png" > /dev/null
sips -s format png -z 256 256   "$APP_ICON" --out "$ICONSET_DIR/icon_128x128@2x.png" > /dev/null
sips -s format png -z 256 256   "$APP_ICON" --out "$ICONSET_DIR/icon_256x256.png" > /dev/null
sips -s format png -z 512 512   "$APP_ICON" --out "$ICONSET_DIR/icon_256x256@2x.png" > /dev/null
sips -s format png -z 512 512   "$APP_ICON" --out "$ICONSET_DIR/icon_512x512.png" > /dev/null
sips -s format png -z 1024 1024 "$APP_ICON" --out "$ICONSET_DIR/icon_512x512@2x.png" > /dev/null

echo "Converting to icns..."
readonly ICNS_FILE="$TMP_DIR/Icon.icns"
iconutil -c icns "$ICONSET_DIR" -o "$ICNS_FILE"

echo "Setting icon for $TARGET_FILE..."
# Use Swift to set the icon (standard macOS way for files)
swift - "$TARGET_FILE" "$ICNS_FILE" <<EOF
import Cocoa

let args = CommandLine.arguments
let filePath = args[1]
let iconPath = args[2]

let fileURL = URL(fileURLWithPath: filePath)
let iconImage = NSImage(contentsOfFile: iconPath)

if let icon = iconImage {
    if NSWorkspace.shared.setIcon(icon, forFile: fileURL.path) {
        print("Successfully set icon for \(filePath)")
        exit(0)
    } else {
        print("Failed to set icon.")
        exit(1)
    }
} else {
    print("Could not load icon from \(iconPath)")
    exit(1)
}
EOF

# Verify success
if [ $? -eq 0 ]; then
    echo "Icon set successfully."
else
    echo "Failed to set icon."
fi
