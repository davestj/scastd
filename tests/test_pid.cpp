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
#include <fstream>
#include <thread>
#include <chrono>
#include <csignal>
#include <cstdlib>

TEST_CASE("daemon creates and removes pid file") {
    namespace fs = std::filesystem;
    fs::path tmpDir = fs::temp_directory_path();
    fs::path pidPath = tmpDir / "scastd-test.pid";
    fs::path dbPath = tmpDir / "scastd-test.db";
    fs::path logDir = tmpDir / "scastd-logs";
    fs::path cfgPath = tmpDir / "scastd-test.conf";

    fs::remove(pidPath);
    fs::remove(dbPath);
    fs::remove_all(logDir);
    fs::create_directories(logDir);

    std::ofstream cfg(cfgPath);
    cfg << "DatabaseType sqlite\n";
    cfg << "sqlite_path " << dbPath.string() << "\n";
    cfg << "log_dir " << logDir.string() << "\n";
    cfg << "poll_interval 1s\n";
    cfg.close();

    std::string cmd = "cd .. && src/scastd --config " + cfgPath.string() +
                      " --daemon --pid-file " + pidPath.string() +
                      " >/dev/null 2>&1 &";
    REQUIRE(std::system(cmd.c_str()) == 0);

    for (int i = 0; i < 50 && !fs::exists(pidPath); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    REQUIRE(fs::exists(pidPath));

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

    fs::remove(dbPath);
    fs::remove_all(logDir);
    fs::remove(cfgPath);
}
