#pragma once

#include <string>

// Fetch the content at the given URL using libcurl.
// Returns true on success and stores the response body in `response`.
bool fetchUrl(const std::string &url, std::string &response);

