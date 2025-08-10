# Icecast2 Statistics Fetcher

The `scastd::Icecast2` class retrieves runtime statistics from an Icecast server using
HTTP and exposes key fields such as listener counts, bitrates and the currently
playing track. It authenticates with the Icecast administrative interface using
HTTP Basic authentication and parses the returned `stats.xml` document.

## Example

```cpp
#include "icecast2.h"
#include <iostream>

int main() {
    scastd::Icecast2 client("stream.example.com", 8000, "admin", "hackme");
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
