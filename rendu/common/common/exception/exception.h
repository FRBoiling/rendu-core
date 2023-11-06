/*
* Created by boil on 2023/11/13.
*/

#ifndef RENDU_EXCEPTION_H
#define RENDU_EXCEPTION_H

#include "define.h"

COMMON_NAMESPACE_BEGIN

    class Exception : public std::exception {
    public:
      Exception(STRING what);

      ~Exception() noexcept override = default;

      // default copy-ctor and operator= are okay.

      const char *what() const noexcept override {
        return message_.c_str();
      }

      const char *stackTrace() const noexcept {
        return stack_.c_str();
      }

    private:
      STRING message_;
      STRING stack_;
    };

COMMON_NAMESPACE_END

#endif //RENDU_EXCEPTION_H
