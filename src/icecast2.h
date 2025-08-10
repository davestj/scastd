/*
/////////////////////////////////////////////////
// Scast Daemon
// Authors: oddsock, dstjohn
/////////////////////////////////////////////////

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#ifndef ICECAST2_H
#define ICECAST2_H

#include <string>
#include <vector>

namespace scastd {

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

} // namespace scastd

#endif // ICECAST2_H
