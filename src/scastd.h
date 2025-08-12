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

#ifndef SCASTD_H
#define SCASTD_H

#include <string>
#include <map>
#include "db/IDatabase.h"

namespace scastd {
int run(const std::string &configPath,
        const std::map<std::string, std::string> &overrides);
int dumpDatabase(const std::string &configPath,
                 const std::map<std::string, std::string> &overrides,
                 const std::string &dumpDir);
bool setupDatabase(const std::string &dbType, IDatabase *db);
}

#endif // SCASTD_H
