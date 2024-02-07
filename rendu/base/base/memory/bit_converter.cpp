/*
* Created by boil on 2023/10/16.
*/

#include "bit_converter.h"
#include <algorithm>

RD_NAMESPACE_BEGIN

static int ToInt32(const byte *data, int start_index) {
  if (sizeof(data) < sizeof(int) + start_index) {
    // 处理长度不足的情况
    // 可以抛出异常或返回一个默认值，具体根据需求而定
    return 0;
  }

  int result;
  std::memcpy(&result, data, sizeof(int));
  return result;
}

static ushort ToUShort(const byte *data, ushort start_index){
  if (sizeof(data) < sizeof(ushort) + start_index) {
    // 处理长度不足的情况
    // 可以抛出异常或返回一个默认值，具体根据需求而定
    return 0;
  }

  short result;
  std::memcpy(&result, data, sizeof(ushort));
  return result;
}


RD_NAMESPACE_END
