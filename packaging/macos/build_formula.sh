#!/bin/bash
set -euo pipefail

VERSION=${1:-1.0}
REPO_URL=${2:-"https://github.com/davestj/scastd"}
FORMULA_TEMPLATE="packaging/macos/Formula/scastd.rb"
DIST_DIR="dist"

# Create dist directory if it doesn't exist
mkdir -p "$DIST_DIR"

# Function to calculate SHA256 for a given URL/version
calculate_sha256() {
    local version="$1"
    local url="$2"
    local temp_file
    
    temp_file=$(mktemp)
    
    echo "Calculating SHA256 for ${url}/archive/v${version}.tar.gz..."
    
    # Try to download and calculate SHA256
    if curl -sL "${url}/archive/v${version}.tar.gz" -o "$temp_file"; then
        sha256sum "$temp_file" | cut -d' ' -f1
        rm -f "$temp_file"
    else
        echo "Warning: Could not download tarball to calculate SHA256"
        echo "PLACEHOLDER_SHA256"
        rm -f "$temp_file"
    fi
}

# Generate the formula with proper SHA256
generate_formula() {
    local version="$1"
    local repo_url="$2"
    local sha256
    
    echo "Generating Homebrew formula for version $version..."
    
    # Calculate SHA256 for the release tarball
    sha256=$(calculate_sha256 "$version" "$repo_url")
    
    # Create the formula by substituting placeholders
    sed -e "s|PLACEHOLDER_SHA256|$sha256|g" \
        -e "s|v1.0|v$version|g" \
        -e "s|https://github.com/davestj/scastd|$repo_url|g" \
        "$FORMULA_TEMPLATE" > "$DIST_DIR/scastd.rb"
    
    echo "Formula generated: $DIST_DIR/scastd.rb"
    echo "SHA256: $sha256"
}

# Main execution
main() {
    echo "Building Homebrew formula for SCASTD v$VERSION"
    
    # Check if formula template exists
    if [[ ! -f "$FORMULA_TEMPLATE" ]]; then
        echo "Error: Formula template not found at $FORMULA_TEMPLATE"
        exit 1
    fi
    
    # Generate the formula
    generate_formula "$VERSION" "$REPO_URL"
    
    echo "Homebrew formula build completed successfully!"
    echo "Formula location: $DIST_DIR/scastd.rb"
    echo ""
    echo "To test the formula locally:"
    echo "  brew install --formula $DIST_DIR/scastd.rb"
    echo ""
    echo "To publish to a Homebrew tap:"
    echo "  1. Copy $DIST_DIR/scastd.rb to your tap repository"
    echo "  2. Commit and push to your tap"
    echo "  3. Users can install with: brew tap your-github-username/scastd && brew install scastd"
}

# Run main function
main "$@"