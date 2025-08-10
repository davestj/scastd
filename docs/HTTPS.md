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
