/*
* Created by boil on 2023/9/23.
*/

#ifndef RENDU_COMMON_ARGUMENT_EXCEPTION_H
#define RENDU_COMMON_ARGUMENT_EXCEPTION_H

#include "base/string/string_helper.h"
#include "common/common_define.h"

COMMON_NAMESPACE_BEGIN

    class ArgumentException : public std::exception {
    public:
      template<typename... Args>
      ArgumentException(format_string<Args...> format, Args... args)
          : m_msg(fmt::format(format, args...).c_str()) {
      }

      ~ArgumentException() {
      }

      const char *what() const _NOEXCEPT override {
        return exception::what();
      }

    private:
      string m_msg;
    };

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_ARGUMENT_EXCEPTION_H
