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
#include <iostream>

Logger::Logger(const std::string &directory, bool consoleOut)
    : logDir(directory), console(consoleOut) {
    openStreams();
}

void Logger::setLogDir(const std::string &directory) {
    std::lock_guard<std::mutex> lock(mtx);
    logDir = directory;
    openStreams();
}

void Logger::setConsoleOutput(bool enable) {
    std::lock_guard<std::mutex> lock(mtx);
    console = enable;
}

void Logger::logAccess(const std::string &message) {
    write(accessStream, message, false);
}

void Logger::logError(const std::string &message) {
    write(errorStream, message, true);
}

void Logger::logDebug(const std::string &message) {
    write(debugStream, message, false);
}

void Logger::openStreams() {
    if (logDir.empty())
        logDir = ".";
    std::filesystem::create_directories(logDir);
    if (accessStream.is_open()) accessStream.close();
    if (errorStream.is_open()) errorStream.close();
    if (debugStream.is_open()) debugStream.close();
    accessStream.open(logDir + "/access.log", std::ios::app);
    errorStream.open(logDir + "/error.log", std::ios::app);
    debugStream.open(logDir + "/debug.log", std::ios::app);
}

void Logger::write(std::ofstream &stream, const std::string &message, bool err) {
    std::lock_guard<std::mutex> lock(mtx);
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
}
