# Nanma NX-MediaSuite Versioning System

## Overview

This document describes the automated semantic versioning and tagging system for the Nanma NX-MediaSuite repository. The system ensures deterministic, explicit, and reviewable version management while respecting phase boundaries.

## Versioning Model

### Semantic Versioning Format

All versions follow the format: `vMAJOR.MINOR.PATCH`

Examples:
- `v1.0.0` - Initial release
- `v1.0.1` - Patch release
- `v1.1.0` - Minor release  
- `v2.0.0` - Major release

### Version Bump Rules

Version increments are **EXPLICIT ONLY** and driven by commit messages containing semver directives:

- `+semver: patch` → Increment PATCH version
- `+semver: minor` → Increment MINOR version, reset PATCH to 0
- `+semver: major` → Increment MAJOR version, reset MINOR and PATCH to 0

**Important**: If no semver directive is present in commits since the last version, the version will NOT change.

### Priority Rules

When multiple semver directives are present:
- `major` takes precedence over `minor` and `patch`
- `minor` takes precedence over `patch`
- Only the highest priority bump is applied

## Phase Tags vs Semantic Versions

### Phase Tags
- **Purpose**: Mark architectural phase boundaries (e.g., `phase-0-core-stable`)
- **Characteristics**:
  - Immutable once created
  - Never auto-incremented
  - Manually created for major architectural milestones
  - Protected from modification

### Semantic Version Tags
- **Purpose**: Track incremental releases and changes
- **Characteristics**:
  - Automatically generated based on commit messages
  - Follow semver conventions
  - Independent of phase boundaries
  - Used for CI/CD and release management

## Usage

### Requesting a Version Bump

Include a semver directive in your commit message:

```bash
# Patch release (bug fixes, small changes)
git commit -m "fix(core): resolve memory leak in result system

+semver: patch"

# Minor release (new features, backward compatible)
git commit -m "feat(api): add new validation functions

+semver: minor"

# Major release (breaking changes)
git commit -m "feat(core): redesign API contracts

BREAKING CHANGE: Function signatures changed

+semver: major"
```

### Checking Current Version

```bash
# Get current version
./scripts/compute-version.sh --current

# Check what the next version would be
./scripts/compute-version.sh

# Check required bump type
./scripts/compute-version.sh --bump-type
```

### Manual Tagging

```bash
# Create a tag (with safety checks)
./scripts/create-tag.sh

# Dry run to see what would happen
./scripts/create-tag.sh --dry-run

# Force tag creation (skip safety checks)
./scripts/create-tag.sh --force
```

## Scripts Reference

### `compute-version.sh`

Determines the next semantic version based on commit messages.

```bash
# Usage
./scripts/compute-version.sh [--help|--current|--bump-type]

# Options
--help      Show help message
--current   Show current version only  
--bump-type Show required bump type only
```

### `create-tag.sh`

Creates annotated Git tags with comprehensive safety checks.

```bash
# Usage  
./scripts/create-tag.sh [--dry-run] [--force] [--help]

# Options
--dry-run   Show what would be done without creating the tag
--force     Skip safety checks (not recommended)
--help      Show help message
```

## Safety Rules

### Mandatory Safety Checks

1. **Working Tree Must Be Clean**: No uncommitted changes allowed when tagging
2. **Explicit Intent Required**: No automatic version bumps without semver directives
3. **Tag Uniqueness**: Cannot create duplicate tags
4. **Phase Boundary Protection**: Phase tags cannot be modified or overwritten

### Error Conditions

The system will **FAIL LOUDLY** in these situations:
- Dirty working tree during tagging
- Missing or ambiguous semver directives
- Attempting to overwrite existing tags
- Phase boundary tag corruption
- Git repository inconsistencies

## Integration with NX-Core Architecture

The versioning system respects and enforces the architectural principles:

- **Deterministic**: Same commit history always produces same version
- **Explicit**: No hidden or automatic version changes
- **Immutable**: Phase boundaries cannot be altered
- **Auditable**: Complete history preserved in Git tags
- **Stable**: Version computation is reproducible across environments