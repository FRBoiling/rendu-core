#include <common/util/string.h>
#include <common/define.h>
#include <algorithm>
#include <cctype>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/replace.hpp>

BEGIN_NAMESPACE_COMMON
namespace str {

std::string trim(std::string_view s) {
    std::string str(s);
    boost::algorithm::trim(str);
    return str;
}

std::vector<std::string> split(std::string_view s, char delim) {
    std::vector<std::string> result;
    std::string str(s);
    boost::algorithm::split(result, str, boost::is_any_of(std::string(1, delim)),
                           boost::token_compress_off);
    return result;
}

std::string join(const std::vector<std::string>& parts, std::string_view delim) {
    if (parts.empty()) {
        return "";
    }
    return boost::algorithm::join(parts, std::string(delim));
}

std::string to_lower(std::string_view s) {
    std::string str(s);
    boost::algorithm::to_lower(str);
    return str;
}

std::string to_upper(std::string_view s) {
    std::string str(s);
    boost::algorithm::to_upper(str);
    return str;
}

bool starts_with(std::string_view s, std::string_view prefix) {
#if __cpp_lib_starts_ends_with >= 201711L
    return s.starts_with(prefix);
#else
    if (prefix.size() > s.size()) {
        return false;
    }
    return s.compare(0, prefix.size(), prefix) == 0;
#endif
}

bool ends_with(std::string_view s, std::string_view suffix) {
#if __cpp_lib_ends_with >= 201711L
    return s.ends_with(suffix);
#else
    if (suffix.size() > s.size()) {
        return false;
    }
    return s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
#endif
}

bool contains(std::string_view s, std::string_view sub) {
#if __cpp_lib_string_contains >= 202011L
    return s.contains(sub);
#else
    return s.find(sub) != std::string_view::npos;
#endif
}

std::string replace(std::string_view s, std::string_view from, std::string_view to) {
    if (from.empty()) {
        return std::string(s);
    }
    std::string str(s);
    boost::algorithm::replace_all(str, std::string(from), std::string(to));
    return str;
}

} // namespace str
END_NAMESPACE_COMMON
