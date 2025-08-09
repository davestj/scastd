#include "Config.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

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

int Config::Get(const std::string &key, int def) const {
    std::map<std::string, std::string>::const_iterator it = values.find(key);
    if (it != values.end()) {
        try {
            return std::stoi(it->second);
        } catch (...) {
            return def;
        }
    }
    return def;
}

bool Config::Get(const std::string &key, bool def) const {
    std::map<std::string, std::string>::const_iterator it = values.find(key);
    if (it != values.end()) {
        std::string val = it->second;
        std::transform(val.begin(), val.end(), val.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        if (val == "1" || val == "true" || val == "yes" || val == "on") {
            return true;
        }
        if (val == "0" || val == "false" || val == "no" || val == "off") {
            return false;
        }
    }
    return def;
}
