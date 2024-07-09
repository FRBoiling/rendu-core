/*
* Created by boil on 2024/7/7.
*/

#ifndef RENDU_BASIC_BASIC_BASE_BASE_TYPE_DEFINE_H_
#define RENDU_BASIC_BASIC_BASE_BASE_TYPE_DEFINE_H_

#include "internal_define.h"
#include <cstddef>

RD_NAMESPACE_BEGIN
RD_INTERNAL_NAMESPACE_BEGIN

using CHAR = char;
using UINT = unsigned int;
using VOID = void;
using INT = int;
using LONG = long;
using ULONG = unsigned long;
using BOOL = bool;
using BIGBOOL = int;
using FLOAT = float;
using DOUBLE = double;
using SIZE = std::size_t;

using CONSTSTR = const char *;// 表示 const char*

RD_INTERNAL_NAMESPACE_END
RD_NAMESPACE_END

#endif//RENDU_BASIC_BASIC_BASE_BASE_TYPE_DEFINE_H_
