#pragma once

#include <atomic>
#include <microhttpd.h>

class HttpServer {
public:
    HttpServer();
    ~HttpServer();

    bool start();
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
};

