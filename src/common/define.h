//
// Created by Boil on 2022/7/6.
//

#ifndef RENDU_DEFINE_H_
#define RENDU_DEFINE_H_

#include <cstddef>
#include <cinttypes>
#include <climits>

typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;
typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;

enum DBCFormer
{
  FT_STRING = 's',                                        // LocalizedString*
  FT_STRING_NOT_LOCALIZED = 'S',                          // char*
  FT_FLOAT = 'f',                                         // float
  FT_INT = 'i',                                           // uint32
  FT_BYTE = 'b',                                          // uint8
  FT_SHORT = 'h',                                         // uint16
  FT_LONG = 'l'                                           // uint64
};

#endif//RENDU_SRC_COMMON_DEFINE_H_
