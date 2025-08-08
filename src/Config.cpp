#include "Config.h"
#include <fstream>
#include <sstream>

bool Config::Load(const std::string &path) {
    std::ifstream in(path.c_str());
    if (!in) {
        return false;
    }
    std::string line;
    while (std::getline(in, line)) {
        std::size_t pos = line.find('#');
        if (pos != std::string::npos) {
            line = line.substr(0, pos);
        }
        std::istringstream iss(line);
        std::string key;
        if (!(iss >> key)) {
            continue;
        }
        std::string value;
        if (!(iss >> value)) {
            value.clear();
        }
        values[key] = value;
    }
    return true;
}

std::string Config::Get(const std::string &key, const std::string &def) const {
    std::map<std::string, std::string>::const_iterator it = values.find(key);
    if (it != values.end()) {
        return it->second;
    }
    return def;
}
