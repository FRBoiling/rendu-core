/*
  * Created by boil on 2023/10/15.
  */

#ifndef RENDU_BASE_STRING_HELPER_H
#define RENDU_BASE_STRING_HELPER_H

#include "number/number_helper.h"
#include "string_define.h"
#include "string_format.h"

RD_NAMESPACE_BEGIN

ALIAS(std::string, string);

namespace Digit {

  bool IsDigit(char c);

  std::uint8_t ToDigitValue(char c);

  std::optional<std::uint8_t> TryParseHexDigit(char c);

  char HexToChar(std::uint8_t value);

  std::optional<std::uint16_t> StringToPort(std::string_view string);

}// namespace Digit

RD_NAMESPACE_END

#include "str_error.h"
#include "string_arg.h"
#include "string_piece.h"

#endif//RENDU_BASE_STRING_HELPER_H
