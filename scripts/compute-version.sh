#!/bin/bash
set -euo pipefail

# Nanma NX-MediaSuite - Semantic Version Computation Script
# Determines next version based on commit messages with +semver directives

# Configuration
SEMVER_PATTERN="v[0-9]+\.[0-9]+\.[0-9]+$"
MAJOR_PATTERN="\+semver:\s*major"
MINOR_PATTERN="\+semver:\s*minor"
PATCH_PATTERN="\+semver:\s*patch"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1" >&2
}

# Get the latest semantic version tag
get_latest_version() {
    latest_tag=$(git tag -l | grep -E "$SEMVER_PATTERN" || true)
    latest_tag=$(echo "$latest_tag" | sort -V | tail -n1)
    
    if [[ -z "$latest_tag" ]]; then
        echo "v0.0.0"
    else
        echo "$latest_tag"
    fi
}

# Parse version components
parse_version() {
    local version="$1"
    # Remove 'v' prefix and split by dots
    version="${version#v}"
    echo "$version" | tr '.' ' '
}

# Get commits since last version tag
get_commits_since_version() {
    local version_tag="$1"
    
    if [[ "$version_tag" == "v0.0.0" ]]; then
        # No previous version, get all commits
        git log --oneline --no-merges
    else
        # Get commits since the version tag
        if git rev-parse "$version_tag" >/dev/null 2>&1; then
            git log "${version_tag}..HEAD" --oneline --no-merges
        else
            log_error "Version tag $version_tag not found"
            exit 1
        fi
    fi
}

# Determine required version bump from commits
determine_bump() {
    local commits="$1"
    local has_major=false
    local has_minor=false
    local has_patch=false
    local bump_reasons=()
    
    while IFS= read -r commit; do
        if [[ -z "$commit" ]]; then
            continue
        fi
        
        if echo "$commit" | grep -qiE "$MAJOR_PATTERN"; then
            has_major=true
            bump_reasons+=("MAJOR: $commit")
        elif echo "$commit" | grep -qiE "$MINOR_PATTERN"; then
            has_minor=true
            bump_reasons+=("MINOR: $commit")
        elif echo "$commit" | grep -qiE "$PATCH_PATTERN"; then
            has_patch=true
            bump_reasons+=("PATCH: $commit")
        fi
    done <<< "$commits"
    
    # Determine highest priority bump
    if [[ "$has_major" == true ]]; then
        echo "major"
    elif [[ "$has_minor" == true ]]; then
        echo "minor"
    elif [[ "$has_patch" == true ]]; then
        echo "patch"
    else
        echo "none"
    fi
    
    # Output reasons to stderr for logging
    if [[ ${#bump_reasons[@]} -gt 0 ]]; then
        log_info "Version bump reasons:"
        printf '%s\n' "${bump_reasons[@]}" >&2
    fi
}

# Calculate next version
calculate_next_version() {
    local current_version="$1"
    local bump_type="$2"
    
    read -r major minor patch <<< "$(parse_version "$current_version")"
    
    case "$bump_type" in
        "major")
            major=$((major + 1))
            minor=0
            patch=0
            ;;
        "minor")
            minor=$((minor + 1))
            patch=0
            ;;
        "patch")
            patch=$((patch + 1))
            ;;
        "none")
            # No change
            ;;
        *)
            log_error "Invalid bump type: $bump_type"
            exit 1
            ;;
    esac
    
    echo "v${major}.${minor}.${patch}"
}

# Main execution
main() {
    log_info "Computing next semantic version..."
    
    # Ensure we're in a git repository
    if ! git rev-parse --git-dir >/dev/null 2>&1; then
        log_error "Not in a git repository"
        exit 1
    fi
    
    # Get current version
    local current_version
    current_version=$(get_latest_version)
    log_info "Current version: $current_version"
    
    # Get commits since last version
    local commits
    commits=$(get_commits_since_version "$current_version")
    
    if [[ -z "$commits" ]]; then
        log_info "No commits since last version"
        echo "$current_version"
        exit 0
    fi
    
    log_info "Commits since $current_version:"
    echo "$commits" | head -5 >&2
    if [[ $(echo "$commits" | wc -l) -gt 5 ]]; then
        echo "... and $(($(echo "$commits" | wc -l) - 5)) more" >&2
    fi
    
    # Determine required bump
    local bump_type
    bump_type=$(determine_bump "$commits")
    
    if [[ "$bump_type" == "none" ]]; then
        log_warn "No semver directive found in commits since $current_version"
        log_warn "Version remains: $current_version"
        echo "$current_version"
        exit 0
    fi
    
    # Calculate next version
    local next_version
    next_version=$(calculate_next_version "$current_version" "$bump_type")
    
    log_info "Bump type: $bump_type"
    log_info "Next version: $next_version"
    
    echo "$next_version"
}

# Handle command line arguments
case "${1:-}" in
    "--help"|"-h")
        echo "Usage: $0 [--help|--current|--bump-type]"
        echo ""
        echo "Computes the next semantic version based on commit messages."
        echo ""
        echo "Options:"
        echo "  --help      Show this help message"
        echo "  --current   Show current version only"
        echo "  --bump-type Show required bump type only"
        echo ""
        echo "Semver directives in commit messages:"
        echo "  +semver: patch  - Increment patch version"
        echo "  +semver: minor  - Increment minor version"
        echo "  +semver: major  - Increment major version"
        exit 0
        ;;
    "--current")
        get_latest_version
        exit 0
        ;;
    "--bump-type")
        current_version=$(get_latest_version)
        commits=$(get_commits_since_version "$current_version")
        determine_bump "$commits" 2>/dev/null
        exit 0
        ;;
    "")
        main
        ;;
    *)
        log_error "Unknown option: $1"
        echo "Use --help for usage information"
        exit 1
        ;;
esac