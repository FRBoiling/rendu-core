/*
* Created by boil on 2023/10/16.
*/

#include "bit_converter.h"

RD_NAMESPACE_BEGIN

    int32 BitConverter::ToInt32(const byte *data, int32 start_index) {
      if (sizeof (data) < sizeof(int32) + start_index) {
        // 处理长度不足的情况
        // 可以抛出异常或返回一个默认值，具体根据需求而定
        return 0;
      }

      int32 result;
      std::memcpy(&result, data, sizeof(int32));
      return result;
    }

    uint16 BitConverter::ToUInt16(const byte *data, int32 start_index) {
      if (sizeof (data) < sizeof(int16) + start_index) {
        // 处理长度不足的情况
        // 可以抛出异常或返回一个默认值，具体根据需求而定
        return 0;
      }

      int16 result;
      std::memcpy(&result, data, sizeof(int16));
      return result;
    }

RD_NAMESPACE_END
