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

int main(int argc, char **argv) {
    std::string configPath = "scastd.conf";
    std::string dumpDir = "/tmp";
    bool doDump = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--dump") {
            doDump = true;
        } else if (arg == "--dump-dir" && i + 1 < argc) {
            dumpDir = argv[++i];
        } else {
            configPath = arg;
        }
    }

    if (doDump) {
        return scastd::dumpDatabase(configPath, dumpDir);
    }

    return scastd::run(configPath);
}

