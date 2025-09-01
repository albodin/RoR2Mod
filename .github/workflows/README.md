# GitHub Actions Workflows

This directory contains automated workflows for building and releasing RoR2Mod.

## Workflows

### build.yml - Continuous Integration
- **Triggers**: Push to master, pull requests, manual dispatch
- **Actions**:
  - Builds the mod and injector
  - Creates versioned artifacts with commit info (e.g., `1.14.1-abc1234-b42`)
  - Updates the "latest" pre-release on every push to master
  - Version format: `{version}-{commit}-b{build_number}`

### release.yml - Tagged Releases
- **Triggers**: Push tags starting with 'v' (e.g., v1.14.2), manual dispatch
- **Actions**:
  - Builds release version (optimized)
  - Creates a distribution ZIP package
  - Generates changelog from previous tag
  - Creates a formal GitHub release

## Usage

### Automatic Builds
Every push to master automatically:
1. Builds the project
2. Updates the "latest" pre-release
3. Artifacts include version, commit hash, and build number

### Creating a Release
1. Update version in `src/version.hpp`
2. Commit and push changes
3. Create and push a tag:
   ```bash
   git tag v1.14.2
   git push origin v1.14.2
   ```
4. The release workflow will automatically create a GitHub release

### Manual Trigger
Both workflows can be triggered manually from GitHub Actions tab:
- **build.yml**: Just click "Run workflow"
- **release.yml**: Provide a tag name (e.g., v1.14.2)

## Version Information
- Base version is read from `src/version.hpp`
- Build artifacts include commit hash and build number
- Release versions match the tag name
