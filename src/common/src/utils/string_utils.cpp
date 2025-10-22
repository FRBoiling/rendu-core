#include "common/utils/string_utils.h"

namespace common {
    namespace utils {
        // 优化后的字符串分割，减少内存分配
        std::vector<std::string> split(const std::string& str, const std::string& delimiter) {
            std::vector<std::string> tokens;
            
            // 特殊情况处理
            if (delimiter.empty() || str.empty()) {
                tokens.push_back(str);
                return tokens;
            }
            
            size_t pos = 0;
            size_t last_pos = 0;
            
            // 预先分配可能的结果空间以减少动态扩容
            tokens.reserve(str.size() / (delimiter.size() + 1) + 1);
            
            while ((pos = str.find(delimiter, last_pos)) != std::string::npos) {
                if (pos > last_pos) {
                    tokens.push_back(str.substr(last_pos, pos - last_pos));
                }
                last_pos = pos + delimiter.size();
            }
            
            // 添加最后一个部分
            if (last_pos < str.size()) {
                tokens.push_back(str.substr(last_pos));
            }
            
            return tokens;
        }
        
        // 优化后的trim函数，使用std::isspace处理所有空白字符
        std::string trim(const std::string& str) {
            auto first = std::find_if_not(str.begin(), str.end(), 
                                         [](unsigned char c) { return std::isspace(c); });
            
            if (first == str.end()) {
                return "";
            }
            
            auto last = std::find_if_not(str.rbegin(), std::string::const_reverse_iterator(first),
                                        [](unsigned char c) { return std::isspace(c); }).base();
            
            return std::string(first, last);
        }
        
        // 优化后的左修剪函数
        std::string ltrim(const std::string& str) {
            auto first = std::find_if_not(str.begin(), str.end(),
                                         [](unsigned char c) { return std::isspace(c); });
            
            if (first == str.end()) {
                return "";
            }
            
            return std::string(first, str.end());
        }
        
        // 优化后的右修剪函数
        std::string rtrim(const std::string& str) {
            auto last = std::find_if_not(str.rbegin(), str.rend(),
                                        [](unsigned char c) { return std::isspace(c); }).base();
            
            return std::string(str.begin(), last);
        }
        
        // 优化后的转小写函数，避免符号扩展问题
        std::string to_lower(const std::string& str) {
            std::string result = str;
            std::transform(result.begin(), result.end(), result.begin(),
                          [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            return result;
        }
        
        // 优化后的转大写函数，避免符号扩展问题
        std::string to_upper(const std::string& str) {
            std::string result = str;
            std::transform(result.begin(), result.end(), result.begin(),
                          [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
            return result;
        }
        
        // 新增功能：字符串包含检查
        bool contains(const std::string& str, const std::string& substr) {
            return str.find(substr) != std::string::npos;
        }
        
        // 新增功能：字符串替换
        std::string replace(const std::string& str, const std::string& from, const std::string& to) {
            if (from.empty()) {
                return str;
            }
            
            std::string result;
            result.reserve(str.size()); // 预先分配空间
            
            size_t last_pos = 0;
            size_t pos = 0;
            
            while ((pos = str.find(from, last_pos)) != std::string::npos) {
                result.append(str, last_pos, pos - last_pos);
                result.append(to);
                last_pos = pos + from.size();
            }
            
            // 添加剩余部分
            result.append(str, last_pos, str.size() - last_pos);
            
            return result;
        }
        
        // 模板特化实现：字符串转换为数值类型
        template<> 
        int to_number<int>(const std::string& str) {
            return std::stoi(str);
        }
        
        template<> 
        long to_number<long>(const std::string& str) {
            return std::stol(str);
        }
        
        template<> 
        long long to_number<long long>(const std::string& str) {
            return std::stoll(str);
        }
        
        template<> 
        unsigned int to_number<unsigned int>(const std::string& str) {
            return std::stoul(str);
        }
        
        template<> 
        unsigned long to_number<unsigned long>(const std::string& str) {
            return std::stoul(str);
        }
        
        template<> 
        unsigned long long to_number<unsigned long long>(const std::string& str) {
            return std::stoull(str);
        }
        
        template<> 
        float to_number<float>(const std::string& str) {
            return std::stof(str);
        }
        
        template<> 
        double to_number<double>(const std::string& str) {
            return std::stod(str);
        }
        
        // 模板特化实现：数值类型转换为字符串
        template<> 
        std::string to_string<int>(const int& value) {
            return std::to_string(value);
        }
        
        template<> 
        std::string to_string<long>(const long& value) {
            return std::to_string(value);
        }
        
        template<> 
        std::string to_string<long long>(const long long& value) {
            return std::to_string(value);
        }
        
        template<> 
        std::string to_string<unsigned int>(const unsigned int& value) {
            return std::to_string(value);
        }
        
        template<> 
        std::string to_string<unsigned long>(const unsigned long& value) {
            return std::to_string(value);
        }
        
        template<> 
        std::string to_string<unsigned long long>(const unsigned long long& value) {
            return std::to_string(value);
        }
        
        template<> 
        std::string to_string<float>(const float& value) {
            return std::to_string(value);
        }
        
        template<> 
        std::string to_string<double>(const double& value) {
            return std::to_string(value);
        }
    } // namespace utils
} // namespace common