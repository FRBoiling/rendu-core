/*
* Created by boil on 2023/11/14.
*/

#ifndef RENDU_BASE_NUMBER_HELPER_H
#define RENDU_BASE_NUMBER_HELPER_H

#include "number_define.h"
#include <cstdint>

RD_NAMESPACE_BEGIN

using int8_t = std::int8_t;
using uint8_t = std::uint8_t;
using int16_t = std::int16_t;
using uint16_t = std::uint16_t;
using int32_t = std::int32_t;
using uint32_t = std::uint32_t;
using int64_t = std::int64_t;
using uint64_t = std::uint64_t;


using ushort = unsigned short;
using uint = unsigned int;

ALIAS(int64_t , Long);
ALIAS(uint64_t , uLong);

RD_NAMESPACE_END

#endif//RENDU_BASE_NUMBER_HELPER_H
