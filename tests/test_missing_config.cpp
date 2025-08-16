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
#include "../src/scastd.h"
#include "../src/logger.h"
#include <sstream>
#include <iostream>

namespace scastd {
extern Logger logger;
}

TEST_CASE("Missing config file logs error") {
    using namespace scastd;
    logger.setEnabled(false);
    logger.setConsoleOutput(false);

    std::stringstream err;
    auto oldBuf = std::cerr.rdbuf(err.rdbuf());
    int rc = run("nonexistent.conf", {}, false);
    std::cerr.rdbuf(oldBuf);

    // Restore logger state for other tests
    logger.setEnabled(true);
    logger.setConsoleOutput(true);

    REQUIRE(rc == 1);
    REQUIRE(err.str().find("Cannot load config file nonexistent.conf") != std::string::npos);
}
