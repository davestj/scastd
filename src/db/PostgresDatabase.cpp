#include "PostgresDatabase.h"
#include <iostream>

PostgresDatabase::PostgresDatabase() : conn(nullptr), result(nullptr), currentRow(0) {}

PostgresDatabase::~PostgresDatabase() {
    if (result) {
        PQclear(result);
    }
    if (conn) {
        PQfinish(conn);
    }
}

bool PostgresDatabase::connect(const std::string &username, const std::string &password) {
    std::string conninfo = "user=" + username + " password=" + password + " dbname=scastd host=localhost";
    conn = PQconnectdb(conninfo.c_str());
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Failed to connect to database: " << PQerrorMessage(conn) << std::endl;
        return false;
    }
    return true;
}

bool PostgresDatabase::query(const std::string &queryStr) {
    if (result) {
        PQclear(result);
        result = nullptr;
    }
    result = PQexec(conn, queryStr.c_str());
    ExecStatusType status = PQresultStatus(result);
    if (status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK) {
        std::cerr << "Query failed: " << PQerrorMessage(conn) << std::endl;
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
