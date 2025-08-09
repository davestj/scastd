#ifndef ICECAST2_H
#define ICECAST2_H

#include <string>
#include <vector>

// Fetch Icecast server statistics via HTTP and expose basic fields.
class Icecast2 {
public:
    struct StreamInfo {
        std::string mount;     // mountpoint name
        int listeners;         // current number of listeners
        int bitrate;           // stream bitrate in kbps
        std::string title;     // currently playing track
    };

    Icecast2(const std::string &host,
             int port,
             const std::string &username,
             const std::string &password);

    // Fetch statistics from the server. Returns true on success.
    // On failure returns false and stores the error message in `error`.
    bool fetchStats(std::vector<StreamInfo> &stats, std::string &error) const;

private:
    std::string host;
    int port;
    std::string username;
    std::string password;
};

#endif // ICECAST2_H
