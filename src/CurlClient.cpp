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

#include "CurlClient.h"

#include <curl/curl.h>

namespace {
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t totalSize = size * nmemb;
    std::string *s = static_cast<std::string *>(userp);
    s->append(static_cast<char *>(contents), totalSize);
    return totalSize;
}
}

bool CurlClient::fetchUrl(const std::string &url, std::string &response,
                          long timeout, bool ipv6,
                          const std::string &userAgent,
                          long *httpCode) const {
    CURL *curl = curl_easy_init();
    if (!curl) {
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent.c_str());
    if (timeout > 0) {
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
    }
    if (ipv6) {
        curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V6);
    }

    CURLcode res = curl_easy_perform(curl);
    if (res == CURLE_OK && httpCode) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, httpCode);
    }
    curl_easy_cleanup(curl);

    return res == CURLE_OK;
}

