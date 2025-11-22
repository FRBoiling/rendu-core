//
// Created by 沸腾 on 2025/10/24.
//

#ifndef RENDU_STRING_FORMAT_H
#define RENDU_STRING_FORMAT_H

#include <string>

#include "common/define.h"
#include <fmt/base.h>

BEGIN_NAMESPACE_COMMON
    namespace Utils
    {
        /**
         * @brief 格式化字符串模板别名，基于fmt库的format_string
         * @tparam Args 格式化参数类型
         * @note 此类型用于类型安全的字符串格式化
         */
        template <typename... Args>
        using FormatString = fmt::format_string<Args...>;

        /**
         * @brief 格式化字符串视图别名，基于fmt库的string_view
         * @note 用于表示不可变的格式化字符串
         */
        using FormatStringView = fmt::string_view;

        /**
         * @brief 格式化参数别名，基于fmt库的format_args
         * @note 用于包装格式化参数，提供类型擦除功能
         */
        using FormatArgs = fmt::format_args;

        /**
         * @brief 创建格式化参数
         * @tparam Args 参数类型
         * @param args 格式化参数
         * @return 格式化参数对象
         * @note 此函数将参数包装为FormatArgs类型
         */
        template <typename... Args>
        constexpr auto MakeFormatArgs(Args&&... args) { return fmt::make_format_args(args...); }

        namespace Impl
        {
            /**
             * @brief 核心字符串格式化函数实现
             * @param fmt 格式化字符串视图
             * @param args 格式化参数
             * @return 格式化后的字符串
             * @note 此函数是字符串格式化的底层实现
             */
            RC_COMMON_API std::string StringVFormat(FormatStringView fmt, FormatArgs args) noexcept;

            /**
             * @brief 将格式化结果输出到缓冲区的实现函数
             * @param buffer 目标缓冲区
             * @param fmt 格式化字符串视图
             * @param args 格式化参数
             * @note 此函数用于高性能场景，避免字符串拷贝
             */
            RC_COMMON_API void StringVFormatToImpl(fmt::detail::buffer<char>& buffer, FormatStringView fmt,
                                                   FormatArgs args) noexcept;
        }

        // 导出实现命名空间中的函数
        using Impl::StringVFormat;

        /**
         * @brief 将格式化结果输出到指定迭代器
         * @tparam OutputIt 输出迭代器类型
         * @param out 输出迭代器
         * @param fmt 格式化字符串视图
         * @param args 格式化参数
         * @return 输出迭代器
         * @note 此函数提供通用的格式化输出功能
         */
        template <typename OutputIt>
        inline OutputIt StringVFormatTo(OutputIt out, FormatStringView fmt, FormatArgs args) noexcept
        {
            auto&& buf = fmt::detail::get_buffer<char>(out);
            Impl::StringVFormatToImpl(buf, fmt, args);
            return fmt::detail::get_iterator(buf, out);
        }

        /**
         * @brief 默认字符串格式化函数
         * @tparam Args 格式化参数类型
         * @param fmt 格式化字符串
         * @param args 格式化参数
         * @return 格式化后的字符串
         * @note 这是主要的字符串格式化接口，支持类型安全的格式化
         */
        template <typename... Args>
        std::string StringFormat(FormatString<Args...> fmt, Args&&... args) noexcept
        {
            return StringVFormat(fmt,MakeFormatArgs(std::forward<Args>(args)...));
        }

        /**
         * @brief 将格式化结果输出到指定迭代器
         * @tparam OutputIt 输出迭代器类型
         * @tparam Args 格式化参数类型
         * @param out 输出迭代器
         * @param fmt 格式化字符串
         * @param args 格式化参数
         * @return 输出迭代器
         * @note 此函数用于高性能场景，避免中间字符串创建
         */
        template <typename OutputIt, typename... Args>
        OutputIt StringFormatTo(OutputIt out, FormatString<Args...> fmt, Args&&... args) noexcept
        {
            return StringVFormatTo(out, fmt, MakeFormatArgs(std::forward<Args>(args)...));
        }

        /**
         * @brief 检查字符指针是否为空或null
         * @param fmt 字符指针
         * @return 如果为空或null返回true，否则返回false
         */
        inline bool IsFormatEmptyOrNull(char const* fmt)
        {
            return fmt == nullptr;
        }

        /**
         * @brief 检查字符串是否为空
         * @param fmt 字符串引用
         * @return 如果为空返回true，否则返回false
         */
        inline bool IsFormatEmptyOrNull(std::string const& fmt)
        {
            return fmt.empty();
        }

        /**
         * @brief 检查字符串视图是否为空
         * @param fmt 字符串视图
         * @return 如果为空返回true，否则返回false
         */
        inline constexpr bool IsFormatEmptyOrNull(std::string_view fmt)
        {
            return fmt.empty();
        }

        /**
         * @brief 检查fmt字符串视图是否为空
         * @param fmt fmt字符串视图
         * @return 如果为空返回true，否则返回false
         */
        inline constexpr bool IsFormatEmptyOrNull(fmt::string_view fmt)
        {
            return fmt.size() == 0;
        }
    }

END_NAMESPACE_COMMON

#endif //RENDU_STRING_FORMAT_H