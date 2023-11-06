/*
* Created by boil on 2023/10/16.
*/

#include "bit_converter.h"
#include <algorithm>

RD_NAMESPACE_BEGIN

    INT32 BitConverter::ToInt32(const BYTE *data, INT32 start_index) {
      if (sizeof (data) < sizeof(INT32) + start_index) {
        // 处理长度不足的情况
        // 可以抛出异常或返回一个默认值，具体根据需求而定
        return 0;
      }

      INT32 result;
      std::memcpy(&result, data, sizeof(INT32));
      return result;
    }

    UINT16 BitConverter::ToUInt16(const BYTE *data, INT32 start_index) {
      if (sizeof (data) < sizeof(INT16) + start_index) {
        // 处理长度不足的情况
        // 可以抛出异常或返回一个默认值，具体根据需求而定
        return 0;
      }

      INT16 result;
      std::memcpy(&result, data, sizeof(INT16));
      return result;
    }

RD_NAMESPACE_END
