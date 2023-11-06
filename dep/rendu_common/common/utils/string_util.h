/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_STRING_UTIL_H
#define RENDU_STRING_UTIL_H

#include <string>
#include <sstream>

#include "common/define.h"

RD_NAMESPACE_BEGIN

#define StrPrinter _StrPrinter()

  class _StrPrinter : public std::string {
  public:
    _StrPrinter() {}

    template<typename T>
    _StrPrinter &operator<<(T &&data) {
      _stream << std::forward<T>(data);
      this->std::string::operator=(_stream.str());
      return *this;
    }

    std::string operator<<(std::ostream &(*f)(std::ostream &)) const {
      return *this;
    }

  private:
    std::stringstream _stream;
  };

  class StringUtil {
  public:
    template<typename T>
    static std::string ToString(T value) {
      std::ostringstream os;
      os << std::forward<T>(value);
      return os.str();
    }

  };

RD_NAMESPACE_END

#endif //RENDU_STRING_UTIL_H
