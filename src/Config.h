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

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
#include <cstddef>

class Config {
public:
    bool Load(const std::string &path);
    std::string Get(const std::string &key, const std::string &def = "") const;
    std::string Get(const std::string &key, const char *def) const {
        return Get(key, std::string(def));
    }
    int Get(const std::string &key, int def) const;
    bool Get(const std::string &key, bool def) const;
    int GetDuration(const std::string &key, int default_ms) const;

    void Set(const std::string &key, const std::string &value);

    std::string AccessLog() const;
    std::string ErrorLog() const;
    std::string DebugLog() const;
    int DebugLevel() const;
    size_t LogMaxSize() const;
    int LogRetention() const;
    bool SyslogEnabled() const;
    std::string SyslogHost() const;
    int SyslogPort() const;
    std::string SyslogProtocol() const;
    std::string PIDFile() const;

private:
    std::map<std::string, std::string> values;
};

#endif // CONFIG_H
