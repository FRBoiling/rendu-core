/*
* Created by boil on 2023/11/14.
*/

#ifndef RENDU_BASE_NUMBER_HELPER_H
#define RENDU_BASE_NUMBER_HELPER_H

#include "number_define.h"
#include <cstdint>

RD_NAMESPACE_BEGIN

ALIAS(std::int8_t, INT8);
ALIAS(std::uint8_t, UINT8);

ALIAS(std::int16_t, INT16);
ALIAS(std::uint16_t, UINT16);

ALIAS(std::int32_t, INT32);
ALIAS(std::uint32_t, UINT32);

ALIAS(std::int64_t, INT64);
ALIAS(std::uint64_t, UINT64);


ALIAS(float, FLOAT);
ALIAS(double, DOUBLE);
ALIAS(int, INT);
ALIAS(double, LONG);

RD_NAMESPACE_END

#endif//RENDU_BASE_NUMBER_HELPER_H
