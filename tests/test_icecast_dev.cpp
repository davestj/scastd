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

#include "catch.hpp"
#include "../src/icecast2.h"
#include "../src/CurlClient.h"
#include <cstdlib>
#include <string>
#include <vector>

TEST_CASE("Fetch stats.xml from dev server") {
    const char *host = std::getenv("ICECAST_DEV_HOST");
    const char *portEnv = std::getenv("ICECAST_DEV_PORT");
    const char *user = std::getenv("ICEADMINUSER");
    if (!user) {
        user = std::getenv("SCASTD_ADMINUSER");
    }
    const char *pass = std::getenv("ICEUSERPASS");
    if (!pass) {
        pass = std::getenv("SCASTD_USERPASS");
    }
    if (!host || !user || !pass) {
        WARN("Missing ICECAST_DEV_HOST or credentials; skipping integration test");
        return;
    }
    int port = portEnv ? std::atoi(portEnv) : 8000;

    CurlClient client;
    scastd::Icecast2 ice(host, port, user, pass, client);
    std::vector<scastd::Icecast2::StreamInfo> stats;
    std::string err;
    REQUIRE(ice.fetchStats(stats, err));
    REQUIRE(err.empty());
    REQUIRE(!stats.empty());
}

