#!/bin/bash
# Manual version bumping script

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

case "$1" in
    major)
        echo "Bumping major version..."
        python3 "$SCRIPT_DIR/version_bump.py" major
        ;;
    minor)
        echo "Bumping minor version..."
        python3 "$SCRIPT_DIR/version_bump.py" minor
        ;;
    patch)
        echo "Bumping patch version..."
        python3 "$SCRIPT_DIR/version_bump.py" patch
        ;;
    build)
        echo "Bumping build version..."
        python3 "$SCRIPT_DIR/version_bump.py" build
        ;;
    *)
        echo "Usage: $0 {major|minor|patch|build}"
        echo "Current version:"
        python3 "$SCRIPT_DIR/version_bump.py"
        exit 1
        ;;
esac

echo "Version bump complete!"