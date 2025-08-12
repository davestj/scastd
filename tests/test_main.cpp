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
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "../src/logger.h"
#include "../src/StatusLogger.h"
#include <sqlite3.h>
#include <filesystem>
#include <cstdlib>
#include <cstdio>

namespace scastd {
Logger logger(true);
StatusLogger statusLogger("/tmp/status.json");
struct LoggerInit {
    LoggerInit() { logger.setConsoleOutput(true); }
} loggerInit;
}

int main(int argc, char *argv[]) {
    const char *env = std::getenv("SQLITE_DB_PATH");
    std::filesystem::path dbPath = env ? env : (std::filesystem::temp_directory_path() / "scastd-test.db");
    std::filesystem::create_directories(dbPath.parent_path());
    sqlite3 *db = nullptr;
    if (sqlite3_open(dbPath.c_str(), &db) == SQLITE_OK) {
        sqlite3_close(db);
    }
    int result = Catch::Session().run(argc, argv);
    std::filesystem::remove(dbPath);
    std::filesystem::remove("/tmp/status.json");
    return result;
}
