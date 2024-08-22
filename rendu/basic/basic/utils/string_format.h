/*
* Created by boil on 2024/8/22.
*/

#ifndef RENDU_BASIC_BASIC_UTILS_STRING_FORMAT_H_
#define RENDU_BASIC_BASIC_UTILS_STRING_FORMAT_H_

#include "type/type_include.h"
#include <fmt/format.h>

RD_NAMESPACE_BEGIN

using memory_buf_t = fmt::basic_memory_buffer<char, 250>;
using StringView = fmt::string_view;

template<typename... Args>
std::string format(const StringView format_string, const Args &...args) {
  memory_buf_t buf;
  fmt::detail::vformat_to(buf, format_string, fmt::make_format_args(args...));
  return String{buf.data(), buf.size()};
}

RD_NAMESPACE_END


#endif//RENDU_BASIC_BASIC_UTILS_STRING_FORMAT_H_
