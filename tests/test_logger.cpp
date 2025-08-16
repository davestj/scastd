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
#include "../src/logger.h"
#include <filesystem>

TEST_CASE("setLogDir creates directory with safe permissions") {
    namespace fs = std::filesystem;
    fs::path base = fs::temp_directory_path() / "scastd-logdir-test";
    fs::remove_all(base);

    Logger logger(false);
    logger.setLogDir(base.string());

    REQUIRE(fs::exists(base));
    fs::perms p = fs::status(base).permissions();
    REQUIRE((p & fs::perms::owner_read) != fs::perms::none);
    REQUIRE((p & fs::perms::owner_write) != fs::perms::none);
    REQUIRE((p & fs::perms::owner_exec) != fs::perms::none);
    REQUIRE((p & fs::perms::group_read) != fs::perms::none);
    REQUIRE((p & fs::perms::group_exec) != fs::perms::none);
    REQUIRE((p & fs::perms::group_write) == fs::perms::none);
    REQUIRE((p & (fs::perms::others_read | fs::perms::others_write |
                  fs::perms::others_exec)) == fs::perms::none);

    fs::remove_all(base);
}
