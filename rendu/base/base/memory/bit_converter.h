/*
* Created by boil on 2023/10/16.
*/

#ifndef RENDU_BASE_BIT_CONVERTER_H
#define RENDU_BASE_BIT_CONVERTER_H

#include "memory_define.h"
#include "number/def_helper.h"
#include "number/int_helper.h"

RD_NAMESPACE_BEGIN

  class BitConverter {
  public:
    static std::int32_t ToInt32(const std::byte *data, std::int32_t start_index);

    static std::uint16_t ToUInt16(const std::byte *data, std::int32_t start_index);
  };

RD_NAMESPACE_END

#endif //RENDU_BASE_BIT_CONVERTER_H
