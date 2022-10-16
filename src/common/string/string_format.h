/*
* Created by boil on 2022/9/25.
*/

#ifndef RENDU_STRING_FORMAT_H_
#define RENDU_STRING_FORMAT_H_

#include "spdlog/fmt/fmt.h"

namespace rendu
{
  /// Default rd string format function.
  template <typename... T>
  inline std::string StringFormat(fmt::format_string<T...> fmt_str, T&&... args)
  {
    try
    {
      return vformat(fmt_str, fmt::make_format_args(args...));
    }
    catch (const fmt::format_error& formatError)
    {
      std::string error = "An error occurred /"+ std::string(formatError.what()) ;
      return error;
    }
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
}

#endif //RENDU_STRING_FORMAT_H_
