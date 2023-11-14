/*
* Created by boil on 2023/10/15.
*/

#ifndef RENDU_BASE_STRING_HELPER_H
#define RENDU_BASE_STRING_HELPER_H


#include "string_define.h"
#include "number/int_helper.h"

RD_NAMESPACE_BEGIN

bool IsDigit(char c);

UINT8 ToDigitValue(char c);

std::optional<UINT8> TryParseHexDigit(char c);

char HexChar(UINT8 value);

std::optional<std::uint16_t> ParseInt(std::string_view string);

RD_NAMESPACE_END

#include "str_error.h"
#include "string_arg.h"
#include "string_piece.h"

#endif //RENDU_BASE_STRING_HELPER_H
