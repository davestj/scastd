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
#include "../src/db/MySQLDatabase.h"
#include "../src/db/MariaDBDatabase.h"
#include "../src/db/PostgresDatabase.h"
#include <sstream>
#include <iostream>

static void check_invalid_db(IDatabase &db) {
    std::streambuf *orig = std::cerr.rdbuf();
    std::ostringstream captured;
    std::cerr.rdbuf(captured.rdbuf());

    bool ok = db.connect("baduser", "badpass", "127.0.0.1", 0, "baddb", "");

    std::cerr.rdbuf(orig);

    int exit_code = ok ? 0 : 1;
    REQUIRE(exit_code == 1);
    REQUIRE(captured.str().find("Failed to connect to database:") != std::string::npos);
}

TEST_CASE("MySQL invalid credentials are logged and cause non-zero exit") {
    MySQLDatabase db;
    check_invalid_db(db);
}

TEST_CASE("MariaDB invalid credentials are logged and cause non-zero exit") {
    MariaDBDatabase db;
    check_invalid_db(db);
}

TEST_CASE("Postgres invalid credentials are logged and cause non-zero exit") {
    PostgresDatabase db;
    check_invalid_db(db);
}

