#pragma once

#include "common/define.h"
#include <string>
#include <vector>
#include <string_view>
#include <fmt/format.h>

BEGIN_NAMESPACE_COMMON
namespace str {

/**
 * @brief 去除字符串首尾空白字符
 * @param s 输入字符串
 * @return 去除空白后的字符串
 */
std::string trim(std::string_view s);

/**
 * @brief 分割字符串
 * @param s 输入字符串
 * @param delim 分隔符
 * @return 分割后的字符串数组
 */
std::vector<std::string> split(std::string_view s, char delim);

/**
 * @brief 连接字符串数组
 * @param parts 字符串数组
 * @param delim 连接符
 * @return 连接后的字符串
 */
std::string join(const std::vector<std::string>& parts, std::string_view delim);

/**
 * @brief 转换为小写
 * @param s 输入字符串
 * @return 小写字符串
 */
std::string to_lower(std::string_view s);

/**
 * @brief 转换为大写
 * @param s 输入字符串
 * @return 大写字符串
 */
std::string to_upper(std::string_view s);

/**
 * @brief 检查是否以指定前缀开头
 * @param s 输入字符串
 * @param prefix 前缀
 * @return 是否匹配
 */
bool starts_with(std::string_view s, std::string_view prefix);

/**
 * @brief 检查是否以指定后缀结尾
 * @param s 输入字符串
 * @param suffix 后缀
 * @return 是否匹配
 */
bool ends_with(std::string_view s, std::string_view suffix);

/**
 * @brief 检查是否包含子串
 * @param s 输入字符串
 * @param sub 子串
 * @return 是否包含
 */
bool contains(std::string_view s, std::string_view sub);

/**
 * @brief 替换子串
 * @param s 输入字符串
 * @param from 被替换的子串
 * @param to 替换的子串
 * @return 替换后的字符串
 */
std::string replace(std::string_view s, std::string_view from, std::string_view to);

/**
 * @brief 格式化字符串
 * @param fmt 格式化字符串，使用 {} 作为占位符
 * @param args 可变参数
 * @return 格式化后的字符串
 */
template <typename... Args>
std::string format(fmt::format_string<Args...> fmt, Args&&... args) {
    return fmt::format(fmt, std::forward<Args>(args)...);
}

} // namespace str
END_NAMESPACE_COMMON
