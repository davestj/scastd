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
#include "icecast2.h"

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <cstdlib>

namespace scastd {

Icecast2::Icecast2(const std::string &h,
                   int p,
                   const std::string &user,
                   const std::string &pass,
                   const CurlClient &client)
    : host(h), port(p), username(user), password(pass), http(client) {}

bool Icecast2::fetchStats(std::vector<StreamInfo> &stats, std::string &error) const {
    std::string url = "http://";
    if (!username.empty() || !password.empty()) {
        url += username + ":" + password + "@";
    }
    url += host + ":" + std::to_string(port) + "/admin/stats.xml";

    std::string response;
    long http_code = 0;
    if (!http.fetchUrl(url, response, 0, false, "scastd/1.0", &http_code)) {
        error = "Failed to fetch stats";
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

} // namespace scastd

