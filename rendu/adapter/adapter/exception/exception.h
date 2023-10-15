/*
* Created by boil on 2023/10/15.
*/

#ifndef RENDU_EXCEPTION_H
#define RENDU_EXCEPTION_H


#include "define.h"
#include "utils/string_helper.h"

RD_NAMESPACE_BEGIN

    class Exception : public std::exception {
    public:
      template<typename... Args>
      explicit Exception(format_string<Args...> format, Args... args)
          : m_msg(fmt::format(format, args...).c_str()) {
      }
      ~Exception() {
      }

      const char *what() const _NOEXCEPT override {
        return exception::what();
      }

    private:
      string m_msg;
    };

RD_NAMESPACE_END

#endif //RENDU_EXCEPTION_H
