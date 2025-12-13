#include "utils.h"
#include <cctype>
#include <string>

static bool is_space(unsigned char c) {
    return std::isspace(c) != 0;
}
static char to_lower(unsigned char c) {
    return static_cast<char>(std::tolower(c));
}

std::string normalize(const std::string& s) {
    std::string res;
    res.reserve(s.size());
    bool lastSpace = false;
    for (unsigned char c : s) {
        if (is_space(c)) {
            if (!lastSpace) {
                res.push_back(' ');
                lastSpace = true;
            }
        } else {
            res.push_back(to_lower(c));
            lastSpace = false;
        }
    }
    if (!res.empty() && res.front() == ' ') res.erase(res.begin());
    if (!res.empty() && res.back() == ' ') res.pop_back();
    return res;
}