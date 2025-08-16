# HTTPS with Let's Encrypt

The `scripts/setup_certbot.sh` script installs [Certbot](https://certbot.eff.org/),
obtains an initial TLS certificate, and configures automatic renewal.

## Usage

```bash
sudo scripts/setup_certbot.sh -e admin@example.com -d example.com [-d www.example.com]
```

Run the script as `root` (or with `sudo`) so it can install packages and write
to `/etc/letsencrypt/`. Supply one or more `-d`/`--domain` options and a single
`-e`/`--email` address for Let's Encrypt registration.

On macOS the script uses Homebrew to install Certbot and registers the renewal
service with `brew services start certbot`.

On Debian or Ubuntu systems, `apt` installs Certbot and a `systemd` timer is
enabled via `systemctl enable --now certbot.timer`.

Certificates and keys are stored under `/etc/letsencrypt/`.

## Certificate locations

Certbot writes certificates to different directories depending on the
operating system:

| Platform | Default path |
| --- | --- |
| Debian/Ubuntu | `/etc/letsencrypt/live/<domain>/` |
| macOS (Intel) | `/usr/local/etc/letsencrypt/live/<domain>/` |
| macOS (Apple Silicon) | `/opt/homebrew/etc/letsencrypt/live/<domain>/` |

Each directory contains `fullchain.pem` (certificate) and
`privkey.pem` (private key).

## Configure `scastd`

Reference these files in `scastd.conf` to enable HTTPS:

### Debian/Ubuntu

```conf
ssl_enable     true
ssl_cert       /etc/letsencrypt/live/example.com/fullchain.pem
ssl_key        /etc/letsencrypt/live/example.com/privkey.pem
```

### macOS (Intel)

```conf
ssl_enable     true
ssl_cert       /usr/local/etc/letsencrypt/live/example.com/fullchain.pem
ssl_key        /usr/local/etc/letsencrypt/live/example.com/privkey.pem
```

On Apple Silicon replace `/usr/local` with `/opt/homebrew`.

## Restart the daemon

Reload or restart SCASTD after installing or renewing certificates:

```bash
sudo systemctl restart scastd      # Debian/Ubuntu
brew services restart scastd       # macOS
```

## Verify TLS

Confirm the server responds over HTTPS:

```bash
curl https://example.com:8000/v1/status.json
```

The command should return the service status without TLS errors.
