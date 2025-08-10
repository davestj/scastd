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

#include "PostgresDatabase.h"
#include <iostream>
#include "i18n.h"
#include "logger.h"

namespace scastd { extern Logger logger; }

PostgresDatabase::PostgresDatabase() : conn(nullptr), result(nullptr), currentRow(0) {}

PostgresDatabase::~PostgresDatabase() {
    if (result) {
        PQclear(result);
    }
    if (conn) {
        PQfinish(conn);
    }
}

bool PostgresDatabase::connect(const std::string &username,
                               const std::string &password,
                               const std::string &host,
                               int port,
                               const std::string &dbname,
                               const std::string &sslmode) {
    std::string conninfo = "user=" + username + " password=" + password +
                           " dbname=" + dbname + " host=" + host;
    if (port > 0) {
        conninfo += " port=" + std::to_string(port);
    }
    if (!sslmode.empty()) {
        conninfo += " sslmode=" + sslmode;
    }
    conn = PQconnectdb(conninfo.c_str());
    if (PQstatus(conn) != CONNECTION_OK) {
        scastd::logger.logError(std::string(_("Failed to connect to database: ")) + PQerrorMessage(conn));
        return false;
    }
    scastd::logger.logDebug(std::string("Postgres connected: ") + host + ":" + std::to_string(port) + "/" + dbname);
    return true;
}

bool PostgresDatabase::query(const std::string &queryStr) {
    if (result) {
        PQclear(result);
        result = nullptr;
    }
    scastd::logger.logDebug(std::string("Postgres query: ") + queryStr);
    result = PQexec(conn, queryStr.c_str());
    ExecStatusType status = PQresultStatus(result);
    if (status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK) {
        scastd::logger.logError(std::string(_("Query failed: ")) + PQerrorMessage(conn));
        return false;
    }
    currentRow = 0;
    return true;
}

IDatabase::Row PostgresDatabase::fetch() {
    Row rowVec;
    if (!result) {
        return rowVec;
    }
    int numRows = PQntuples(result);
    if (currentRow >= numRows) {
        return Row();
    }
    int numFields = PQnfields(result);
    for (int i = 0; i < numFields; ++i) {
        rowVec.push_back(PQgetvalue(result, currentRow, i));
    }
    ++currentRow;
    return rowVec;
}

void PostgresDatabase::disconnect() {
    if (result) {
        PQclear(result);
        result = nullptr;
    }
    if (conn) {
        PQfinish(conn);
        conn = nullptr;
    }
}
