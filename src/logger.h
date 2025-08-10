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

#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <mutex>
#include <cstddef>

class Logger {
public:
    enum class Level { Access, Error, Debug };
    enum class SyslogProto { UDP, TCP };

    explicit Logger(bool enabled = false,
                    const std::string &accessFile = "access.log",
                    const std::string &errorFile = "error.log",
                    const std::string &debugFile = "debug.log",
                    bool console = false);

    void setEnabled(bool enable);

    void setLogFiles(const std::string &accessFile,
                     const std::string &errorFile,
                     const std::string &debugFile);
    void setLogDir(const std::string &directory);
    void setConsoleOutput(bool enable);
    void setDebugLevel(int level);
    void setSyslog(const std::string &host, int port, SyslogProto proto);
    void setRotation(size_t maxBytes, int retentionCount);

    void logAccess(const std::string &message);
    void logError(const std::string &message);
    void logDebug(const std::string &message, int level = 1);

private:
    std::string accessPath;
    std::string errorPath;
    std::string debugPath;
    bool console;
    bool enabled;
    std::ofstream accessStream;
    std::ofstream errorStream;
    std::ofstream debugStream;
    std::mutex mtx;
    int debugLevel;
    std::string syslogHost;
    int syslogPort;
    SyslogProto syslogProto;
    size_t maxSize;
    int retention;

    void write(std::ofstream &stream, const std::string &path,
               const std::string &message, bool err, Level level);
    void openStreams();
    void sendToSyslog(const std::string &message, Level level);
    void rotateIfNeeded(std::ofstream &stream, const std::string &path);
};

#endif // LOGGER_H
