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
#include "../src/CurlClient.h"
#include <filesystem>
#include <fstream>
#include <thread>
#include <chrono>
#include <csignal>
#include <cstdlib>

TEST_CASE("scastd --test-mode validates configuration") {
    namespace fs = std::filesystem;
    fs::path tmpDir = fs::temp_directory_path() / "scastd-testmode";
    fs::path dbPath = tmpDir / "scastd-test.db";
    fs::path logDir = tmpDir / "logs";
    fs::path cfgPath = tmpDir / "scastd.conf";

    fs::remove_all(tmpDir);
    fs::create_directories(logDir);

    std::ofstream cfg(cfgPath);
    cfg << "DatabaseType sqlite\n";
    cfg << "sqlite_path " << dbPath.string() << "\n";
    cfg << "log_dir " << logDir.string() << "\n";
    cfg << "poll_interval 1s\n";
    cfg.close();

    std::string cmd = "cd .. && src/scastd --config " + cfgPath.string() +
                      " --test-mode --ip 127.0.0.1 --port 18080 >/dev/null 2>&1";
    REQUIRE(std::system(cmd.c_str()) == 0);

    fs::remove_all(tmpDir);
}

TEST_CASE("scastd daemon serves HTTP and writes pid and logs") {
    namespace fs = std::filesystem;
    fs::path tmpDir = fs::temp_directory_path() / "scastd-daemon";
    fs::path pidPath = tmpDir / "scastd.pid";
    fs::path dbPath = tmpDir / "scastd.db";
    fs::path logDir = tmpDir / "logs";
    fs::path cfgPath = tmpDir / "scastd.conf";

    fs::remove_all(tmpDir);
    fs::create_directories(logDir);

    std::ofstream cfg(cfgPath);
    cfg << "DatabaseType sqlite\n";
    cfg << "sqlite_path " << dbPath.string() << "\n";
    cfg << "log_dir " << logDir.string() << "\n";
    cfg << "poll_interval 1s\n";
    cfg.close();

    int port = 18081;
    std::string cmd = "cd .. && src/scastd --config " + cfgPath.string() +
                      " --daemon --pid-file " + pidPath.string() +
                      " --ip 127.0.0.1 --port " + std::to_string(port) +
                      " >/dev/null 2>&1 &";
    REQUIRE(std::system(cmd.c_str()) == 0);

    for (int i = 0; i < 50 && !fs::exists(pidPath); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    REQUIRE(fs::exists(pidPath));

    CurlClient client;
    std::string response;
    long code = 0;
    bool ok = false;
    for (int i = 0; i < 50 && !ok; ++i) {
        ok = client.fetchUrl("http://127.0.0.1:" + std::to_string(port) + "/status.json", response, 0, false, "scastd-test", &code) && code == 200;
        if (!ok)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    REQUIRE(ok);
    REQUIRE(response.find("\"status\":\"ok\"") != std::string::npos);

    std::ifstream pf(pidPath);
    int pid = 0;
    pf >> pid;
    pf.close();
    REQUIRE(pid > 0);
    REQUIRE(kill(pid, SIGTERM) == 0);

    for (int i = 0; i < 50 && fs::exists(pidPath); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    REQUIRE(!fs::exists(pidPath));

    std::ifstream debug(logDir / "debug.log");
    std::string dlog((std::istreambuf_iterator<char>(debug)), {});
    REQUIRE(dlog.find("HTTP server started") != std::string::npos);
    REQUIRE(dlog.find("HTTP request: GET /status.json") != std::string::npos);

    std::ifstream access(logDir / "access.log");
    std::string alog((std::istreambuf_iterator<char>(access)), {});
    REQUIRE(alog.find("/status.json 200") != std::string::npos);

    fs::remove_all(tmpDir);
}
