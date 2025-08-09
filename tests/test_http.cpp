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
    HttpServer server;
    REQUIRE(server.start(18080));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    CURL *curl = curl_easy_init();
    REQUIRE(curl != nullptr);
    std::string buffer;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:18080/status.json");
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
    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:18080/status.xml");
    res = curl_easy_perform(curl);
    REQUIRE(res == CURLE_OK);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    REQUIRE(code == 200);
    curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ctype);
    REQUIRE(std::string(ctype) == "application/xml");
    REQUIRE(buffer == "<status>ok</status>");

    curl_easy_cleanup(curl);
    server.stop();
}
