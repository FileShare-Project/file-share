/*
** Project FileShare, 2026
**
** Author Francois Michaut
**
** Started on  Fri May 22 17:10:27 2026 Francois Michaut
** Last update Tue May 26 05:34:41 2026 Francois Michaut
**
** utils.cpp : Utility functions
*/

#include <algorithm>
#include <cstddef>
#include <string>

void string_trim(std::string &str) {
    bool found_first_non_space = false;
    const auto is_space_wrapper = [](auto chr) { return std::isspace(chr); };
    const auto remove_first_spaces = [&](auto chr) {
        if (!std::isspace(chr)) {
            found_first_non_space = true;
        }
        return !found_first_non_space;
    };

    const auto last_non_space = std::ranges::find_if_not(str.rbegin(), str.rend(), is_space_wrapper);
    const auto tmp_end = std::remove_if(str.begin(), last_non_space.base(), remove_first_spaces);
    const auto tmp_str = std::string_view(str.begin(), tmp_end);

    str.erase(tmp_end, str.end());
}

void string_lowercase(std::string &str) {
    std::ranges::transform(str, str.begin(), [](unsigned char chr){ return std::tolower(chr); });
}
