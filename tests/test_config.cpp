#include "catch.hpp"
#include "Config.h"
#include <fstream>
#include <cstdio>
#include <cstdlib>

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

TEST_CASE("Environment and secret overrides") {
    const char* cfg = "test.conf";
    std::ofstream out(cfg);
    out << "username fileuser\n";
    out << "password filepass\n";
    out.close();

    // environment variables override config
    setenv("SCASTD_USERNAME", "envuser", 1);
    setenv("SCASTD_PASSWORD", "envpass", 1);
    Config envcfg;
    REQUIRE(envcfg.Load(cfg));
    REQUIRE(envcfg.Get("username", "") == std::string("envuser"));
    REQUIRE(envcfg.Get("password", "") == std::string("envpass"));
    unsetenv("SCASTD_USERNAME");
    unsetenv("SCASTD_PASSWORD");

    // secret files via environment variables
    const char* ufile = "user.secret";
    const char* pfile = "pass.secret";
    std::ofstream uf(ufile); uf << "ufile\n"; uf.close();
    std::ofstream pf(pfile); pf << "pfile\n"; pf.close();
    setenv("SCASTD_USERNAME_FILE", ufile, 1);
    setenv("SCASTD_PASSWORD_FILE", pfile, 1);
    Config filecfg;
    REQUIRE(filecfg.Load(cfg));
    REQUIRE(filecfg.Get("username", "") == std::string("ufile"));
    REQUIRE(filecfg.Get("password", "") == std::string("pfile"));
    unsetenv("SCASTD_USERNAME_FILE");
    unsetenv("SCASTD_PASSWORD_FILE");

    // secret files via configuration entries
    std::ofstream out2(cfg);
    out2 << "username_file " << ufile << "\n";
    out2 << "password_file " << pfile << "\n";
    out2.close();
    Config cfgfile;
    REQUIRE(cfgfile.Load(cfg));
    REQUIRE(cfgfile.Get("username", "") == std::string("ufile"));
    REQUIRE(cfgfile.Get("password", "") == std::string("pfile"));

    std::remove(ufile);
    std::remove(pfile);
    std::remove(cfg);
}
