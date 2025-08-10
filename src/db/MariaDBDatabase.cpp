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

#include "MariaDBDatabase.h"
#include <iostream>
#include <cstring>
#include "i18n.h"

MariaDBDatabase::MariaDBDatabase() : pResult(nullptr) {
    mysql_init(&mySQL);
}

MariaDBDatabase::~MariaDBDatabase() {
    if (pResult) {
        mysql_free_result(pResult);
    }
    mysql_close(&mySQL);
}

bool MariaDBDatabase::connect(const std::string &username,
                              const std::string &password,
                              const std::string &host,
                              int port,
                              const std::string &dbname,
                              const std::string &sslmode) {
    mysql_options(&mySQL, MYSQL_READ_DEFAULT_GROUP, "scastd");
    (void)sslmode;
    if (!mysql_real_connect(&mySQL, host.c_str(), username.c_str(), password.c_str(), dbname.c_str(), port, NULL, 0)) {
        std::cerr << _("Failed to connect to database: ") << mysql_error(&mySQL) << std::endl;
        return false;
    }
    return true;
}

bool MariaDBDatabase::query(const std::string &queryStr) {
    if (pResult) {
        mysql_free_result(pResult);
        pResult = nullptr;
    }
    if (mysql_query(&mySQL, queryStr.c_str()) != 0) {
        std::cerr << _("Misformed query (") << queryStr << _(")\nError: ") << mysql_error(&mySQL) << std::endl;
        return false;
    }
    pResult = mysql_store_result(&mySQL);
    return true;
}

IDatabase::Row MariaDBDatabase::fetch() {
    Row rowVec;
    if (!pResult) {
        return rowVec;
    }
    MYSQL_ROW row = mysql_fetch_row(pResult);
    if (!row) {
        return Row();
    }
    unsigned int numFields = mysql_num_fields(pResult);
    for (unsigned int i = 0; i < numFields; ++i) {
        rowVec.push_back(row[i] ? row[i] : "");
    }
    return rowVec;
}

void MariaDBDatabase::disconnect() {
    if (pResult) {
        mysql_free_result(pResult);
        pResult = nullptr;
    }
    mysql_close(&mySQL);
}
