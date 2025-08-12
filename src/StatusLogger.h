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

#ifndef STATUS_LOGGER_H
#define STATUS_LOGGER_H

#include <string>
#include <fstream>
#include <mutex>
#include <cstddef>

class StatusLogger {
public:
    explicit StatusLogger(const std::string &path);
    void setPath(const std::string &path);
    void setRotation(size_t maxBytes, int retentionCount);
    void log(const std::string &action,
             const std::string &result,
             const std::string &metadata);

private:
    std::string path_;
    std::ofstream stream_;
    std::mutex mtx_;
    size_t maxSize_;
    int retention_;

    void rotateIfNeeded();
    static std::string escape(const std::string &in);
    void openStream();
};

#endif // STATUS_LOGGER_H

