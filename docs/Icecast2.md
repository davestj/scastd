# Icecast2 Statistics Fetcher

The `scastd::Icecast2` class retrieves runtime statistics from an Icecast server using
HTTP and exposes key fields such as listener counts, bitrates and the currently
playing track. It authenticates with the Icecast administrative interface using
HTTP Basic authentication and parses the returned `stats.xml` document.

## Servers Table

SCASTD stores Icecast endpoints in the `servers` table:

| Column | Type | Description |
| ------ | ---- | ----------- |
| `id` | INT | Primary key |
| `server_host` | VARCHAR(255) | Hostname |
| `server_port` | BIGINT | Port number |
| `server_username` | VARCHAR(255) | Optional admin user |
| `server_password` | VARCHAR(255) | Optional admin password |

Insert the provided development server:

```sql
INSERT INTO servers (server_host, server_port, server_username, server_password)
VALUES ('$ICECAST_DEV_HOST', $ICECAST_DEV_PORT, '$ICEADMINUSER', '$ICEUSERPASS');
```

See [`src/mariadb.sql`](../src/mariadb.sql) for the full schema.

## Example

```cpp
#include "CurlClient.h"
#include "icecast2.h"
#include <iostream>

int main() {
    CurlClient http;
    scastd::Icecast2 client("stream.example.com", 8000, "admin", "hackme", http);
    std::vector<scastd::Icecast2::StreamInfo> stats;
    std::string err;
    if (client.fetchStats(stats, err)) {
        for (const auto &s : stats) {
            std::cout << s.mount << ": " << s.listeners << " listeners\n";
        }
    } else {
        std::cerr << "Failed to fetch stats: " << err << std::endl;
    }
    return 0;
}
```

The `fetchStats` method returns a vector of `StreamInfo` structures which can be
used by the existing database layer to record real‑time stream statistics.

## Configuration

The Icecast statistics client reads credentials from the environment. For
backward compatibility the daemon checks the legacy `SCASTD_*` variables if the
new names are not set:

| Variable | Description | Fallback |
| -------- | ----------- | -------- |
| `ICEADMINUSER` | Username for the Icecast admin interface | `SCASTD_ADMINUSER` |
| `ICEUSERPASS` | Password for the admin user | `SCASTD_USERPASS` |

These values are used when establishing the HTTP connection to
`/admin/stats.xml`.

## Full Example Using Provided Server

The following program queries the Icecast2 server defined in the `servers`
table using credentials supplied via environment variables:

```cpp
#include "CurlClient.h"
#include "icecast2.h"
#include <cstdlib>
#include <iostream>

int main() {
    CurlClient http;
    const char* host = std::getenv("ICECAST_DEV_HOST");
    int port = std::atoi(std::getenv("ICECAST_DEV_PORT"));
    scastd::Icecast2 client(host, port,
                            std::getenv("ICEADMINUSER"),
                            std::getenv("ICEUSERPASS"),
                            http);
    std::vector<scastd::Icecast2::StreamInfo> stats;
    std::string err;
    if (client.fetchStats(stats, err)) {
        for (const auto &s : stats) {
            std::cout << s.mount << ": " << s.listeners << " listeners\n";
        }
    } else {
        std::cerr << "Failed to fetch stats: " << err << std::endl;
    }
    return 0;
}
```
