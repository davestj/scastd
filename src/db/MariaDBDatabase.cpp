#include "MariaDBDatabase.h"
#include <iostream>
#include <cstring>

MariaDBDatabase::MariaDBDatabase() : pResult(nullptr) {
    mysql_init(&mySQL);
}

MariaDBDatabase::~MariaDBDatabase() {
    if (pResult) {
        mysql_free_result(pResult);
    }
    mysql_close(&mySQL);
}

bool MariaDBDatabase::connect(const std::string &username, const std::string &password) {
    mysql_options(&mySQL, MYSQL_READ_DEFAULT_GROUP, "scastd");
    if (!mysql_real_connect(&mySQL, "localhost", username.c_str(), password.c_str(), "scastd", 0, NULL, 0)) {
        std::cerr << "Failed to connect to database: " << mysql_error(&mySQL) << std::endl;
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
        std::cerr << "Misformed query (" << queryStr << ")\nError: " << mysql_error(&mySQL) << std::endl;
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
