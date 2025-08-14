# Packaging

## Release target

The top-level `Makefile` provides a convenience target that builds a
platform-appropriate package. Specify the version on the command line:

```bash
make release VERSION=1.0
```

On Linux this runs `packaging/debian/build_deb.sh` and produces a `.deb`.
On macOS it executes `packaging/macos/build_pkg.sh` to generate a `.pkg`.

## Debian/Ubuntu

The `packaging/debian` directory provides a helper script and systemd unit file.

```bash
./packaging/debian/build_deb.sh 1.0
```

The script compiles the project, stages the install tree and produces a `.deb` such as `scastd_1.0_amd64.deb`.
Install the included `scastd.service` to `/etc/systemd/system` and enable it with:

```bash
sudo systemctl enable --now scastd
```

## macOS

The `packaging/macos` directory includes a `build_pkg.sh` script and a launchd property list.

```bash
./packaging/macos/build_pkg.sh 1.0
```

This generates `scastd-1.0.pkg` using `pkgbuild`.
Copy `org.scastd.daemon.plist` to `/Library/LaunchDaemons` and load it:

```bash
sudo launchctl load -w /Library/LaunchDaemons/org.scastd.daemon.plist
```
