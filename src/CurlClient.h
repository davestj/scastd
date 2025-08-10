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

#pragma once

#include <string>

// Simple HTTP client built on top of libcurl.  Methods are virtual to allow
// tests to provide mock implementations.
class CurlClient {
public:
    virtual ~CurlClient() = default;

    // Fetch the content at the given URL using libcurl.  Returns true on
    // success and stores the response body in `response`.  If `httpCode` is
    // non-null it will be populated with the HTTP status code from the
    // response.
    virtual bool fetchUrl(const std::string &url, std::string &response,
                          long timeout = 0, bool ipv6 = false,
                          const std::string &userAgent =
                              "Mozilla/4.0 (compatible; scastd)",
                          long *httpCode = nullptr) const;
};

