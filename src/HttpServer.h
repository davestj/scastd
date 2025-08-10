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

#include <atomic>
#include <chrono>
#include <microhttpd.h>
#include <string>

namespace scastd {

class HttpServer {
public:
    HttpServer();
    ~HttpServer();

    bool start(int port = 8333, const std::string &user = "", const std::string &pass = "");
    void stop();

private:
    static MHD_Result handleRequest(void *cls,
                                    struct MHD_Connection *connection,
                                    const char *url,
                                    const char *method,
                                    const char *version,
                                    const char *upload_data,
                                    size_t *upload_data_size,
                                    void **con_cls);

    std::atomic<bool> running_;
    struct MHD_Daemon *daemon_;
    std::chrono::steady_clock::time_point start_time_;
    std::string username_;
    std::string password_;
};

} // namespace scastd

