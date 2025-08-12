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
#include "../src/icecast2.h"
#include "../src/CurlClient.h"
#include <microhttpd.h>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

struct StatsServer {
    std::string xml;
    MHD_Daemon *daemon;
    StatsServer(const std::string &x) : xml(x) {
        daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, 18081, nullptr, nullptr,
                                  &StatsServer::handler, this, MHD_OPTION_END);
    }
    ~StatsServer() {
        if (daemon) {
            MHD_stop_daemon(daemon);
        }
    }
    static MHD_Result handler(void *cls, struct MHD_Connection *connection, const char *url,
                              const char *method, const char *version, const char *upload_data,
                              size_t *upload_data_size, void **con_cls) {
        (void)version; (void)upload_data; (void)upload_data_size; (void)con_cls;
        StatsServer *self = static_cast<StatsServer *>(cls);
        if (std::string(method) != "GET" || std::string(url) != "/admin/stats.xml") {
            return MHD_NO;
        }
        struct MHD_Response *response = MHD_create_response_from_buffer(
            self->xml.size(), (void *)self->xml.c_str(), MHD_RESPMEM_PERSISTENT);
        MHD_add_response_header(response, "Content-Type", "application/xml");
        int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);
        return static_cast<MHD_Result>(ret);
    }
};

TEST_CASE("Icecast2 parses XML stats") {
    std::string xml = "<icestats><source mount=\"/stream\"><listeners>5</listeners><bitrate>128</bitrate><title>Test</title></source></icestats>";
    StatsServer server(xml);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CurlClient client;
    scastd::Icecast2 ice("localhost", 18081, "", "", client);
    std::vector<scastd::Icecast2::StreamInfo> stats;
    std::string err;
    REQUIRE(ice.fetchStats(stats, err));
    REQUIRE(err.empty());
    REQUIRE(stats.size() == 1);
    REQUIRE(stats[0].mount == "/stream");
    REQUIRE(stats[0].listeners == 5);
    REQUIRE(stats[0].bitrate == 128);
    REQUIRE(stats[0].title == "Test");
}
