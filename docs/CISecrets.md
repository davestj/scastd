# CI Secrets and Environment Variables

The GitHub Actions workflow relies on several secrets that must be set in
the repository's settings under **Settings > Secrets and variables >
Actions**. These secrets are exposed to jobs as environment variables.

| Secret | Description |
| ------ | ----------- |
| `DB_USER` | Username for database authentication. |
| `DB_PASSWORD` | Password for `DB_USER`. |
| `SQLITE_DB_PATH` | Path to a SQLite database used for tests. |
| `SSL_CERT` | PEM-encoded TLS certificate. |
| `SSL_KEY` | Private key for `SSL_CERT`. |

Populate these secrets before running the CI workflow to prevent failures
due to missing configuration.  For local development the same variables
can be exported directly.

## Environment Variables

Additional environment variables influence local and CI builds:

| Variable | Purpose |
| -------- | ------- |
| `SCASTD_USERNAME` / `SCASTD_USERNAME_FILE` | Override or provide the username used by the daemon. |
| `SCASTD_PASSWORD` / `SCASTD_PASSWORD_FILE` | Override or provide the password used by the daemon. |
| `PKG_CONFIG_PATH` | On macOS, helps `pkg-config` locate Homebrew-installed libraries. |
| `GITHUB_TOKEN` | Automatically provided in CI for release tagging. |
