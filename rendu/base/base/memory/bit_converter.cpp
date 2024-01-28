/*
* Created by boil on 2023/10/16.
*/

#include "bit_converter.h"
#include <algorithm>

RD_NAMESPACE_BEGIN

static INT32 ToInt32(const BYTE *data, INT32 start_index) {
  if (sizeof(data) < sizeof(std::int32_t) + start_index) {
    // 处理长度不足的情况
    // 可以抛出异常或返回一个默认值，具体根据需求而定
    return 0;
  }

  std::int32_t result;
  std::memcpy(&result, data, sizeof(std::int32_t));
  return result;
}

static UINT16 ToUInt16(const BYTE *data, INT32 start_index) {
  if (sizeof(data) < sizeof(std::int16_t) + start_index) {
    // 处理长度不足的情况
    // 可以抛出异常或返回一个默认值，具体根据需求而定
    return 0;
  }

  std::int16_t result;
  std::memcpy(&result, data, sizeof(std::int16_t));
  return result;
}

RD_NAMESPACE_END
