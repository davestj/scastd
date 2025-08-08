#ifndef IDATABASE_H
#define IDATABASE_H

#include <string>
#include <vector>

class IDatabase {
public:
    using Row = std::vector<std::string>;
    virtual ~IDatabase() {}
    virtual bool connect(const std::string &username, const std::string &password) = 0;
    virtual bool query(const std::string &query) = 0;
    virtual Row fetch() = 0;
    virtual void disconnect() = 0;
};

#endif // IDATABASE_H
