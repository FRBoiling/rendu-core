/*
* Created by boil on 2024/7/7.
*/

#ifndef RENDU_BASIC_BASIC_BASE_VALUE_TYPE_H_
#define RENDU_BASIC_BASIC_BASE_VALUE_TYPE_H_

#include "detail/base_type_define.h"
#include "detail/status.h"
#include "detail/exception.h"

RD_NAMESPACE_BEGIN

using Char = rendu::detail::CHAR;
using CharPtr = rendu::detail::CHAR *;
using UInt = rendu::detail::UINT;
using Size = rendu::detail::SIZE;
using Void = rendu::detail::VOID;
using VoidPtr = rendu::detail::VOID *;
using Int = rendu::detail::INT;
using Long = rendu::detail::LONG;
using ULong = rendu::detail::ULONG;
using Bool = rendu::detail::BOOL;

using Index = rendu::detail::INT;                // 表示标识信息，可以为负数
using Float = rendu::detail::FLOAT;
using Double = rendu::detail::DOUBLE;


using ConstStr =rendu::detail::CONSTSTR;             // 表示 const char*
using BigBool = rendu::detail::BIGBOOL;

using Level = rendu::detail::INT;
using Sec = rendu::detail::LONG;                  // 表示秒信息, for second
using MSec = rendu::detail::LONG;                 // 表示毫秒信息, for millisecond
using FMSec = rendu::detail::DOUBLE;              // 表示毫秒信息，包含小数点信息

using String = rendu::detail::STRING;

using Status = rendu::detail::Status;
using Exception = rendu::detail::Exception;

RD_NAMESPACE_END

#endif//#define RENDU_BASIC_BASIC_BASE_VALUE_TYPE_H_

