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
            /**
             * @brief 使用fmt库进行字符串格式化
             * @param fmt 格式化字符串视图
             * @param args 格式化参数
             * @return 格式化后的字符串
             * @note 此函数包装了fmt::vformat，提供异常安全处理
             */
            std::string StringVFormat(FormatStringView fmt, FormatArgs args) noexcept
            try
            {
                // 调用fmt库的vformat函数进行实际格式化
                return fmt::vformat(fmt, args);
            }
            catch (std::exception const& formatError)
            {
                // 格式化失败时返回错误信息，避免程序崩溃
                return fmt::format(R"(An error occurred formatting string "{}" : {})", fmt, formatError.what());
            }

            /**
             * @brief 将格式化结果输出到缓冲区
             * @param buffer 目标缓冲区
             * @param fmt 格式化字符串视图
             * @param args 格式化参数
             * @note 此函数用于高性能场景，避免字符串拷贝
             */
            void StringVFormatToImpl(fmt::detail::buffer<char>& buffer, FormatStringView fmt, FormatArgs args) noexcept
            try
            {
                // 直接将格式化结果写入缓冲区
                fmt::detail::vformat_to(buffer, fmt, args, {});
            }
            catch (std::exception const& formatError)
            {
                // 格式化失败时写入错误信息到缓冲区
                fmt::detail::vformat_to(buffer, FormatStringView(R"(An error occurred formatting string "{}" : {})"),
                                        MakeFormatArgs(fmt, formatError.what()), {});
            }
        } // namespace Impl
    }

END_NAMESPACE_COMMON


// 显式模板实例化部分
// 这些实例化确保fmt库能够正确处理各种基本类型的格式化

// 基本整数类型格式化器实例化
template struct RC_COMMON_API fmt::formatter<int>;
template struct RC_COMMON_API fmt::formatter<unsigned>;
template struct RC_COMMON_API fmt::formatter<long long>;
template struct RC_COMMON_API fmt::formatter<unsigned long long>;

// 布尔类型格式化器实例化
template struct RC_COMMON_API fmt::formatter<bool>;

// 字符类型格式化器实例化
template struct RC_COMMON_API fmt::formatter<char>;

// 浮点数类型格式化器实例化
template struct RC_COMMON_API fmt::formatter<float>;
template struct RC_COMMON_API fmt::formatter<double>;
template struct RC_COMMON_API fmt::formatter<long double>;

// 指针和字符串类型格式化器实例化
template struct RC_COMMON_API fmt::formatter<char const*>;
template struct RC_COMMON_API fmt::formatter<void const*>;
template struct RC_COMMON_API fmt::formatter<fmt::basic_string_view<char>>;

// 原生格式化器的模板实例化
// 这些实例化确保fmt库内部能够正确处理各种类型的格式化操作

// 整数类型原生格式化器
template RC_COMMON_API fmt::appender fmt::detail::native_formatter<
    int, char, fmt::detail::type::int_type>::format<
    fmt::format_context>(int const&, format_context&) const;
template RC_COMMON_API fmt::appender fmt::detail::native_formatter<
    unsigned, char, fmt::detail::type::uint_type>::format
<fmt::format_context>(unsigned const&, format_context&) const;

// 长整数类型原生格式化器
template RC_COMMON_API fmt::appender fmt::detail::native_formatter<
    long long, char, fmt::detail::type::long_long_type>::format<fmt::format_context>(
    long long const&, format_context&) const;
template RC_COMMON_API fmt::appender fmt::detail::native_formatter<
    unsigned long long, char, fmt::detail::type::ulong_long_type>::format<fmt::format_context>(
    unsigned long long const&, format_context&) const;

// 布尔类型原生格式化器
template RC_COMMON_API fmt::appender fmt::detail::native_formatter<
    bool, char, fmt::detail::type::bool_type>::format<
    fmt::format_context>(bool const&, format_context&) const;

// 字符类型原生格式化器
template RC_COMMON_API fmt::appender fmt::detail::native_formatter<
    char, char, fmt::detail::type::char_type>::format<
    fmt::format_context>(char const&, format_context&) const;

// 浮点数类型原生格式化器
template RC_COMMON_API fmt::appender fmt::detail::native_formatter<
    float, char, fmt::detail::type::float_type>::format<
    fmt::format_context>(float const&, format_context&) const;
template RC_COMMON_API fmt::appender fmt::detail::native_formatter<
    double, char, fmt::detail::type::double_type>::format
<fmt::format_context>(double const&, format_context&) const;
template RC_COMMON_API fmt::appender fmt::detail::native_formatter<
    long double, char, fmt::detail::type::long_double_type>::format<fmt::format_context>(
    long double const&, format_context&) const;

// 字符串和指针类型原生格式化器
template RC_COMMON_API fmt::appender fmt::detail::native_formatter<
    char const*, char, fmt::detail::type::cstring_type>::format<fmt::format_context>(
    char const* const&, format_context&) const;
template RC_COMMON_API fmt::appender fmt::detail::native_formatter<
    void const*, char, fmt::detail::type::pointer_type>::format<fmt::format_context>(
    void const* const&, format_context&) const;
template RC_COMMON_API fmt::appender fmt::detail::native_formatter<
    fmt::string_view, char, fmt::detail::type::string_type>::format<fmt::format_context>(
    string_view const&, format_context&) const;