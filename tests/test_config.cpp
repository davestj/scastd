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

TEST_CASE("Logging configuration defaults") {
    const char* fname = "log.conf";
    std::ofstream out(fname);
    out << "log_dir logs\n";
    out.close();

    Config cfg;
    REQUIRE(cfg.Load(fname));
    REQUIRE(cfg.AccessLog() == std::string("logs/access.log"));
    REQUIRE(cfg.ErrorLog() == std::string("logs/error.log"));
    REQUIRE(cfg.DebugLog() == std::string("logs/debug.log"));
    REQUIRE(cfg.DebugLevel() == 0);
    REQUIRE(cfg.SyslogEnabled() == false);
    REQUIRE(cfg.SyslogHost() == std::string("localhost"));
    REQUIRE(cfg.SyslogPort() == 514);
    REQUIRE(cfg.SyslogProtocol() == std::string("udp"));

    std::remove(fname);
}

TEST_CASE("Logging configuration overrides") {
    const char* fname = "log_override.conf";
    std::ofstream out(fname);
    out << "log_dir logs\n";
    out << "access_log /tmp/access.log\n";
    out << "error_log err.log\n";
    out << "debug_log dbg.log\n";
    out << "debug_level 2\n";
    out << "syslog_enabled true\n";
    out << "syslog_host remote\n";
    out << "syslog_port 1514\n";
    out << "syslog_protocol tcp\n";
    out.close();

    Config cfg;
    REQUIRE(cfg.Load(fname));
    REQUIRE(cfg.AccessLog() == std::string("/tmp/access.log"));
    REQUIRE(cfg.ErrorLog() == std::string("logs/err.log"));
    REQUIRE(cfg.DebugLog() == std::string("logs/dbg.log"));
    REQUIRE(cfg.DebugLevel() == 2);
    REQUIRE(cfg.SyslogEnabled() == true);
    REQUIRE(cfg.SyslogHost() == std::string("remote"));
    REQUIRE(cfg.SyslogPort() == 1514);
    REQUIRE(cfg.SyslogProtocol() == std::string("tcp"));

    std::remove(fname);
}
