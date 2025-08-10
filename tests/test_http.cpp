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
#include "catch.hpp"
#include "HttpServer.h"
#include <curl/curl.h>
#include <thread>
#include <chrono>
#include <string>
#include <cstdlib>

static size_t write_cb(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total = size * nmemb;
    std::string *s = static_cast<std::string *>(userp);
    s->append(static_cast<char *>(contents), total);
    return total;
}

TEST_CASE("HTTP server responds with status") {
    setenv("SCASD_NO_DAEMON", "1", 1);
    scastd::HttpServer server;
    REQUIRE(server.start(18080));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    CURL *curl = curl_easy_init();
    REQUIRE(curl != nullptr);
    std::string buffer;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:18080/v1/status.json");
    CURLcode res = curl_easy_perform(curl);
    REQUIRE(res == CURLE_OK);
    long code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    REQUIRE(code == 200);
    char *ctype = nullptr;
    curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ctype);
    REQUIRE(std::string(ctype) == "application/json");
    REQUIRE(buffer == "{\"status\":\"ok\"}");

    buffer.clear();
    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:18080/v1/status.xml");
    res = curl_easy_perform(curl);
    REQUIRE(res == CURLE_OK);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    REQUIRE(code == 200);
    curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ctype);
    REQUIRE(std::string(ctype) == "application/xml");
    REQUIRE(buffer == "<status>ok</status>");

    buffer.clear();
    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:18080/v1/uptime");
    res = curl_easy_perform(curl);
    REQUIRE(res == CURLE_OK);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    REQUIRE(code == 200);
    REQUIRE(buffer.find("\"uptime\"") != std::string::npos);

    curl_easy_cleanup(curl);
    server.stop();
}

TEST_CASE("HTTP server uptime requires auth") {
    setenv("SCASD_NO_DAEMON", "1", 1);
    scastd::HttpServer server;
    REQUIRE(server.start(18081, "user", "pass"));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    CURL *curl = curl_easy_init();
    REQUIRE(curl != nullptr);
    std::string buffer;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:18081/v1/uptime");

    CURLcode res = curl_easy_perform(curl);
    REQUIRE(res == CURLE_OK);
    long code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    REQUIRE(code == 401);
    curl_easy_cleanup(curl);

    curl = curl_easy_init();
    REQUIRE(curl != nullptr);
    buffer.clear();
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:18081/v1/uptime");
    curl_easy_setopt(curl, CURLOPT_USERPWD, "user:pass");
    res = curl_easy_perform(curl);
    REQUIRE(res == CURLE_OK);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    REQUIRE(code == 200);
    REQUIRE(buffer.find("\"uptime\"") != std::string::npos);
    curl_easy_cleanup(curl);
    server.stop();
}
