# Version Management for RoR2Mod

This project uses **Semantic Versioning** with automatic detection based on commit messages.

## Version Format: `MAJOR.MINOR.PATCH`

- **MAJOR**: Breaking changes, incompatible API changes
- **MINOR**: New features, backwards compatible
- **PATCH**: Bug fixes, backwards compatible
- **BUILD**: Internal build number (not shown to users)

## Setup

The automatic version bumping requires git hooks to be installed. Run this command after cloning:

```bash
./hooks/install.sh
```

This installs the necessary git hooks.

## Automatic Version Bumping

The version is automatically updated based on keywords in your commit message:

### Major Version (Breaking Changes)
Triggered by:
- `BREAKING CHANGE` anywhere in the message
- `!:` in the commit type (e.g., `feat!: complete rewrite`)

Example:
```
feat!: Remove old API methods
BREAKING CHANGE: Old save files are no longer compatible
```

### Minor Version (New Features)
Triggered by:
- `feat:` or `feature:` at the start
- `add:` or `added:` at the start
- Keywords: `feature`, `add`, `adds`, `implement`, `new`

Example:
```
feat: Add portal forcing for all portals
add: Implement automatic teleporter charging
```

### Patch Version (Bug Fixes)
Triggered by:
- `fix:` or `fixed:` at the start
- `patch:` or `bugfix:` at the start
- Keywords: `fix`, `fixed`, `bugfix`, `patch`, `repair`

Example:
```
fix: Correct portal spawner array access
bugfix: Resolve crash when teleporter is null
```

### No Version Change
For regular commits (refactoring, docs, tests), no version change occurs:
```
refactor: Clean up hook implementation
docs: Update README
test: Add unit tests for world module
```

## Manual Version Bumping

You can also manually bump versions:

```bash
# Bump versions manually
./scripts/bump_version.sh major   # 1.0.0 → 2.0.0
./scripts/bump_version.sh minor   # 1.0.0 → 1.1.0
./scripts/bump_version.sh patch   # 1.0.0 → 1.0.1

# For CI/CD builds only
./scripts/bump_version.sh build   # Updates build number only
```

## Examples

```bash
# This will bump to 1.1.0:
git commit -m "feat: Add instant teleporter option"

# This will bump to 1.0.1:
git commit -m "fix: Prevent crash on null reference"

# This will stay at current version:
git commit -m "refactor: Improve code structure"

# This will bump to 2.0.0:
git commit -m "feat!: Complete mod rewrite due to large game update"
```

## Notes

- Version changes are automatic via git pre-commit hook
- The version file (`src/version.h`) is automatically staged
- Users see clean versions like "1.2.3" in the mod UI
- Build numbers are tracked internally but not displayed