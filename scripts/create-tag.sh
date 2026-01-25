#!/bin/bash
set -euo pipefail

# Nanma NX-MediaSuite - Git Tagging Script
# Creates annotated semantic version tags with safety checks

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
COMPUTE_VERSION_SCRIPT="$SCRIPT_DIR/compute-version.sh"

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

# Check if working tree is clean
check_working_tree() {
    if ! git diff-index --quiet HEAD --; then
        log_error "Working tree is dirty. Commit or stash changes before tagging."
        git status --porcelain
        exit 1
    fi
    
    if [[ -n "$(git ls-files --others --exclude-standard)" ]]; then
        log_error "Untracked files present. Add or ignore them before tagging."
        git ls-files --others --exclude-standard
        exit 1
    fi
}

# Check if we're on the main branch
check_branch() {
    local current_branch
    current_branch=$(git branch --show-current)
    
    if [[ "$current_branch" != "main" && "$current_branch" != "master" ]]; then
        log_warn "Not on main/master branch (current: $current_branch)"
        read -p "Continue anyway? [y/N]: " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            log_info "Tagging cancelled"
            exit 0
        fi
    fi
}

# Check if tag already exists
check_tag_exists() {
    local tag="$1"
    
    if git rev-parse "$tag" >/dev/null 2>&1; then
        log_error "Tag $tag already exists"
        log_error "Use 'git tag -d $tag' to delete it first (if you're sure)"
        exit 1
    fi
}

# Get commit information for tag annotation
get_commit_info() {
    local current_version="$1"
    local next_version="$2"
    
    echo "Version: $next_version"
    echo "Previous: $current_version"
    echo "Commit: $(git rev-parse HEAD)"
    echo "Date: $(date -u +"%Y-%m-%d %H:%M:%S UTC")"
    echo ""
    echo "Changes since $current_version:"
    
    if [[ "$current_version" == "v0.0.0" ]]; then
        git log --oneline --no-merges | head -10
    else
        git log "${current_version}..HEAD" --oneline --no-merges
    fi
}

# Create annotated tag
create_tag() {
    local tag="$1"
    local annotation="$2"
    
    log_info "Creating annotated tag: $tag"
    
    # Create temporary file for annotation
    local temp_file
    temp_file=$(mktemp)
    echo "$annotation" > "$temp_file"
    
    # Create the tag
    if git tag -a "$tag" -F "$temp_file"; then
        log_info "Tag $tag created successfully"
        log_info "Push with: git push origin $tag"
    else
        log_error "Failed to create tag $tag"
        rm -f "$temp_file"
        exit 1
    fi
    
    rm -f "$temp_file"
}

# Main execution
main() {
    local dry_run=false
    local force=false
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --dry-run)
                dry_run=true
                shift
                ;;
            --force)
                force=true
                shift
                ;;
            --help|-h)
                echo "Usage: $0 [--dry-run] [--force] [--help]"
                echo ""
                echo "Creates an annotated Git tag for the next semantic version."
                echo ""
                echo "Options:"
                echo "  --dry-run   Show what would be done without creating the tag"
                echo "  --force     Skip safety checks (not recommended)"
                echo "  --help      Show this help message"
                echo ""
                echo "Safety checks:"
                echo "  - Working tree must be clean"
                echo "  - Must have semver directive in recent commits"
                echo "  - Tag must not already exist"
                exit 0
                ;;
            *)
                log_error "Unknown option: $1"
                echo "Use --help for usage information"
                exit 1
                ;;
        esac
    done
    
    log_info "Preparing to create semantic version tag..."
    
    # Ensure we're in a git repository
    if ! git rev-parse --git-dir >/dev/null 2>&1; then
        log_error "Not in a git repository"
        exit 1
    fi
    
    # Check if compute-version script exists
    if [[ ! -x "$COMPUTE_VERSION_SCRIPT" ]]; then
        log_error "Version computation script not found or not executable: $COMPUTE_VERSION_SCRIPT"
        exit 1
    fi
    
    # Safety checks (unless forced)
    if [[ "$force" != true ]]; then
        check_working_tree
        check_branch
    fi
    
    # Get current and next version
    local current_version next_version bump_type
    current_version=$("$COMPUTE_VERSION_SCRIPT" --current)
    bump_type=$("$COMPUTE_VERSION_SCRIPT" --bump-type)
    next_version=$("$COMPUTE_VERSION_SCRIPT")
    
    log_info "Current version: $current_version"
    log_info "Bump type: $bump_type"
    log_info "Next version: $next_version"
    
    # Check if version bump is needed
    if [[ "$bump_type" == "none" ]]; then
        log_error "No semver directive found in commits since $current_version"
        log_error "Add a commit with +semver: patch|minor|major to trigger version bump"
        exit 1
    fi
    
    # Check if version actually changed
    if [[ "$current_version" == "$next_version" ]]; then
        log_info "Version unchanged: $current_version"
        exit 0
    fi
    
    # Check if tag already exists
    check_tag_exists "$next_version"
    
    # Prepare tag annotation
    local annotation
    annotation=$(get_commit_info "$current_version" "$next_version")
    
    if [[ "$dry_run" == true ]]; then
        log_info "DRY RUN - Would create tag: $next_version"
        echo ""
        echo "Tag annotation:"
        echo "==============="
        echo "$annotation"
        exit 0
    fi
    
    # Confirm before creating tag
    echo ""
    echo "Tag annotation preview:"
    echo "======================="
    echo "$annotation" | head -15
    echo ""
    
    read -p "Create tag $next_version? [y/N]: " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        log_info "Tagging cancelled"
        exit 0
    fi
    
    # Create the tag
    create_tag "$next_version" "$annotation"
}

main "$@"