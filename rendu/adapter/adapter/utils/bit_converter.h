/*
* Created by boil on 2023/10/16.
*/

#ifndef RENDU_BIT_CONVERTER_H
#define RENDU_BIT_CONVERTER_H

#include "define.h"

RD_NAMESPACE_BEGIN

class BitConverter {
public:
  static int32 ToInt32(const byte * data, int32 start_index);
  static uint16 ToUInt16(const byte * data, int32 start_index);
};

RD_NAMESPACE_END

#endif //RENDU_BIT_CONVERTER_H
