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

