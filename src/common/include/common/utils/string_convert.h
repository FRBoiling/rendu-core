//
// Created by 沸腾 on 2025/11/22.
//

#ifndef RENDU_STRING_CONVERT_H
#define RENDU_STRING_CONVERT_H

#include "common/define.h"
#include "optional.h"
#include "errors.h"
#include "types.h"

#include <charconv>

#include "string_utils.h"

BEGIN_NAMESPACE_COMMON
    namespace Utils
    {
        namespace Impl::StringConvertImpl
        {
            template <typename T, typename = void>
            struct For
            {
                static_assert(dependant_false_v<T>, "Unsupported type used for ToString or StringTo");
                /*
                static Optional<T> FromString(std::string_view str, ...);
                static std::string ToString(T&& val, ...);
                */
            };

            template <typename T>
            struct For<T, std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>>>
            {
                static Optional<T> FromString(std::string_view str, int base = 10)
                {
                    if (base == 0)
                    {
                        if (Utils::StringEqualI(str.substr(0, 2), "0x"))
                        {
                            base = 16;
                            str.remove_prefix(2);
                        }
                        else if (Utils::StringEqualI(str.substr(0, 2), "0b"))
                        {
                            base = 2;
                            str.remove_prefix(2);
                        }
                        else
                            base = 10;

                        if (str.empty())
                            return std::nullopt;
                    }

                    char const* const start = str.data();
                    char const* const end = (start + str.length());

                    T val;
                    std::from_chars_result const res = std::from_chars(start, end, val, base);
                    if ((res.ptr == end) && (res.ec == std::errc()))
                        return val;
                    else
                        return std::nullopt;
                }

                static std::string ToString(T val)
                {
                    using buffer_size = std::integral_constant<size_t, sizeof(T) < 8 ? 11 : 20>;

                    std::string buf(buffer_size::value, '\0');
                    /* 2^64 is 20 decimal characters, -(2^63) is 20 including the sign */
                    char* const start = buf.data();
                    char* const end = (start + buf.length());
                    std::to_chars_result const res = std::to_chars(start, end, val);
                    ASSERT(res.ec == std::errc());
                    buf.resize(res.ptr - start);
                    return buf;
                }
            };

            template <>
            struct For<bool, void>
            {
                static Optional<bool> FromString(std::string_view str, int strict = 0)
                /* this is int to match the signature for "proper" integral types */
                {
                    if (strict)
                    {
                        if (str == "1")
                            return true;
                        if (str == "0")
                            return false;
                        return std::nullopt;
                    }
                    else
                    {
                        if ((str == "1") || Utils::StringEqualI(str, "y") || Utils::StringEqualI(str, "on") ||
                            Utils::StringEqualI(str, "yes") ||
                            Utils::StringEqualI(str, "true"))
                            return true;
                        if ((str == "0") || Utils::StringEqualI(str, "n") || Utils::StringEqualI(str, "off") ||
                            Utils::StringEqualI(str, "no") ||
                            Utils::StringEqualI(str, "false"))
                            return false;
                        return std::nullopt;
                    }
                }

                static std::string ToString(bool val)
                {
                    return (val ? "1" : "0");
                }
            };

#if RENDU_COMPILER == RENDU_COMPILER_MICROSOFT
            template <typename T>
            struct For<T, std::enable_if_t<std::is_floating_point_v<T>>>
            {
                static Optional<T> FromString(std::string_view str, std::chars_format fmt = std::chars_format())
                {
                    if (str.empty())
                        return std::nullopt;

                    if (fmt == std::chars_format())
                    {
                        if (StringEqualI(str.substr(0, 2), "0x"))
                        {
                            fmt = std::chars_format::hex;
                            str.remove_prefix(2);
                        }
                        else
                            fmt = std::chars_format::general;

                        if (str.empty())
                            return std::nullopt;
                    }

                    char const* const start = str.data();
                    char const* const end = (start + str.length());

                    T val;
                    std::from_chars_result const res = std::from_chars(start, end, val, fmt);
                    if ((res.ptr == end) && (res.ec == std::errc()))
                        return val;
                    else
                        return std::nullopt;
                }

                // this allows generic converters for all numeric types (easier templating!)
                static Optional<T> FromString(std::string_view str, int base)
                {
                    if (base == 16)
                        return FromString(str, std::chars_format::hex);
                    else if (base == 10)
                        return FromString(str, std::chars_format::general);
                    else
                        return FromString(str, std::chars_format());
                }

                static std::string ToString(T val)
                {
                    return std::to_string(val);
                }
            };
#else
            // @todo replace this once libc++ supports double args to from_chars
            template <typename T>
            struct For<T, std::enable_if_t<std::is_floating_point_v<T>>>
            {
                static Optional<T> FromString(std::string_view str, int base = 0)
                {
                    try
                    {
                        if (str.empty())
                            return std::nullopt;

                        if ((base == 10) && Utils::StringEqualI(str.substr(0, 2), "0x"))
                            return std::nullopt;

                        std::string tmp;
                        if (base == 16)
                            tmp.append("0x");
                        tmp.append(str);

                        size_t n;
                        T val = static_cast<T>(std::stold(tmp, &n));
                        if (n != tmp.length())
                            return std::nullopt;
                        return val;
                    }
                    catch (...) { return std::nullopt; }
                }

                static std::string ToString(T val)
                {
                    return std::to_string(val);
                }
            };
#endif
        }


        template <typename Result, typename... Params>
        RC_COMMON_API constexpr Optional<Result> StringTo(std::string_view str, Params&&... params)
        {
            return Impl::StringConvertImpl::For<Result>::FromString(str, std::forward<Params>(params)...);
        }

        template <typename Type, typename... Params>
        RC_COMMON_API constexpr std::string ToString(Type&& val, Params&&... params)
        {
            return Impl::StringConvertImpl::For<std::decay_t<Type>>::ToString(
                std::forward<Type>(val), std::forward<Params>(params)...);
        }

        template <typename Enum, typename... Args>
        RC_COMMON_API constexpr Enum StringToEnum(Enum defaultValue, std::string_view str, Args&&... args)
        {
            return static_cast<Enum>(StringTo<uint8>(str, std::forward<Args>(args)...)
                .value_or(static_cast<uint8>(defaultValue))
            );
        }
    }

END_NAMESPACE_COMMON

#endif //RENDU_STRING_CONVERT_H
