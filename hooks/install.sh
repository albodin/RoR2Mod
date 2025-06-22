#!/bin/bash
# Install git hooks for semantic versioning

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
GIT_HOOKS_DIR="$PROJECT_ROOT/.git/hooks"

echo "Installing git hooks for semantic versioning..."

# Create hooks directory if it doesn't exist
mkdir -p "$GIT_HOOKS_DIR"

# Copy hooks
for hook in pre-commit prepare-commit-msg post-commit; do
    if [ -f "$SCRIPT_DIR/$hook" ]; then
        cp "$SCRIPT_DIR/$hook" "$GIT_HOOKS_DIR/$hook"
        chmod +x "$GIT_HOOKS_DIR/$hook"
        echo "✓ Installed $hook"
    else
        echo "✗ Missing $hook in hooks directory"
    fi
done

# Make hooks executable
chmod +x "$GIT_HOOKS_DIR"/*

echo ""
echo "Git hooks installed successfully!"
echo "Semantic versioning will now work automatically for commits with:"
echo "  - 'fix:', 'patch:', 'bugfix:' → patch version bump"
echo "  - 'feat:', 'feature:', 'add:', 'added:' → minor version bump"
echo "  - 'BREAKING CHANGE' or '!:' → major version bump"