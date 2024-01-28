/*
* Created by boil on 2023/10/16.
*/

#ifndef RENDU_BASE_BIT_CONVERTER_H
#define RENDU_BASE_BIT_CONVERTER_H

#include "memory_define.h"
#include "number/number_helper.h"
#include <cstddef>

RD_NAMESPACE_BEGIN

ALIAS(std::byte, BYTE);

class BitConverter {
public:
  static INT32 ToInt32(const BYTE *data, INT32 start_index);

  static UINT16 ToUInt16(const BYTE *data, INT32 start_index);
};

RD_NAMESPACE_END

#endif//RENDU_BASE_BIT_CONVERTER_H
