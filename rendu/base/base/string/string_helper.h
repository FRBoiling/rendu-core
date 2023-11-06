/*
* Created by boil on 2023/11/12.
*/

#ifndef RENDU_BASE_STRING_H
#define RENDU_BASE_STRING_H

#include "base/define.h"

#include <string>
#include <string_view>

RD_NAMESPACE_BEGIN

  using STRING= std::string;

  template<typename T>
  STRING To_String(T value) {
    return std::to_string(value);
  }


RD_NAMESPACE_END

#endif //RENDU_BASE_STRING_H
