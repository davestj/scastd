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

#include "StatusLogger.h"

#include <filesystem>
#include <chrono>
#include <ctime>
#include <sstream>

StatusLogger::StatusLogger(const std::string &path)
    : path_(path), maxSize_(0), retention_(0) {
    openStream();
}

void StatusLogger::setPath(const std::string &path) {
    std::lock_guard<std::mutex> lock(mtx_);
    path_ = path;
    stream_.close();
    openStream();
}

void StatusLogger::setRotation(size_t maxBytes, int retentionCount) {
    std::lock_guard<std::mutex> lock(mtx_);
    maxSize_ = maxBytes;
    retention_ = retentionCount;
}

void StatusLogger::openStream() {
    std::filesystem::path p(path_);
    auto dir = p.parent_path();
    if (!dir.empty())
        std::filesystem::create_directories(dir);
    stream_.open(path_, std::ios::app);
}

std::string StatusLogger::escape(const std::string &in) {
    std::string out;
    out.reserve(in.size());
    for (char c : in) {
        switch (c) {
        case '\\': out += "\\\\"; break;
        case '"': out += "\\\""; break;
        case '\n': out += "\\n"; break;
        case '\r': out += "\\r"; break;
        case '\t': out += "\\t"; break;
        default: out += c; break;
        }
    }
    return out;
}

void StatusLogger::log(const std::string &action,
                       const std::string &result,
                       const std::string &metadata) {
    std::lock_guard<std::mutex> lock(mtx_);
    rotateIfNeeded();
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    gmtime_r(&t, &tm);
    char timebuf[32];
    std::strftime(timebuf, sizeof(timebuf), "%Y-%m-%dT%H:%M:%SZ", &tm);
    std::ostringstream oss;
    oss << "{\"timestamp\":\"" << timebuf << "\",";
    oss << "\"action\":\"" << escape(action) << "\",";
    oss << "\"result\":\"" << escape(result) << "\",";
    oss << "\"metadata\":\"" << escape(metadata) << "\"}";
    stream_ << oss.str() << '\n';
    stream_.flush();
}

void StatusLogger::rotateIfNeeded() {
    if (maxSize_ == 0 || retention_ <= 0)
        return;
    std::streampos pos = stream_.tellp();
    if (pos < static_cast<std::streampos>(maxSize_))
        return;
    stream_.close();
    for (int i = retention_; i >= 1; --i) {
        std::string from = path_ + (i == 1 ? "" : "." + std::to_string(i - 1));
        std::string to = path_ + "." + std::to_string(i);
        if (std::filesystem::exists(from)) {
            if (i == retention_)
                std::filesystem::remove(to);
            std::filesystem::rename(from, to);
        }
    }
    stream_.open(path_, std::ios::trunc);
}

