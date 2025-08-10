# Resource Limits

The daemon supports resource limiting through the following configuration keys in `scastd.conf`:

- `thread_count` – number of worker threads and HTTP server threads.
- `cpu_cores` – number of CPU cores the process may utilize.
- `memory_limit` – maximum memory usage in megabytes.

## Recommended values

| Environment | thread_count | cpu_cores | memory_limit |
|-------------|--------------|-----------|--------------|
| Container   | 2            | 2         | 256 MB       |
| Bare metal  | match cores  | 0 (all)   | 0 (unlimited)|

`thread_count` defaults to the number of hardware threads when set to `0`.
`cpu_cores` and `memory_limit` values of `0` disable the respective limits.
