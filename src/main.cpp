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

#include "scastd.h"
#include <string>
#include <iostream>

static void printUsage()
{
    std::cout << "Usage: scastd [options] [config]\n"
              << "\nOptions:\n"
              << "  --dump                 Dump the database and exit\n"
              << "  --dump-dir DIR         Directory for database dump\n"
              << "  --logging=true|false   Enable or disable file logging\n"
              << "  --logpath=PATH         Override log directory\n"
              << "  -h, --help             Show this help message\n";
}

int main(int argc, char **argv) {
    std::string configPath = "scastd.conf";
    std::string dumpDir = "/tmp";
    bool doDump = false;
    bool loggingEnabled = true;
    std::string logPath;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--dump") {
            doDump = true;
        } else if (arg == "--dump-dir" && i + 1 < argc) {
            dumpDir = argv[++i];
        } else if (arg.rfind("--logging=", 0) == 0) {
            std::string val = arg.substr(10);
            loggingEnabled = !(val == "false" || val == "0");
        } else if (arg.rfind("--logpath=", 0) == 0) {
            logPath = arg.substr(10);
        } else if (arg == "--help" || arg == "-h") {
            printUsage();
            return 0;
        } else {
            configPath = arg;
        }
    }

    if (doDump) {
        return scastd::dumpDatabase(configPath, dumpDir, loggingEnabled, logPath);
    }

    return scastd::run(configPath, loggingEnabled, logPath);
}

