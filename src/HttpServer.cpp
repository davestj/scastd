#include "HttpServer.h"

#include <cstring>
#include <csignal>
#include <string>
#include <unistd.h>
#include <cstdlib>

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
}

HttpServer::HttpServer() : running_(false), daemon_(nullptr) {}

HttpServer::~HttpServer() {
    stop();
}

bool HttpServer::start(int port) {
#if defined(__APPLE__) || defined(__linux__)
    const char *no_daemon = std::getenv("SCASD_NO_DAEMON");
    if (!no_daemon || std::strcmp(no_daemon, "1") != 0) {
        if (daemon(0, 0) != 0) {
            return false;
        }
    }
#endif
    g_server = this;
    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);

    daemon_ = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD,
                               port,
                               nullptr, nullptr,
                               &HttpServer::handleRequest, this,
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
    (void)cls;
    (void)version;
    (void)upload_data;
    (void)upload_data_size;
    (void)con_cls;

    if (std::strcmp(method, "GET") != 0) {
        return MHD_NO;
    }

    const char *page = nullptr;
    const char *content_type = nullptr;

    if (std::strcmp(url, "/status.json") == 0) {
        page = kJsonResponse;
        content_type = "application/json";
    } else if (std::strcmp(url, "/status.xml") == 0) {
        page = kXmlResponse;
        content_type = "application/xml";
    } else {
        static const char not_found[] = "Not Found";
        struct MHD_Response *response = MHD_create_response_from_buffer(
            sizeof(not_found) - 1, (void *)not_found, MHD_RESPMEM_PERSISTENT);
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

