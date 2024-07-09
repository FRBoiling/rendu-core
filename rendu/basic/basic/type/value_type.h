/*
* Created by boil on 2024/7/7.
*/

#ifndef RENDU_BASIC_BASIC_BASE_VALUE_TYPE_H_
#define RENDU_BASIC_BASIC_BASE_VALUE_TYPE_H_

#include "internal/base_type_define.h"
#include "internal/status.h"
#include "internal/exception.h"

RD_NAMESPACE_BEGIN

using Char = rendu::internal::CHAR;
using CharPtr = rendu::internal::CHAR *;
using UInt = rendu::internal::UINT;
using Size = rendu::internal::SIZE;
using Void = rendu::internal::VOID;
using VoidPtr = rendu::internal::VOID *;
using Int = rendu::internal::INT;
using Long = rendu::internal::LONG;
using ULong = rendu::internal::ULONG;
using Bool = rendu::internal::BOOL;

using Index = rendu::internal::INT;                // 表示标识信息，可以为负数
using Float = rendu::internal::FLOAT;
using Double = rendu::internal::DOUBLE;


using ConstStr =rendu::internal::CONSTSTR;             // 表示 const char*
using BigBool = rendu::internal::BIGBOOL;

using Level = rendu::internal::INT;
using Sec = rendu::internal::LONG;                  // 表示秒信息, for second
using MSec = rendu::internal::LONG;                 // 表示毫秒信息, for millisecond
using FMSec = rendu::internal::DOUBLE;              // 表示毫秒信息，包含小数点信息

using Status = rendu::internal::Status;
using Exception = rendu::internal::Exception;
using String = rendu::internal::String;

RD_NAMESPACE_END

#endif//#define RENDU_BASIC_BASIC_BASE_VALUE_TYPE_H_

