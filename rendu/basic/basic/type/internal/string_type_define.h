/*
* Created by boil on 2024/7/7.
*/

#ifndef RENDU_BASIC_BASIC_BASE_STRING_TYPE_DEFINE_H_
#define RENDU_BASIC_BASIC_BASE_STRING_TYPE_DEFINE_H_

#include "internal_define.h"
#include <string>

RD_NAMESPACE_BEGIN
RD_INTERNAL_NAMESPACE_BEGIN

using String = std::string;

RD_INTERNAL_NAMESPACE_END

static const internal::String& RD_EMPTY = "";
static const internal::String& RD_DEFAULT = "default";
static const internal::String& RD_UNKNOWN = "unknown";
static const internal::String& RD_FUNCTION_NO_SUPPORT = "function no support";
static const internal::String& RD_INPUT_IS_NULL = "input is nullptr";

RD_NAMESPACE_END

#endif//RENDU_BASIC_BASIC_BASE_STRING_TYPE_DEFINE_H_
