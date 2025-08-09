#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>

class Config {
public:
    bool Load(const std::string &path);
    std::string Get(const std::string &key, const std::string &def = "") const;
    std::string Get(const std::string &key, const char *def) const {
        return Get(key, std::string(def));
    }
    int Get(const std::string &key, int def) const;
    bool Get(const std::string &key, bool def) const;
private:
    std::map<std::string, std::string> values;
};

#endif // CONFIG_H
