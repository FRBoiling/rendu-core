//
// Created by 沸腾 on 2025/10/24.
//

#ifndef RENDU_STRING_FORMAT_H
#define RENDU_STRING_FORMAT_H

#include <fmt/core.h>
#include "common/define.h"
#include "optional.h"

BEGIN_NAMESPACE_COMMON
    namespace Utils
    {
        template <typename... Args>
        using FormatString = fmt::format_string<Args...>;

        using FormatStringView = fmt::string_view;

        using FormatArgs = fmt::format_args;

        template <typename... Args>
        constexpr auto MakeFormatArgs(Args&&... args) { return fmt::make_format_args(args...); }

        namespace Impl
        {
            RC_COMMON_API std::string StringVFormat(FormatStringView fmt, FormatArgs args) noexcept;

            RC_COMMON_API void StringVFormatToImpl(fmt::detail::buffer<char>& buffer, FormatStringView fmt,
                                                   FormatArgs args) noexcept;
        }

        using Impl::StringVFormat;

        template <typename OutputIt>
        inline OutputIt StringVFormatTo(OutputIt out, FormatStringView fmt, FormatArgs args) noexcept
        {
            auto&& buf = fmt::detail::get_buffer<char>(out);
            Impl::StringVFormatToImpl(buf, fmt, args);
            return fmt::detail::get_iterator(buf, out);
        }

        /// Default TC string format function.
        template <typename... Args>
        inline std::string StringFormat(FormatString<Args...> fmt, Args&&... args) noexcept
        {
            return StringVFormat(fmt, MakeFormatArgs(std::forward<Args>(args)...));
        }

        template <typename OutputIt, typename... Args>
        inline OutputIt StringFormatTo(OutputIt out, FormatString<Args...> fmt, Args&&... args) noexcept
        {
            return StringVFormatTo(out, fmt, MakeFormatArgs(std::forward<Args>(args)...));
        }

        /// Returns true if the given char pointer is null.
        inline bool IsFormatEmptyOrNull(char const* fmt)
        {
            return fmt == nullptr;
        }

        /// Returns true if the given std::string is empty.
        inline bool IsFormatEmptyOrNull(std::string const& fmt)
        {
            return fmt.empty();
        }

        /// Returns true if the given std::string_view is empty.
        inline constexpr bool IsFormatEmptyOrNull(std::string_view fmt)
        {
            return fmt.empty();
        }

        inline constexpr bool IsFormatEmptyOrNull(fmt::string_view fmt)
        {
            return fmt.size() == 0;
        }
    }

END_NAMESPACE_COMMON

template <typename T, typename Char>
struct fmt::formatter<Optional<T>, Char> : formatter<T, Char>
{
    template <typename FormatContext>
    auto format(Optional<T> const& value, FormatContext& ctx) const -> decltype(ctx.out())
    {
        if (value.has_value())
            return formatter<T, Char>::format(*value, ctx);

        return formatter<string_view, Char>().format("(nullopt)", ctx);
    }
};

// allow implicit enum to int conversions for formatting
template <typename E, std::enable_if_t<std::is_enum_v<E>, std::nullptr_t> = nullptr>
inline constexpr auto format_as(E e) { return static_cast<std::underlying_type_t<E>>(e); }

#endif //RENDU_STRING_FORMAT_H
