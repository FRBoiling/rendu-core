/*
* Created by boil on 2023/12/23.
*/

#ifndef RENDU_STRING_FORMAT_H
#define RENDU_STRING_FORMAT_H

#include "base_define.h"
#include <fmt/core.h>
#include <fmt/format.h>

RD_NAMESPACE_BEGIN

#ifdef SPDLOG_USE_STD_FORMAT
namespace fmt_lib = std;

using string_view_t = std::string_view;
using memory_buf_t = std::string;

template<typename... Args>
using format_string_t = std::string_view;

template<class T, class char = char>
struct is_convertible_to_basic_format_string : std::integral_constant<bool, std::is_convertible<T, std::basic_string_view<char>>::value> {};

#if defined(SPDLOG_WCHAR_FILENAMES) || defined(SPDLOG_WCHAR_TO_UTF8_SUPPORT)
using wstring_view_t = std::wstring_view;
using wmemory_buf_t = std::wstring;

template<typename... Args>
using wformat_string_t = std::wstring_view;
#endif
#define SPDLOG_BUF_TO_STRING(x) x
#else// use fmt lib instead of std::format
namespace fmt_lib = fmt;

using string_view_t = fmt::basic_string_view<char>;
using memory_buf_t = fmt::basic_memory_buffer<char, 250>;

template<typename... Args>
using format_string_t = fmt::format_string<Args...>;

template<class T>
using remove_cvref_t = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

// clang doesn't like SFINAE disabled constructor in std::is_convertible<> so have to repeat the condition from basic_format_string here,
// in addition, fmt::basic_runtime<char> is only convertible to basic_format_string<char> but not basic_string_view<char>
template<class T, class Char = char>
struct is_convertible_to_basic_format_string
    : std::integral_constant<bool,
                             std::is_convertible<T, fmt::basic_string_view<char>>::value || std::is_same<remove_cvref_t<T>, fmt::basic_runtime<char>>::value> {};

#if defined(SPDLOG_WCHAR_FILENAMES) || defined(SPDLOG_WCHAR_TO_UTF8_SUPPORT)
using wstring_view_t = fmt::basic_string_view<wchar_t>;
using wmemory_buf_t = fmt::basic_memory_buffer<wchar_t, 250>;

template<typename... Args>
using wformat_string_t = fmt::wformat_string<Args...>;
#endif
#define SPDLOG_BUF_TO_STRING(x) fmt::to_string(x)
#endif

RD_NAMESPACE_END

#endif//RENDU_STRING_FORMAT_H
