//
// Created by 沸腾 on 2025/10/24.
//

#include "common/utils/string_format.h"
#include <fmt/format.h>

BEGIN_NAMESPACE_COMMON
namespace Utils
{
    namespace Impl
    {
        std::string StringVFormat(FormatStringView fmt, FormatArgs args) noexcept
        try
        {
            return fmt::vformat(fmt, args);
        }
        catch (std::exception const& formatError)
        {
            return fmt::format(R"(An error occurred formatting string "{}" : {})", fmt, formatError.what());
        }

        void StringVFormatToImpl(fmt::detail::buffer<char>& buffer, FormatStringView fmt, FormatArgs args) noexcept
        try
        {
            fmt::detail::vformat_to(buffer, fmt, args, {});
        }
        catch (std::exception const& formatError)
        {
            fmt::detail::vformat_to(buffer, FormatStringView(R"(An error occurred formatting string "{}" : {})"),
                                    MakeFormatArgs(fmt, formatError.what()), {});
        }
    }
}
END_NAMESPACE_COMMON

// explicit template instantiations
template struct RC_COMMON_API fmt::formatter<int>;
template struct RC_COMMON_API fmt::formatter<unsigned>;
template struct RC_COMMON_API fmt::formatter<long long>;
template struct RC_COMMON_API fmt::formatter<unsigned long long>;
template struct RC_COMMON_API fmt::formatter<bool>;
template struct RC_COMMON_API fmt::formatter<char>;
template struct RC_COMMON_API fmt::formatter<float>;
template struct RC_COMMON_API fmt::formatter<double>;
template struct RC_COMMON_API fmt::formatter<long double>;
template struct RC_COMMON_API fmt::formatter<char const*>;
template struct RC_COMMON_API fmt::formatter<void const*>;
template struct RC_COMMON_API fmt::formatter<fmt::basic_string_view<char>>;

template RC_COMMON_API fmt::appender fmt::detail::native_formatter<int, char, fmt::detail::type::int_type>::format<
    fmt::format_context>(int const&, format_context&) const;
template RC_COMMON_API fmt::appender fmt::detail::native_formatter<unsigned, char, fmt::detail::type::uint_type>::format
<fmt::format_context>(unsigned const&, format_context&) const;
template RC_COMMON_API fmt::appender fmt::detail::native_formatter<
    long long, char, fmt::detail::type::long_long_type>::format<fmt::format_context>(
    long long const&, format_context&) const;
template RC_COMMON_API fmt::appender fmt::detail::native_formatter<
    unsigned long long, char, fmt::detail::type::ulong_long_type>::format<fmt::format_context>(
    unsigned long long const&, format_context&) const;
template RC_COMMON_API fmt::appender fmt::detail::native_formatter<bool, char, fmt::detail::type::bool_type>::format<
    fmt::format_context>(bool const&, format_context&) const;
template RC_COMMON_API fmt::appender fmt::detail::native_formatter<char, char, fmt::detail::type::char_type>::format<
    fmt::format_context>(char const&, format_context&) const;
template RC_COMMON_API fmt::appender fmt::detail::native_formatter<float, char, fmt::detail::type::float_type>::format<
    fmt::format_context>(float const&, format_context&) const;
template RC_COMMON_API fmt::appender fmt::detail::native_formatter<double, char, fmt::detail::type::double_type>::format
<fmt::format_context>(double const&, format_context&) const;
template RC_COMMON_API fmt::appender fmt::detail::native_formatter<
    long double, char, fmt::detail::type::long_double_type>::format<fmt::format_context>(
    long double const&, format_context&) const;
template RC_COMMON_API fmt::appender fmt::detail::native_formatter<
    char const*, char, fmt::detail::type::cstring_type>::format<fmt::format_context>(
    char const* const&, format_context&) const;
template RC_COMMON_API fmt::appender fmt::detail::native_formatter<
    void const*, char, fmt::detail::type::pointer_type>::format<fmt::format_context>(
    void const* const&, format_context&) const;
template RC_COMMON_API fmt::appender fmt::detail::native_formatter<
    fmt::string_view, char, fmt::detail::type::string_type>::format<fmt::format_context>(
    string_view const&, format_context&) const;
