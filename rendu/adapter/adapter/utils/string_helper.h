/*
* Created by boil on 2023/10/15.
*/

#ifndef RENDU_STRING_HELPER_H
#define RENDU_STRING_HELPER_H

#include "define.h"
#include <fmt/core.h>

RD_NAMESPACE_BEGIN

template<typename... Args>
using format_string = fmt::format_string<Args...>;

RD_NAMESPACE_END

#endif //RENDU_STRING_HELPER_H
