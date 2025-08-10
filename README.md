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
This repository omits generated build system files such as `configure` and `Makefile.in`.
Run `./autogen.sh` (or `autoreconf -i && ./configure`) to generate them before building:

Install the required development packages, including `libmicrohttpd-dev`
and `libpq-dev` on Debian/Ubuntu or `libmicrohttpd` and `postgresql`
via Homebrew on macOS, then run:

```
./autogen.sh
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
# log configuration
log_dir ./logs
log_max_size 1048576
log_retention 5
```

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

Optional flags control logging at startup:

```
./src/scastd --logging=false --logpath=/var/log/scastd scastd.conf &
```

`--logging` enables or disables file logging (default `true`), while
`--logpath` overrides the log directory. Send `SIGHUP` to the running
process to reload `scastd.conf`. Updated settings such as the log
directory or database credentials take effect without restarting.

API Versioning
--------------
HTTP endpoints are namespaced by API version (e.g., `/v1/status.json`).
Breaking changes will result in a new major version such as `/v2`. Older
versions may continue to be served for a transition period, but
unversioned paths are deprecated and may be removed in a future release.


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
