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

#include "logger.h"

#include <filesystem>
#include <system_error>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

Logger::Logger(bool enabledInit,
               const std::string &accessFile,
               const std::string &errorFile,
               const std::string &debugFile,
               bool consoleOut)
    : accessPath(accessFile),
      errorPath(errorFile),
      debugPath(debugFile),
      console(consoleOut),
      enabled(enabledInit),
      debugLevel(1),
      syslogPort(0),
      syslogProto(SyslogProto::UDP),
      maxSize(0),
      retention(0) {
    if (enabled)
        openStreams();
}

void Logger::setLogFiles(const std::string &accessFile,
                         const std::string &errorFile,
                         const std::string &debugFile) {
    std::lock_guard<std::mutex> lock(mtx);
    accessPath = accessFile;
    errorPath = errorFile;
    debugPath = debugFile;
    if (enabled)
        openStreams();
}

void Logger::setLogDir(const std::string &directory) {
    namespace fs = std::filesystem;
    std::error_code ec;
    fs::create_directories(directory, ec);
    fs::permissions(directory,
                    fs::perms::owner_read | fs::perms::owner_write |
                        fs::perms::owner_exec | fs::perms::group_read |
                        fs::perms::group_exec,
                    fs::perm_options::replace, ec);
    setLogFiles(directory + "/access.log",
                directory + "/error.log",
                directory + "/debug.log");
}

void Logger::setConsoleOutput(bool enable) {
    std::lock_guard<std::mutex> lock(mtx);
    console = enable;
}

void Logger::setDebugLevel(int level) {
    std::lock_guard<std::mutex> lock(mtx);
    if (level < 1) level = 1;
    if (level > 4) level = 4;
    debugLevel = level;
}

void Logger::setSyslog(const std::string &host, int port, SyslogProto proto) {
    std::lock_guard<std::mutex> lock(mtx);
    syslogHost = host;
    syslogPort = port;
    syslogProto = proto;
}

void Logger::setRotation(size_t maxBytes, int retentionCount) {
    std::lock_guard<std::mutex> lock(mtx);
    maxSize = maxBytes;
    retention = retentionCount;
}

void Logger::setEnabled(bool enable) {
    std::lock_guard<std::mutex> lock(mtx);
    if (enabled == enable)
        return;
    enabled = enable;
    if (enabled) {
        openStreams();
    } else {
        if (accessStream.is_open()) accessStream.close();
        if (errorStream.is_open()) errorStream.close();
        if (debugStream.is_open()) debugStream.close();
    }
}

void Logger::logAccess(const std::string &message) {
    write(accessStream, accessPath, message, false, Level::Access);
}

void Logger::logError(const std::string &message) {
    write(errorStream, errorPath, message, true, Level::Error);
}

void Logger::logDebug(const std::string &message, int level) {
    if (level <= debugLevel)
        write(debugStream, debugPath, message, false, Level::Debug);
}

void Logger::openStreams() {
    auto open = [](std::ofstream &stream, const std::string &path) {
        if (stream.is_open()) stream.close();
        std::filesystem::path p(path);
        auto dir = p.parent_path();
        if (!dir.empty())
            std::filesystem::create_directories(dir);
        stream.open(path, std::ios::app);
    };

    open(accessStream, accessPath);
    open(errorStream, errorPath);
    open(debugStream, debugPath);
}

void Logger::write(std::ofstream &stream, const std::string &path,
                   const std::string &message, bool err, Level level) {
    std::lock_guard<std::mutex> lock(mtx);
    if (!enabled)
        return;
    rotateIfNeeded(stream, path);
    std::string msg = message;
    if (!msg.empty() && msg.back() != '\n')
        msg.push_back('\n');
    stream << msg;
    stream.flush();
    if (console) {
        if (err)
            std::cerr << msg;
        else
            std::cout << msg;
    }
    sendToSyslog(msg, level);
}

void Logger::rotateIfNeeded(std::ofstream &stream, const std::string &path) {
    if (maxSize == 0 || retention <= 0)
        return;
    std::streampos pos = stream.tellp();
    if (pos < static_cast<std::streampos>(maxSize))
        return;
    stream.close();
    for (int i = retention; i >= 1; --i) {
        std::string from = path + (i == 1 ? "" : "." + std::to_string(i - 1));
        std::string to = path + "." + std::to_string(i);
        if (std::filesystem::exists(from)) {
            if (i == retention)
                std::filesystem::remove(to);
            std::filesystem::rename(from, to);
        }
    }
    stream.open(path, std::ios::trunc);
}

void Logger::sendToSyslog(const std::string &message, Level /*level*/) {
    if (syslogHost.empty() || syslogPort <= 0)
        return;

    int socktype = syslogProto == SyslogProto::UDP ? SOCK_DGRAM : SOCK_STREAM;
    struct addrinfo hints{};
    struct addrinfo *res = nullptr;
    hints.ai_socktype = socktype;
    hints.ai_family = AF_UNSPEC;
    if (getaddrinfo(syslogHost.c_str(), std::to_string(syslogPort).c_str(), &hints, &res) != 0)
        return;

    int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (fd < 0) {
        freeaddrinfo(res);
        return;
    }

    if (syslogProto == SyslogProto::TCP) {
        if (connect(fd, res->ai_addr, res->ai_addrlen) == 0)
            send(fd, message.c_str(), message.size(), 0);
    } else {
        sendto(fd, message.c_str(), message.size(), 0, res->ai_addr, res->ai_addrlen);
    }

    freeaddrinfo(res);
    close(fd);
}
