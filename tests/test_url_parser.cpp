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
#include "../src/UrlParser.h"

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
