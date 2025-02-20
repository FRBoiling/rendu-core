/*
* Created by boil on 2024/7/7.
*/

#ifndef RENDU_BASIC_BASIC_BASE_STRING_TYPE_DEFINE_H_
#define RENDU_BASIC_BASIC_BASE_STRING_TYPE_DEFINE_H_

#include "basic_define.h"
#include <string>

RD_NAMESPACE_BEGIN
RD_DETAIL_NAMESPACE_BEGIN

using STRING = std::string;

RD_DETAIL_NAMESPACE_END

static const detail::STRING& RD_EMPTY = "";
static const detail::STRING& RD_DEFAULT = "default";
static const detail::STRING& RD_UNKNOWN = "unknown";
static const detail::STRING& RD_FUNCTION_NO_SUPPORT = "function no support";
static const detail::STRING& RD_INPUT_IS_NULL = "input is nullptr";

RD_NAMESPACE_END

#endif//RENDU_BASIC_BASIC_BASE_STRING_TYPE_DEFINE_H_
