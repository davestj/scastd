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
#include "HttpServer.h"
#include "i18n.h"

#include <cstring>
#include <csignal>
#include <string>
#include <unistd.h>
#include <cstdlib>
#include <chrono>

namespace scastd {

namespace {
HttpServer *g_server = nullptr;

void handle_signal(int) {
    if (g_server) {
        g_server->stop();
    }
    _exit(0);
}

static const char kJsonResponse[] = "{\"status\":\"ok\"}";
static const char kXmlResponse[] = "<status>ok</status>";
static const char kApiVersion[] = "/v1";
} // namespace

HttpServer::HttpServer() : running_(false), daemon_(nullptr) {}

HttpServer::~HttpServer() {
    stop();
}

bool HttpServer::start(int port, const std::string &user, const std::string &pass, int threads) {
#if defined(__APPLE__) || defined(__linux__)
    const char *no_daemon = std::getenv("SCASD_NO_DAEMON");
    if (!no_daemon || std::strcmp(no_daemon, "1") != 0) {
        if (daemon(0, 0) != 0) {
            return false;
        }
    }
#endif
    username_ = user;
    password_ = pass;
    start_time_ = std::chrono::steady_clock::now();

    g_server = this;
    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);

    daemon_ = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD,
                               port,
                               nullptr, nullptr,
                               &HttpServer::handleRequest, this,
                               MHD_OPTION_THREAD_POOL_SIZE, threads,
                               MHD_OPTION_END);
    running_ = daemon_ != nullptr;
    return running_;
}

void HttpServer::stop() {
    if (running_) {
        MHD_stop_daemon(daemon_);
        daemon_ = nullptr;
        running_ = false;
    }
}

MHD_Result HttpServer::handleRequest(void *cls,
                                     struct MHD_Connection *connection,
                                     const char *url,
                                     const char *method,
                                     const char *version,
                                     const char *upload_data,
                                     size_t *upload_data_size,
                                     void **con_cls) {
    HttpServer *server = static_cast<HttpServer *>(cls);
    (void)version;
    (void)upload_data;
    (void)upload_data_size;
    (void)con_cls;

    if (std::strcmp(method, "GET") != 0) {
        return MHD_NO;
    }

    if (!server->username_.empty()) {
        char *pass = nullptr;
        char *user = MHD_basic_auth_get_username_password(connection, &pass);
        bool ok = user && pass &&
                   server->username_ == user && server->password_ == pass;
        if (user) {
            MHD_free(user);
        }
        if (pass) {
            MHD_free(pass);
        }
        if (!ok) {
            struct MHD_Response *response = MHD_create_response_from_buffer(0, (void *)"", MHD_RESPMEM_PERSISTENT);
            int ret = MHD_queue_basic_auth_fail_response(connection, "scastd", response);
            MHD_destroy_response(response);
            return static_cast<MHD_Result>(ret);
        }
    }

    const char *page = nullptr;
    const char *content_type = nullptr;

    if (std::strncmp(url, kApiVersion, std::strlen(kApiVersion)) == 0) {
        url += std::strlen(kApiVersion);
    }

    if (std::strcmp(url, "/status.json") == 0) {
        page = kJsonResponse;
        content_type = "application/json";
    } else if (std::strcmp(url, "/status.xml") == 0) {
        page = kXmlResponse;
        content_type = "application/xml";
    } else if (std::strcmp(url, "/uptime") == 0) {
        auto uptime = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - server->start_time_).count();
        std::string uptime_str = "{\"uptime\":" + std::to_string(uptime) + "}";
        struct MHD_Response *response = MHD_create_response_from_buffer(
            uptime_str.size(), (void *)uptime_str.c_str(), MHD_RESPMEM_MUST_COPY);
        MHD_add_response_header(response, "Content-Type", "application/json");
        int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);
        return static_cast<MHD_Result>(ret);
    }

    if (!page) {
        const char *not_found = _("Not Found");
        struct MHD_Response *response = MHD_create_response_from_buffer(
            std::strlen(not_found), (void *)not_found, MHD_RESPMEM_PERSISTENT);
        MHD_add_response_header(response, "Content-Type", "text/plain");
        int ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
        MHD_destroy_response(response);
        return static_cast<MHD_Result>(ret);
    }

    struct MHD_Response *response = MHD_create_response_from_buffer(
        std::strlen(page), (void *)page, MHD_RESPMEM_PERSISTENT);
    MHD_add_response_header(response, "Content-Type", content_type);
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return static_cast<MHD_Result>(ret);
}

} // namespace scastd

