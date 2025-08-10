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

class Logger {
public:
    enum class Level { Access, Error, Debug };

    explicit Logger(const std::string &directory = ".", bool console = false);

    void setLogDir(const std::string &directory);
    void setConsoleOutput(bool enable);

    void logAccess(const std::string &message);
    void logError(const std::string &message);
    void logDebug(const std::string &message);

private:
    std::string logDir;
    bool console;
    std::ofstream accessStream;
    std::ofstream errorStream;
    std::ofstream debugStream;
    std::mutex mtx;

    void write(std::ofstream &stream, const std::string &message, bool err);
    void openStreams();
};

#endif // LOGGER_H
