/*
* Created by boil on 2023/10/16.
*/

#include "bit_converter.h"
#include <algorithm>

RD_NAMESPACE_BEGIN

    std::int32_t BitConverter::ToInt32(const std::byte *data, std::int32_t start_index) {
      if (sizeof (data) < sizeof(std::int32_t) + start_index) {
        // 处理长度不足的情况
        // 可以抛出异常或返回一个默认值，具体根据需求而定
        return 0;
      }

      std::int32_t result;
      std::memcpy(&result, data, sizeof(std::int32_t));
      return result;
    }

    std::uint16_t BitConverter::ToUInt16(const std::byte *data, std::int32_t start_index) {
      if (sizeof (data) < sizeof(std::int16_t) + start_index) {
        // 处理长度不足的情况
        // 可以抛出异常或返回一个默认值，具体根据需求而定
        return 0;
      }

      std::int16_t result;
      std::memcpy(&result, data, sizeof(std::int16_t));
      return result;
    }

RD_NAMESPACE_END
