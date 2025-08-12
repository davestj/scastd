/* Scast Daemon
Authors: oddsock, dstjohn
This program is free software; you can
redistribute it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your option) any later
version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details. You should have received a copy of the GNU General
Public License along with this program; if not, write to the Free Software Foundation, Inc., 59
Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

Overview
--------
scastd is a small daemon for collecting Icecast2 stream statistics and
publishing them over HTTP.  It polls an Icecast2 server for current
listener information, stores the results in a database, and exposes a
lightweight web service built with `libmicrohttpd`.  The HTTP server
listens on port **8333** and serves JSON or XML status responses at
`/v1/status.json` and `/v1/status.xml` (unversioned paths remain for
backward compatibility).  Database backends for MariaDB/MySQL
and PostgreSQL are currently supported.

Building
--------
This repository omits generated build system files such as `configure` and
`Makefile.in`. Run `./autogen.sh` (or `autoreconf -i`) to generate them before
building. Platform specific examples are shown below.

### Debian/Ubuntu

Install the required development packages and build the project:

```
sudo apt-get update && sudo apt-get install -y build-essential autoconf automake \
  libtool pkg-config libxml2-dev libcurl4-openssl-dev libmysqlclient-dev \
  libpq-dev libmicrohttpd-dev
./autogen.sh
./configure
make
```

### macOS arm64

Use Homebrew to install dependencies and set `PKG_CONFIG_PATH` so `pkg-config`
can locate the libraries:

```
brew update
brew install autoconf automake libtool pkg-config libxml2 curl mysql-client libpq libmicrohttpd
export PKG_CONFIG_PATH=/opt/homebrew/opt/libpq/lib/pkgconfig:/opt/homebrew/opt/mysql-client/lib/pkgconfig:/opt/homebrew/opt/libxml2/lib/pkgconfig:$PKG_CONFIG_PATH
./autogen.sh
./configure
make
```

Configuration
-------------
scastd reads database settings from a simple configuration file at
startup. The file contains lines of the form `key value` and supports
`#` comments. By default the daemon looks for `scastd.conf` in the
current directory, but an alternate path may be supplied as the first
command line argument.

Example `scastd.conf`:

```
# database credentials
username root
password secret
# poll interval between server checks (ms, s, m, h)
poll_interval 5m
# log configuration
log_dir ./logs
log_max_size 1048576
log_retention 5
access_log access.log
error_log error.log
debug_log debug.log
debug_level 1
syslog_enabled false
syslog_host localhost
syslog_port 514
syslog_protocol udp
```

`poll_interval` controls how often the daemon polls the Icecast server. The value
accepts `ms`, `s`, `m`, or `h`/`hr` suffixes (e.g., `600ms`, `60s`, `5m`, `1h`) and
defaults to 60 seconds.

The log settings allow fine-grained control over where messages are
written. `access_log`, `error_log`, and `debug_log` may be absolute
paths or names relative to `log_dir` and default to `access.log`,
`error.log`, and `debug.log` respectively. `log_max_size` controls the
maximum size in bytes before a log file is rotated, while
`log_retention` specifies how many rotated logs are retained. `debug_level`
sets the verbosity for the debug log from 1 (least verbose) to 4 (most verbose).
When `syslog_enabled` is `true`, log messages are also forwarded to a
remote syslog server defined by `syslog_host`, `syslog_port` (default
`514`), and `syslog_protocol` (`udp` or `tcp`).

For security, make the configuration file readable only by your user:

```
chmod 600 scastd.conf
```

The `username` and `password` settings may be supplied via environment
variables. `SCASTD_USERNAME` and `SCASTD_PASSWORD` override values in the
configuration file. Alternatively, `SCASTD_USERNAME_FILE` or
`SCASTD_PASSWORD_FILE` can point to files containing the corresponding
credential. The configuration file also supports `username_file` and
`password_file` keys. Any secrets file should also be protected with
restrictive permissions, for example `chmod 600 password.txt`.

### HTTPS

The built-in status server can optionally serve HTTPS. Enable TLS and
point the daemon to your certificate and private key:

```
ssl_enabled true
ssl_cert /etc/scastd/server.crt
ssl_key /etc/scastd/server.key
```

For local testing a self-signed certificate may be generated:

```
scripts/gen_self_signed_cert.sh /tmp/certs
```

This creates `scastd.crt` and `scastd.key` with subject alternative
names for `127.0.0.1` and `0.0.0.0`. Self-signed certificates are
convenient for development but **must not** be used in production.
Always secure the private key and obtain a certificate from a trusted
authority for real deployments.

Usage
-----
Start the daemon with your configuration file and query the HTTP
endpoint:

```
./src/scastd scastd.conf &
curl http://localhost:8333/v1/status.json
```

Send `SIGHUP` to the running process to reload `scastd.conf`. Updated
settings such as the log directory or database credentials take effect
without restarting.

API Versioning
--------------
HTTP endpoints are namespaced by API version (e.g., `/v1/status.json`).
Breaking changes will result in a new major version such as `/v2`. Older
versions may continue to be served for a transition period, but
unversioned paths are deprecated and may be removed in a future release.

Continuous Integration and Releases
-----------------------------------
The project is built and tested by GitHub Actions on the following
platforms:

- macOS 14 (arm64)
- Debian 12
- Ubuntu 22.04
- Ubuntu 24.04

Each job bootstraps the build with `./autogen.sh`, configures, compiles
and executes the test suite via `make check`.  After all tests and lint
jobs succeed, a release job tags the commit on `master` as
`v<run_number>` and publishes a GitHub release.

Required secrets and environment variables are detailed in
[docs/CISecrets.md](docs/CISecrets.md).  The complete CI workflow is
described in [docs/CI.md](docs/CI.md).


Troubleshooting
---------------
If the build fails, common issues and fixes include:

- `aclocal`: command not found – install the `automake` package.
- `AM_PROG_LIBTOOL` missing – install `libtool`.
- Missing `mariadb/mysql.h` or `mysql.h` – install `libmariadb-dev` and `libmariadb-dev-compat`.
- `microhttpd.h` missing – install `libmicrohttpd-dev`.

After installing the required packages, rerun `./autogen.sh`,
`./configure CXXFLAGS='-std=c++17 -g -O2'`, and `make`.  Tests (if any)
can be executed with `make check`.

Contributing
------------

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines on coding style, branch naming,
commit conventions, and the pull request process.
