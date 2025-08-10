# Continuous Integration

## Matrix

The GitHub Actions workflow tests the project on the following platforms:

| Runner | Architecture |
| ------ | ------------ |
| macOS 14 | arm64 |
| Debian 12 | x86_64 |
| Ubuntu 22.04 | x86_64 |
| Ubuntu 24.04 | x86_64 |

## Bootstrap Steps

Each matrix job performs the same bootstrap and build procedure:

```
./autogen.sh
./configure
make
make check
```

## Release Tagging

When pushes land on the `master` branch and all tests and lint jobs have
succeeded, a release job runs.  It tags the commit as `v<run_number>` and
publishes a GitHub release using `actions/create-release`.

## Environment and Secrets

The workflow expects several secrets to be configured in the repository
settings.  They are exposed to jobs as environment variables.  A detailed
list is available in [CISecrets.md](CISecrets.md).

For local development the same variables may be exported directly.  The
runtime also honors `SCASTD_USERNAME`, `SCASTD_PASSWORD` and their
`*_FILE` variants to supply credentials via the environment.
