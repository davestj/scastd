#include "catch.hpp"
#include "Config.h"
#include <fstream>
#include <cstdio>

TEST_CASE("Config parsing") {
    const char* fname = "test.conf";
    std::ofstream out(fname);
    out << "# comment\n";
    out << "port 8080\n";
    out << "flag yes\n";
    out.close();

    Config cfg;
    REQUIRE(cfg.Load(fname));
    REQUIRE(cfg.Get("port", 0) == 8080);
    REQUIRE(cfg.Get("flag", false) == true);
    REQUIRE(cfg.Get("missing", 42) == 42);

    std::remove(fname);
}
