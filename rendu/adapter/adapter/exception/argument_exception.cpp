/*
* Created by boil on 2023/9/23.
*/

#include "argument_exception.h"

RD_NAMESPACE_BEGIN

    ArgumentException::ArgumentException(const std::string &msg) {

    }

    ArgumentException::~ArgumentException() {

    }

    const char *ArgumentException::what() const noexcept {
      return "ArgumentException :" ;
    }

RD_NAMESPACE_END
