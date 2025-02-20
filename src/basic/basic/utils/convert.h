/*
* Created by boil on 2024/1/17.
*/

#ifndef RENDU_CONVERT_H
#define RENDU_CONVERT_H

#include "basic_define.h"
#include <algorithm>
#include <cctype>
#include <charconv>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <type_traits>

RD_NAMESPACE_BEGIN

class Convert {
public:
  template<typename Src, typename Dst>
  bool IsInBounds(Src &&src) {
    if constexpr (std::is_integral_v<Src> && std::is_integral_v<Dst>) {
      return src >= std::numeric_limits<Dst>::lowest() && src <= std::numeric_limits<Dst>::max();
    } else if constexpr (std::is_floating_point_v<Src> && std::is_integral_v<Dst>) {
      return src >= static_cast<double>(std::numeric_limits<Dst>::lowest()) && src <= static_cast<double>(std::numeric_limits<Dst>::max()) && std::floor(src) == src;
    }
    return true;// 默认情况，不需要检查
  }

  // 直接赋值
  template<class Src, typename Dst>
  static std::enable_if_t<std::is_same_v<std::decay_t<Src>, Dst>, std::optional<Dst>> Parse(Src &&src) {
    return src;
  }

  // 算术类型之间的转换，增强处理
  template<class Src, typename Dst>
  static std::enable_if_t<std::is_arithmetic_v<Src> && std::is_arithmetic_v<Dst>, std::optional<Dst>>
  Parse(Src &&src) {
    if constexpr (std::is_integral_v<Src> && std::is_integral_v<Dst>) {
      if (src < std::numeric_limits<Dst>::lowest() || src > std::numeric_limits<Dst>::max()) {
        return std::nullopt;
      }
    } else if constexpr (std::is_floating_point_v<Src> && std::is_integral_v<Dst>) {
      if (src < static_cast<double>(std::numeric_limits<Dst>::lowest()) || src > static_cast<double>(std::numeric_limits<Dst>::max()) || std::floor(src) != src) {
        return std::nullopt;
      }
    }
    return static_cast<Dst>(src);
  }

  // 对于整数类型
  template<typename Dst>
  static std::enable_if_t<std::is_integral_v<Dst> && !std::is_same_v<Dst, bool>, std::optional<Dst>>
  Parse(const std::string_view &src) {
    Dst result;
    auto begin = src.data();
    auto end = src.data() + src.size();
    auto [ptr, ec] = std::from_chars(begin, end, result);

    if (ec == std::errc() && ptr == end) {// 成功转换且消费了全部输入
      return result;
    } else {
      return std::nullopt;// 转换失败或未消费全部输入
    }
  }

  // 对于bool类型
  template<typename Dst>
  static std::enable_if_t<std::is_same_v<Dst, bool>, std::optional<Dst>>
  Parse(const std::string_view &src) {
    std::string lowerSrc;
    std::transform(src.begin(), src.end(), std::back_inserter(lowerSrc), [](unsigned char c) { return std::tolower(c); });
    if (lowerSrc == "true" || lowerSrc == "1") {
      return true;
    } else if (lowerSrc == "false" || lowerSrc == "0") {
      return false;
    }
    return std::nullopt;
  }

  // 对于浮点数类型
  template<typename Dst>
  static std::enable_if_t<std::is_floating_point_v<Dst>, std::optional<Dst>>
  Parse(const std::string_view &src) {
    Dst value;
    std::stringstream ss(src.data());
    ss >> value;
    if (!ss.fail() && ss.eof()) {// 成功转换且消费了全部输入
      return value;
    } else {
      return std::nullopt;// 转换失败或未消费全部输入
    }
  }


  //数组类型到字符串
  template<typename Src, std::size_t N>
  static std::string ToString(Src (&array)[N]) {
    std::string result = "[";
    for (std::size_t i = 0; i < N; ++i) {
      auto elemStr = ToString<Src>(std::move(array[i]));
      if (!elemStr) {
        return nullptr;
      }
      if (i > 0) {
        result += ", ";
      }
      result += *elemStr;
    }
    result += "]";
    return result;
  }

  // 算术类型到字符串，使用 std::to_string 优化
  template<typename Src>
  static std::enable_if_t<std::is_arithmetic_v<Src>, std::string>
  ToString(Src &&src) {
    if constexpr (std::is_same_v<std::decay_t<Src>, bool>) {
      return src ? "true" : "false";
    } else {
      return std::to_string(src);
    }
  }

  // 非算术类型(地址)到字符串
  template<typename Src>
  static std::enable_if_t<std::is_pointer_v<Src>, std::string>
  ToString(Src &&src) {
    std::ostringstream oss;
    oss << src;
    if (oss.fail()) {
      return nullptr;
    }
    return oss.str();
  }

  // 枚举类型到字符串
  // 注意：这个实现只是将枚举值转换为其底层整型的字符串表示。
  // 如果需要枚举值的具体名称，你需要实现一个额外的映射。
  template<typename Src>
  static std::enable_if_t<std::is_enum_v<Src>, std::string>
  ToString(Src &&src) {
    using UnderlyingType = std::underlying_type_t<Src>;
    return std::to_string(static_cast<UnderlyingType>(src));
  }
};
RD_NAMESPACE_END

#endif//RENDU_CONVERT_H
