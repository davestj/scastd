#include "icecast2.h"

#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <cstdlib>

namespace {
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t totalSize = size * nmemb;
    std::string *s = static_cast<std::string *>(userp);
    s->append(static_cast<char *>(contents), totalSize);
    return totalSize;
}
}

Icecast2::Icecast2(const std::string &h,
                   int p,
                   const std::string &user,
                   const std::string &pass)
    : host(h), port(p), username(user), password(pass) {}

bool Icecast2::fetchStats(std::vector<StreamInfo> &stats, std::string &error) const {
    CURL *curl = curl_easy_init();
    if (!curl) {
        error = "Failed to initialize curl";
        return false;
    }

    std::string url = "http://" + host + ":" + std::to_string(port) + "/admin/stats.xml";
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "scastd/1.0");

    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    }
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        error = curl_easy_strerror(res);
        return false;
    }
    if (http_code != 200) {
        error = "HTTP response code " + std::to_string(http_code);
        return false;
    }

    xmlDocPtr doc = xmlParseMemory(response.c_str(), response.size());
    if (!doc) {
        error = "Failed to parse XML";
        return false;
    }

    xmlNodePtr root = xmlDocGetRootElement(doc);
    if (!root || xmlStrcmp(root->name, BAD_CAST "icestats") != 0) {
        xmlFreeDoc(doc);
        error = "Unexpected XML format";
        return false;
    }

    for (xmlNodePtr node = root->children; node; node = node->next) {
        if (node->type != XML_ELEMENT_NODE) {
            continue;
        }
        if (xmlStrcmp(node->name, BAD_CAST "source") == 0) {
            StreamInfo info;
            xmlChar *mount = xmlGetProp(node, BAD_CAST "mount");
            if (mount) {
                info.mount = reinterpret_cast<char *>(mount);
                xmlFree(mount);
            }
            info.listeners = 0;
            info.bitrate = 0;
            for (xmlNodePtr child = node->children; child; child = child->next) {
                if (child->type != XML_ELEMENT_NODE) {
                    continue;
                }
                xmlChar *content = xmlNodeGetContent(child);
                if (!content) {
                    continue;
                }
                if (xmlStrcmp(child->name, BAD_CAST "listeners") == 0) {
                    info.listeners = std::atoi(reinterpret_cast<char *>(content));
                } else if (xmlStrcmp(child->name, BAD_CAST "bitrate") == 0) {
                    info.bitrate = std::atoi(reinterpret_cast<char *>(content));
                } else if (xmlStrcmp(child->name, BAD_CAST "title") == 0) {
                    info.title = reinterpret_cast<char *>(content);
                }
                xmlFree(content);
            }
            stats.push_back(info);
        }
    }

    xmlFreeDoc(doc);
    return true;
}

