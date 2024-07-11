/*
* Created by boil on 2024/7/7.
*/

#ifndef RENDU_BASIC_BASIC_BASE_FUNCTION_TYPE_H_
#define RENDU_BASIC_BASIC_BASE_FUNCTION_TYPE_H_


#include "type/value_type.h"
#include <functional>

RD_NAMESPACE_BEGIN

RD_DETAIL_NAMESPACE_BEGIN

using RD_DEFAULT_FUNCTION = std::function<void()>;
using RD_DEFAULT_CONST_FUNCTION_REF = const std::function<void()>&;

using RD_STATUS_FUNCTION = std::function<Status()>;
using RD_STATUS_CONST_FUNCTION_REF = const std::function<Status()>&;

using RD_CALLBACK_FUNCTION = std::function<void(Status)>;
using RD_CALLBACK_CONST_FUNCTION_REF = const std::function<void(Status)>&;

RD_DETAIL_NAMESPACE_END

RD_NAMESPACE_END

#endif//RENDU_BASIC_BASIC_BASE_FUNCTION_TYPE_H_
