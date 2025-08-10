# CI Secrets

The GitHub Actions workflow relies on several secrets that must be set in the repository's settings under **Settings > Secrets and variables > Actions**.

| Secret | Description |
| ------ | ----------- |
| `DB_USER` | Username for database authentication. |
| `DB_PASSWORD` | Password for `DB_USER`. |
| `SQLITE_DB_PATH` | Path to a SQLite database used for tests. |
| `SSL_CERT` | PEM-encoded TLS certificate. |
| `SSL_KEY` | Private key for `SSL_CERT`. |

Populate these secrets before running the CI workflow to prevent failures due to missing configuration.
