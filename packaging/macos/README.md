# macOS Packaging

## build_pkg.sh

`build_pkg.sh` builds a signed macOS installer package and generates a matching Homebrew formula. It performs a full build of SCASTD, stages the files under `pkg_build/`, and uses `pkgbuild` to produce `dist/scastd-<version>.pkg`. After the package is created it invokes `build_formula.sh` to refresh `dist/scastd.rb`.

Run it when preparing a release:

```bash
./packaging/macos/build_pkg.sh <version>
```

The resulting `.pkg` and formula should be attached to the GitHub release.

## build_formula.sh

`build_formula.sh` creates a Homebrew formula from the template in `Formula/scastd.rb`. It calculates the SHA256 of the release tarball and writes the finished formula to `dist/scastd.rb`.

```bash
./packaging/macos/build_formula.sh <version> [repo_url]
```

This script runs automatically from `build_pkg.sh` but can also be called on its own when only the formula needs to be updated.

## Publishing the formula to a tap

1. Clone or create a Homebrew tap repository such as `githubuser/homebrew-scastd`.
2. Copy `dist/scastd.rb` into the tap's `Formula/` directory.
3. Commit and push the change.
4. Users can then install with:
   
   ```bash
   brew tap githubuser/scastd
   brew install scastd
   ```

These scripts allow the macOS package and Homebrew formula to be generated as part of the release workflow and distributed through your tap.
