# Logging Configuration

Scastd can write access, error, and debug information to separate log
files and optionally forward messages to a remote syslog server. The
following configuration keys control logging behaviour. Paths may be
absolute or relative to `log_dir`.

| Key | Default | Description |
| --- | --- | --- |
| `log_dir` | `./logs` | Directory where log files are stored. |
| `access_log` | `access.log` | File receiving HTTP access entries. |
| `error_log` | `error.log` | File receiving error messages. |
| `debug_log` | `debug.log` | File receiving debug output. |
| `debug_level` | `1` | Debug verbosity level (`1`–`4`). |
| `syslog_enabled` | `false` | Enable forwarding to a syslog server. |
| `syslog_host` | `localhost` | Hostname or IP of the syslog server. |
| `syslog_port` | `514` | Syslog server port. |
| `syslog_protocol` | `udp` | Transport protocol (`udp` or `tcp`). |

When syslog forwarding is enabled, messages are still written to the
local log files in addition to being sent to the remote server.
