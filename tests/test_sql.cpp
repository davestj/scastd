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
#include <fstream>
#include <sstream>
#include <string>

static std::string read_file(const std::string &path) {
    std::ifstream in(path.c_str());
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

TEST_CASE("MariaDB schema has required tables") {
    const std::string schema = std::string(TEST_SRCDIR) + "/src/mariadb.sql";
    std::string sql = read_file(schema);
    REQUIRE(sql.find("DROP TABLE IF EXISTS scastd_memberinfo") != std::string::npos);
    REQUIRE(sql.find("CREATE TABLE servers") != std::string::npos);
    REQUIRE(sql.find("CREATE TABLE scastd_runtime") != std::string::npos);
}

TEST_CASE("PostgreSQL schema has required tables") {
    std::string sql = read_file(std::string(TEST_SRCDIR) + "/src/postgres.sql");
    REQUIRE(sql.find("DROP TABLE IF EXISTS scastd_memberinfo") != std::string::npos);
    REQUIRE(sql.find("CREATE TABLE servers") != std::string::npos);
    REQUIRE(sql.find("CREATE TABLE scastd_runtime") != std::string::npos);
}
