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

#include "../src/i18n.h"
#include "../src/db/SQLiteDatabase.h"

void init_i18n() {}

SQLiteDatabase::SQLiteDatabase() : db(nullptr), stmt(nullptr) {}
SQLiteDatabase::~SQLiteDatabase() {}
bool SQLiteDatabase::connect(const std::string &, const std::string &, const std::string &, int, const std::string &, const std::string &) { return false; }
bool SQLiteDatabase::query(const std::string &) { return false; }
IDatabase::Row SQLiteDatabase::fetch() { return {}; }
void SQLiteDatabase::disconnect() {}
