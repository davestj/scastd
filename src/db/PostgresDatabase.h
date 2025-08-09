#ifndef POSTGRESDATABASE_H
#define POSTGRESDATABASE_H

#include "IDatabase.h"
#include <libpq-fe.h>

class PostgresDatabase : public IDatabase {
public:
    PostgresDatabase();
    ~PostgresDatabase();

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
    PGconn *conn;
    PGresult *result;
    int currentRow;
};

#endif // POSTGRESDATABASE_H
