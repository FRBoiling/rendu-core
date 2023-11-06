/*
* Created by boil on 2023/10/15.
*/

#ifndef RENDU_COMMON_STRING_HELPER_H
#define RENDU_COMMON_STRING_HELPER_H

#include "define.h"
#include <fmt/core.h>

COMMON_NAMESPACE_BEGIN

template<typename... Args>
using format_string = fmt::format_string<Args...>;

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_STRING_HELPER_H
