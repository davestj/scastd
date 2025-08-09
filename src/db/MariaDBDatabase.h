#ifndef MARIADBDATABASE_H
#define MARIADBDATABASE_H

#include "IDatabase.h"
#include <mariadb/mysql.h>

class MariaDBDatabase : public IDatabase {
public:
    MariaDBDatabase();
    ~MariaDBDatabase();

    bool connect(const std::string &username,
                 const std::string &password,
                 const std::string &host,
                 int port,
                 const std::string &dbname,
                 const std::string &sslmode) override;
    bool query(const std::string &query) override;
    Row fetch() override;
    void disconnect() override;
private:
    MYSQL mySQL;
    MYSQL_RES *pResult;
};

#endif // MARIADBDATABASE_H
