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

#include "UrlParser.h"
#include <cstring>
#include <cstdlib>

bool parseHostPort(const std::string &url, std::string &host, int &port) {
    const char *scheme = "http://";
    size_t start = 0;
    if (url.compare(0, std::strlen(scheme), scheme) == 0) {
        start = std::strlen(scheme);
    }
    if (start >= url.size()) return false;
    if (url[start] == '[') {
        size_t end = url.find(']', start);
        if (end == std::string::npos) return false;
        host = url.substr(start + 1, end - start - 1);
        size_t colon = url.find(':', end);
        if (colon == std::string::npos) return false;
        port = std::atoi(url.c_str() + colon + 1);
        return true;
    } else {
        size_t colon = url.find(':', start);
        if (colon == std::string::npos) return false;
        host = url.substr(start, colon - start);
        port = std::atoi(url.c_str() + colon + 1);
        return true;
    }
}

