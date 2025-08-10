#include "catch.hpp"
#include "UrlParser.h"

TEST_CASE("parseHostPort handles IPv4") {
    std::string host;
    int port = 0;
    REQUIRE(parseHostPort("http://192.0.2.1:8000", host, port));
    REQUIRE(host == "192.0.2.1");
    REQUIRE(port == 8000);
}

TEST_CASE("parseHostPort handles IPv6") {
    std::string host;
    int port = 0;
    REQUIRE(parseHostPort("http://[2001:db8::1]:8000", host, port));
    REQUIRE(host == "2001:db8::1");
    REQUIRE(port == 8000);
}
