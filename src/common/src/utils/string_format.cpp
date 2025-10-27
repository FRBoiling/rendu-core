//
// Created by 沸腾 on 2025/10/24.
//

#include "common/utils/string_format.h"
#include <fmt/format.h>

// 定义实现类
BEGIN_NAMESPACE_COMMON
    namespace Utils
    {
        class FormatImpl
        {
        public:
            template <typename... Args>
            static FormatArgs MakeFormatArgsImpl(Args&&... args)
            {
                FormatArgs result;
                // 存储 fmt::format_args 的实际实现
                auto args_ptr = new fmt::format_args(fmt::make_format_args(args...));
                result.impl_ = args_ptr;
                return result;
            }

            static fmt::format_args GetFormatArgs(const FormatArgs& args)
            {
                return *static_cast<fmt::format_args*>(args.impl_);
            }
        };

        // FormatArgs 移动构造和赋值
        FormatArgs::FormatArgs(FormatArgs&& other) noexcept : impl_(other.impl_)
        {
            other.impl_ = nullptr;
        }

        FormatArgs& FormatArgs::operator=(FormatArgs&& other) noexcept
        {
            if (this != &other)
            {
                delete static_cast<fmt::format_args*>(impl_);
                impl_ = other.impl_;
                other.impl_ = nullptr;
            }
            return *this;
        }

        FormatArgs::~FormatArgs()
        {
            if (impl_)
            {
                delete static_cast<fmt::format_args*>(impl_);
            }
        }

        // MakeFormatArgs 实现
        template <typename... Args>
        FormatArgs MakeFormatArgs(Args&&... args)
        {
            return FormatImpl::MakeFormatArgsImpl(std::forward<Args>(args)...);
        }

        namespace Impl
        {
            std::string StringVFormat(FormatStringView fmt, FormatArgs args) noexcept
            try
            {
                // 确保格式字符串有效
                if (!fmt.data() || fmt.size() == 0)
                    return "[Empty Format String]";
                
                return fmt::vformat(fmt, FormatImpl::GetFormatArgs(args));
            }
            catch (std::exception const& formatError)
            {
                try
                {
                    // 使用更安全的错误处理方式
                    std::string errorMsg = "Format error: ";
                    errorMsg += formatError.what();
                    return errorMsg;
                }
                catch (...)
                {
                    return "[Format Error]";
                }
            }
        }

        // StringVFormatTo 实现
        template <typename OutputIt>
        OutputIt StringVFormatTo(OutputIt out, FormatStringView fmt, FormatArgs args) noexcept
        {
            auto&& buf = fmt::detail::get_buffer<char>(out);
            try
            {
                // 确保格式字符串有效
                if (!fmt.data() || fmt.size() == 0)
                {
                    fmt::detail::vformat_to(buf, "[Empty Format String]", fmt::format_args(fmt::make_format_args()), {});
                }
                else
                {
                    fmt::detail::vformat_to(buf, fmt, FormatImpl::GetFormatArgs(args), {});
                }
            }
            catch (const fmt::format_error& e)
            {
                // 使用直接字符串拼接代替fmt格式化
                std::string errorMsg = "Format error: ";
                errorMsg += e.what();
                for (const char c : errorMsg) {
                    *out++ = c;
                }
                return out;
            }
            return fmt::detail::get_iterator(buf, out);
        }

        // 为常用类型提供完整的模板实例化声明
        // 单参数版本
        template FormatArgs MakeFormatArgs<const char*>(const char*&&);
        template FormatArgs MakeFormatArgs<const char*&>(const char*&);
        template FormatArgs MakeFormatArgs<std::string>(std::string&&);
        template FormatArgs MakeFormatArgs<std::string&>(std::string&);
        template FormatArgs MakeFormatArgs<std::string_view>(std::string_view&&);
        template FormatArgs MakeFormatArgs<std::string_view&>(std::string_view&);
        
        // 两个参数的组合
        template FormatArgs MakeFormatArgs<const char*, const char*>(const char*&&, const char*&&);
        template FormatArgs MakeFormatArgs<const char*, const char*&>(const char*&&, const char*&);
        template FormatArgs MakeFormatArgs<const char*&, const char*>(const char*&, const char*&&);
        template FormatArgs MakeFormatArgs<const char*&, const char*&>(const char*&, const char*&);
        template FormatArgs MakeFormatArgs<const char*, std::string>(const char*&&, std::string&&);
        template FormatArgs MakeFormatArgs<const char*, std::string&>(const char*&&, std::string&);
        template FormatArgs MakeFormatArgs<const char*&, std::string>(const char*&, std::string&&);
        template FormatArgs MakeFormatArgs<const char*&, std::string&>(const char*&, std::string&);
        template FormatArgs MakeFormatArgs<std::string, const char*>(std::string&&, const char*&&);
        template FormatArgs MakeFormatArgs<std::string, const char*&>(std::string&&, const char*&);
        template FormatArgs MakeFormatArgs<std::string&, const char*>(std::string&, const char*&&);
        template FormatArgs MakeFormatArgs<std::string&, const char*&>(std::string&, const char*&);
        template FormatArgs MakeFormatArgs<int, std::string>(int&&, std::string&&);
        template FormatArgs MakeFormatArgs<const char*, int>(const char*&&, int&&);

        // 为常用输出迭代器提供模板实例化声明
        template std::back_insert_iterator<std::string> StringVFormatTo<std::back_insert_iterator<std::string>>(
            std::back_insert_iterator<std::string>, FormatStringView, FormatArgs) noexcept;
    }

END_NAMESPACE_COMMON

// 为 Optional<T> 提供 fmt formatter 支持
namespace fmt
{
    template <typename T, typename Char>
    struct formatter<Optional<T>, Char> : formatter<T, Char>
    {
        template <typename FormatContext>
        auto format(Optional<T> const& value, FormatContext& ctx) const -> decltype(ctx.out())
        {
            if (value.has_value())
                return formatter<T, Char>::format(*value, ctx);

            return formatter<string_view, Char>().format("(nullopt)", ctx);
        }
    };
}

// allow implicit enum to int conversions for formatting
namespace Common
{
    namespace Utils
    {
        template <typename E, std::enable_if_t<std::is_enum_v<E>, std::nullptr_t> = nullptr>
        inline constexpr auto format_as(E e) { return static_cast<std::underlying_type_t<E>>(e); }
    }
}

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