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

#include "SQLiteDatabase.h"
#include <iostream>
#include "i18n.h"

SQLiteDatabase::SQLiteDatabase() : db(nullptr), stmt(nullptr) {}

SQLiteDatabase::~SQLiteDatabase() {
    disconnect();
}

bool SQLiteDatabase::connect(const std::string &username,
                             const std::string &password,
                             const std::string &host,
                             int port,
                             const std::string &dbname,
                             const std::string &sslmode) {
    (void)username; (void)password; (void)host; (void)port; (void)sslmode;
    if (sqlite3_open(dbname.c_str(), &db) != SQLITE_OK) {
        std::cerr << _("Failed to open SQLite database: ") << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    return true;
}

bool SQLiteDatabase::query(const std::string &queryStr) {
    if (stmt) {
        sqlite3_finalize(stmt);
        stmt = nullptr;
    }
    const char *tail = nullptr;
    if (sqlite3_prepare_v2(db, queryStr.c_str(), -1, &stmt, &tail) == SQLITE_OK && tail && *tail == '\0') {
        return true;
    }
    if (stmt) {
        sqlite3_finalize(stmt);
        stmt = nullptr;
    }
    char *err = nullptr;
    if (sqlite3_exec(db, queryStr.c_str(), nullptr, nullptr, &err) != SQLITE_OK) {
        std::cerr << _("Failed to execute query: ") << (err ? err : "") << std::endl;
        if (err) sqlite3_free(err);
        return false;
    }
    return true;
}

IDatabase::Row SQLiteDatabase::fetch() {
    Row rowVec;
    if (!stmt) {
        return rowVec;
    }
    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        stmt = nullptr;
        return Row();
    }
    int cols = sqlite3_column_count(stmt);
    for (int i = 0; i < cols; ++i) {
        const unsigned char *text = sqlite3_column_text(stmt, i);
        rowVec.push_back(text ? reinterpret_cast<const char*>(text) : "");
    }
    return rowVec;
}

void SQLiteDatabase::disconnect() {
    if (stmt) {
        sqlite3_finalize(stmt);
        stmt = nullptr;
    }
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

