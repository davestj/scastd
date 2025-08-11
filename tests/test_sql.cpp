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

TEST_CASE("MySQL schema has required tables") {
    std::string sql = read_file(std::string(TEST_SRCDIR) + "/src/mysql.sql");
    REQUIRE(sql.find("CREATE TABLE scastd_memberinfo") != std::string::npos);
    REQUIRE(sql.find("CREATE TABLE scastd_runtime") != std::string::npos);
}

TEST_CASE("PostgreSQL schema has required tables") {
    std::string sql = read_file(std::string(TEST_SRCDIR) + "/src/postgres.sql");
    REQUIRE(sql.find("CREATE TABLE scastd_memberinfo") != std::string::npos);
    REQUIRE(sql.find("CREATE TABLE scastd_runtime") != std::string::npos);
}
