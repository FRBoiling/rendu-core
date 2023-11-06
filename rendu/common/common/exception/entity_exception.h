/*
* Created by boil on 2023/9/29.
*/

#ifndef RENDU_COMMON_ENTITY_EXCEPTION_H
#define RENDU_COMMON_ENTITY_EXCEPTION_H

#include "common/define.h"
#include "utils/string_helper.h"

COMMON_NAMESPACE_BEGIN

    class EntityException : public std::exception {
    public:
      template<typename... Args>
      EntityException(format_string<Args...> format, Args... args)
          : m_msg(fmt::format(format, args...).c_str()) {
      }

      ~EntityException() {
      }

      const char *what() const _NOEXCEPT override {
        return exception::what();
      }

    private:
      string m_msg;
    };

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_ENTITY_EXCEPTION_H
