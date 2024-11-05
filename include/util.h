#pragma once

#include <string>

void replace_all (std::string& str, const std::string& from, const std::string& to) {
    if (from.empty ())
        return;
    size_t pos = 0;
    while ((pos = str.find (from, pos)) != std::string::npos) {
        str.replace (pos, from.length (), to);
        pos += to.length ();
    }
}