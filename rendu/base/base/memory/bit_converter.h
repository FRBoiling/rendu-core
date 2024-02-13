/*
* Created by boil on 2023/10/16.
*/

#ifndef RENDU_BASE_BIT_CONVERTER_H
#define RENDU_BASE_BIT_CONVERTER_H

#include "memory_define.h"
#include "number/number_helper.h"
#include <cstddef>

RD_NAMESPACE_BEGIN

using byte = unsigned char;
#define BYTE_MAX UCHAR_MAX


class BitConverter {
public:
  static int ToInt(const byte *data, int start_index);

  static ushort ToUShort(const byte *data, ushort start_index);
};

RD_NAMESPACE_END

#endif//RENDU_BASE_BIT_CONVERTER_H
