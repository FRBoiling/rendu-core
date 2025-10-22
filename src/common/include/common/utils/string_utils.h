#pragma once

#include <fmt/core.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <iterator>
#include <string_view>

namespace common {
    namespace utils {
        // 字符串格式化工具 - 已优化支持运行时格式字符串
        template<typename S, typename... Args>
        std::string format(const S& format_str, Args&&... args) {
            return fmt::vformat(fmt::string_view(format_str), fmt::make_format_args(args...));
        }
        
        // 字符串分割工具
        std::vector<std::string> split(const std::string& str, const std::string& delimiter);
        
        // 字符串修剪工具
        std::string trim(const std::string& str);
        std::string ltrim(const std::string& str);
        std::string rtrim(const std::string& str);
        
        // 字符串大小写转换
        std::string to_lower(const std::string& str);
        std::string to_upper(const std::string& str);
        
        // 新增功能：字符串包含检查
        bool contains(const std::string& str, const std::string& substr);
        
        // 新增功能：字符串替换
        std::string replace(const std::string& str, const std::string& from, const std::string& to);
        
        // 新增功能：字符串连接
        template<typename... Args>
        std::string join(const Args&... args) {
            std::string result;
            (result.append(args), ...);
            return result;
        }
        
        // 新增功能：字符串转换为数值类型
        template<typename T> 
        T to_number(const std::string& str);
        
        // 新增功能：数值类型转换为字符串
        template<typename T> 
        std::string to_string(const T& value);
    } // namespace utils
} // namespace common