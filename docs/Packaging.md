# Packaging

## Release target

The top-level `Makefile` provides a convenience target that builds a
platform-appropriate package. Specify the version and target distribution on the command line:

```bash
make release VERSION=1.0 DISTRO=bookworm
```

On Linux this runs `packaging/debian/build_deb.sh` and produces a `.deb` in `dist/`.
On macOS it executes `packaging/macos/build_pkg.sh` to generate both a `.pkg` installer and a Homebrew formula in `dist/`.

## Debian/Ubuntu

The `packaging/debian` directory provides a helper script and systemd unit file.

```bash
./packaging/debian/build_deb.sh 1.0 bookworm
```

The script compiles the project, stages the install tree and produces a `.deb` such as `scastd_1.0_bookworm_amd64.deb` inside `dist/`.
Install the included `scastd.service` to `/etc/systemd/system` and enable it with:

```bash
sudo systemctl enable --now scastd
```

## macOS

### Homebrew Formula

The preferred method for installing SCASTD on macOS is via Homebrew using the included formula:

```bash
# Install from local formula (for development/testing)
brew install --formula packaging/macos/Formula/scastd.rb

# Or install from a custom tap (when published)
brew tap davestj/scastd
brew install scastd
```

The Homebrew formula automatically:
- Installs all required dependencies (libxml2, libcurl, MySQL client, PostgreSQL, etc.)
- Configures the build for ARM64 macOS with proper library paths
- Sets up configuration files in `/opt/homebrew/etc/scastd/`
- Creates log directory at `/opt/homebrew/var/log/scastd/`
- Provides service management via `brew services`

#### Managing the Service

```bash
# Start the service
brew services start scastd

# Stop the service
brew services stop scastd

# Restart the service
brew services restart scastd

# View service status
brew services list | grep scastd
```

#### Configuration

Configuration files are installed to `/opt/homebrew/etc/scastd/`:
- `scastd.conf.example` - Example configuration template
- `scastd.db` - SQLite database (initialized automatically)

Copy the example configuration and customize as needed:

```bash
cp /opt/homebrew/etc/scastd/scastd.conf.example /opt/homebrew/etc/scastd/scastd.conf
# Edit configuration as needed
nano /opt/homebrew/etc/scastd/scastd.conf
```

### Traditional Package Installation

The `packaging/macos` directory also includes a `build_pkg.sh` script and a launchd property list for traditional installation:

```bash
./packaging/macos/build_pkg.sh 1.0
```

This generates both:
- `dist/scastd-1.0.pkg` - macOS installer package using `pkgbuild`
- `dist/scastd.rb` - Homebrew formula with calculated SHA256

For the traditional installer, copy `org.scastd.daemon.plist` to `/Library/LaunchDaemons` and load it:

```bash
sudo launchctl load -w /Library/LaunchDaemons/org.scastd.daemon.plist
```

### Homebrew Formula Generation

The build process automatically generates a Homebrew formula with the correct SHA256 checksum for the release tarball. The formula is created by:

1. `packaging/macos/build_formula.sh` - Standalone script for formula generation
2. Integrated into `packaging/macos/build_pkg.sh` for the complete release process

The generated formula supports:
- ARM64 macOS builds with proper dependency paths
- Automatic service management via `brew services`
- Proper configuration file and database setup
- Comprehensive test suite validation
