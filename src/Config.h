#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>

class Config {
public:
    bool Load(const std::string &path);
    std::string Get(const std::string &key, const std::string &def = "") const;
private:
    std::map<std::string, std::string> values;
};

#endif // CONFIG_H
