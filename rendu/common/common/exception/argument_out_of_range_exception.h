/*
* Created by boil on 2023/10/17.
*/

#ifndef RENDU_COMMON_ARGUMENT_OUT_OF_RANGE_EXCEPTION_H
#define RENDU_COMMON_ARGUMENT_OUT_OF_RANGE_EXCEPTION_H

#include "base/string/string_helper.h"
#include "common/common_define.h"

COMMON_NAMESPACE_BEGIN

    class ArgumentOutOfRangeException : public std::exception {
    public:
      template<typename... Args>
      ArgumentOutOfRangeException(format_string<Args...> format, Args... args)
          : m_msg(fmt::format(format, args...).c_str()) {
      }

      ~ArgumentOutOfRangeException() {
      }

      const char *what() const _NOEXCEPT override {
        return exception::what();
      }

    private:
      string m_msg;
    };

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_ARGUMENT_OUT_OF_RANGE_EXCEPTION_H
