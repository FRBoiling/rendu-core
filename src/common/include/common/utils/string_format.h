//
// Created by 沸腾 on 2025/10/24.
//

#ifndef RENDU_STRING_FORMAT_H
#define RENDU_STRING_FORMAT_H

#include "common/define.h"
#include "optional.h"
#include <string>
#include <string_view>

BEGIN_NAMESPACE_COMMON
    namespace Utils
    {
        // 自定义类型替代 fmt 类型
        template <typename... Args>
        class FormatString
        {
        public:
            explicit FormatString(const char* str) : str_(str) {}
            explicit FormatString(std::string_view str) : str_(str.data(), str.size()) {}
            
            const char* data() const { return str_.c_str(); }
            size_t size() const { return str_.size(); }
            
        private:
            std::string str_;
        };
        
        using FormatStringView = std::string_view;
        
        // 用于包装 format_args 的不透明类型
        class FormatArgs
        {
        public:
            FormatArgs() = default;
            FormatArgs(const FormatArgs&) = delete;
            FormatArgs& operator=(const FormatArgs&) = delete;
            FormatArgs(FormatArgs&&) noexcept;
            FormatArgs& operator=(FormatArgs&&) noexcept;
            ~FormatArgs();
        
        private:
            friend class FormatImpl;
            void* impl_ = nullptr;
        };
        
        // 前向声明 MakeFormatArgs 函数模板
        template <typename... Args>
        FormatArgs MakeFormatArgs(Args&&... args);
        
        namespace Impl
        {
            RC_COMMON_API std::string StringVFormat(FormatStringView fmt, FormatArgs args) noexcept;
        }
        
        using Impl::StringVFormat;
        
        // 简化版本的 StringVFormatTo 函数声明
        template <typename OutputIt>
        OutputIt StringVFormatTo(OutputIt out, FormatStringView fmt, FormatArgs args) noexcept;
        
        /// Default TC string format function.
        template <typename... Args>
        std::string StringFormat(const char* fmt, Args&&... args) noexcept
        {
            return StringVFormat(fmt, MakeFormatArgs(std::forward<Args>(args)...));
        }
        
        // 还有其他重载版本
        // 为string_view版本添加实现
        template <typename... Args>
        std::string StringFormat(std::string_view fmt, Args&&... args) noexcept
        {
            return StringVFormat(fmt, MakeFormatArgs(std::forward<Args>(args)...));
        }
        
        // 修复string const&版本，移除末尾分号
        template <typename... Args>
        std::string StringFormat(std::string const& fmt, Args&&... args) noexcept
        {
            return StringVFormat(fmt, MakeFormatArgs(std::forward<Args>(args)...));
        }
        
        // Keep the original version for backward compatibility
        template <typename... Args>
        std::string StringFormat(FormatString<Args...> fmt, Args&&... args) noexcept
        {
            return StringVFormat(FormatStringView(fmt.data(), fmt.size()), MakeFormatArgs(std::forward<Args>(args)...));
        }
        
        template <typename OutputIt, typename... Args>
        OutputIt StringFormatTo(OutputIt out, const char* fmt, Args&&... args) noexcept
        {
            return StringVFormatTo(out, fmt, MakeFormatArgs(std::forward<Args>(args)...));
        }
        
        template <typename OutputIt, typename... Args>
        OutputIt StringFormatTo(OutputIt out, std::string_view fmt, Args&&... args) noexcept
        {
            return StringVFormatTo(out, fmt, MakeFormatArgs(std::forward<Args>(args)...));
        }
        
        template <typename OutputIt, typename... Args>
        OutputIt StringFormatTo(OutputIt out, std::string const& fmt, Args&&... args) noexcept
        {
            return StringVFormatTo(out, fmt, MakeFormatArgs(std::forward<Args>(args)...));
        }
        
        // Keep the original version for backward compatibility
        template <typename OutputIt, typename... Args>
        OutputIt StringFormatTo(OutputIt out, FormatString<Args...> fmt, Args&&... args) noexcept
        {
            return StringVFormatTo(out, FormatStringView(fmt.data(), fmt.size()), MakeFormatArgs(std::forward<Args>(args)...));
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
    }

END_NAMESPACE_COMMON

#endif //RENDU_STRING_FORMAT_H