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

#include "Config.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstdlib>

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

    auto read_secret = [](const std::string &p, std::string &out) -> bool {
        std::ifstream f(p.c_str());
        if (!f) {
            return false;
        }
        if (!std::getline(f, out)) {
            return false;
        }
        if (!out.empty() && out.back() == '\r') {
            out.pop_back();
        }
        return true;
    };

    std::string secret;
    std::map<std::string, std::string>::iterator it;
    it = values.find("username_file");
    if (it != values.end() && read_secret(it->second, secret)) {
        values["username"] = secret;
    }
    it = values.find("password_file");
    if (it != values.end() && read_secret(it->second, secret)) {
        values["password"] = secret;
    }

    const char *env = std::getenv("SCASTD_USERNAME_FILE");
    if (env && read_secret(env, secret)) {
        values["username"] = secret;
    }
    env = std::getenv("SCASTD_PASSWORD_FILE");
    if (env && read_secret(env, secret)) {
        values["password"] = secret;
    }
    env = std::getenv("SCASTD_USERNAME");
    if (env) {
        values["username"] = env;
    }
    env = std::getenv("SCASTD_PASSWORD");
    if (env) {
        values["password"] = env;
    }

    env = std::getenv("ICEADMINUSER");
    if (!env) {
        env = std::getenv("SCASTD_ADMINUSER");
    }
    if (env) {
        values["iceadminuser"] = env;
    }
    env = std::getenv("ICEUSERPASS");
    if (!env) {
        env = std::getenv("SCASTD_USERPASS");
    }
    if (env) {
        values["iceuserpass"] = env;
    }

    env = std::getenv("SCASTD_THREAD_COUNT");
    if (env) {
        values["thread_count"] = env;
    }
    env = std::getenv("SCASTD_CPU_CORES");
    if (env) {
        values["cpu_cores"] = env;
    }
    env = std::getenv("SCASTD_MEMORY_LIMIT");
    if (env) {
        values["memory_limit"] = env;
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

int Config::GetDuration(const std::string &key, int default_ms) const {
    std::map<std::string, std::string>::const_iterator it = values.find(key);
    if (it == values.end()) {
        return default_ms;
    }
    std::string val = it->second;
    std::string lower;
    lower.resize(val.size());
    std::transform(val.begin(), val.end(), lower.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    int multiplier = 1;
    std::string number = lower;
    if (lower.size() >= 2 && lower.substr(lower.size() - 2) == "ms") {
        number = lower.substr(0, lower.size() - 2);
    } else if (!lower.empty() && lower.back() == 's') {
        multiplier = 1000;
        number = lower.substr(0, lower.size() - 1);
    } else if (!lower.empty() && lower.back() == 'm') {
        multiplier = 60 * 1000;
        number = lower.substr(0, lower.size() - 1);
    } else if (lower.size() >= 2 && lower.substr(lower.size() - 2) == "hr") {
        multiplier = 60 * 60 * 1000;
        number = lower.substr(0, lower.size() - 2);
    } else if (!lower.empty() && lower.back() == 'h') {
        multiplier = 60 * 60 * 1000;
        number = lower.substr(0, lower.size() - 1);
    }
    try {
        int base = std::stoi(number);
        return base * multiplier;
    } catch (...) {
        return default_ms;
    }
}

void Config::Set(const std::string &key, const std::string &value) {
    values[key] = value;
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

static std::string combine_log_path(const std::string &dir, const std::string &path) {
    if (!path.empty() && path[0] != '/') {
        if (!dir.empty() && dir.back() == '/') {
            return dir + path;
        }
        return dir + "/" + path;
    }
    return path;
}

std::string Config::AccessLog() const {
    std::string dir = Get("log_dir", "/var/log/scastd");
    std::string path = Get("access_log", "access.log");
    return combine_log_path(dir, path);
}

std::string Config::ErrorLog() const {
    std::string dir = Get("log_dir", "/var/log/scastd");
    std::string path = Get("error_log", "error.log");
    return combine_log_path(dir, path);
}

std::string Config::DebugLog() const {
    std::string dir = Get("log_dir", "/var/log/scastd");
    std::string path = Get("debug_log", "debug.log");
    return combine_log_path(dir, path);
}

int Config::DebugLevel() const {
    return Get("debug_level", 0);
}

size_t Config::LogMaxSize() const {
    return static_cast<size_t>(Get("log_max_size", 0));
}

int Config::LogRetention() const {
    return Get("log_retention", 0);
}

bool Config::SyslogEnabled() const {
    return Get("syslog_enabled", false);
}

std::string Config::SyslogHost() const {
    return Get("syslog_host", "localhost");
}

int Config::SyslogPort() const {
    return Get("syslog_port", 514);
}

std::string Config::SyslogProtocol() const {
    return Get("syslog_protocol", "udp");
}

std::string Config::PIDFile() const {
    return Get("pid_file", "/var/run/scastd.pid");
}
