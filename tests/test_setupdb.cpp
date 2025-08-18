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
#include <filesystem>
#include <cstdlib>
#include <sqlite3.h>

TEST_CASE("setupdb flag initializes SQLite database") {
    std::filesystem::path tmp = std::filesystem::temp_directory_path() / "scastd-setup.db";
    std::filesystem::remove(tmp);
    std::string cmd = "cd .. && src/scastd --config scastd.conf --setupdb=sqlite3 --sqlite-db " + tmp.string();
    int rc = std::system(cmd.c_str());
    REQUIRE(rc == 0);
    sqlite3 *db = nullptr;
    REQUIRE(sqlite3_open(tmp.string().c_str(), &db) == SQLITE_OK);
    sqlite3_stmt *stmt = nullptr;
    REQUIRE(sqlite3_prepare_v2(db, "SELECT name FROM sqlite_master WHERE type='table' AND name='servers';", -1, &stmt, nullptr) == SQLITE_OK);
    REQUIRE(sqlite3_step(stmt) == SQLITE_ROW);
    REQUIRE(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))) == "servers");
    sqlite3_finalize(stmt);
    REQUIRE(sqlite3_prepare_v2(db, "SELECT name FROM sqlite_master WHERE type='table' AND name='scastd_memberinfo';", -1, &stmt, nullptr) == SQLITE_OK);
    REQUIRE(sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    std::filesystem::remove(tmp);
}
