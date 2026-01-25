#include "common/ser/protobuf_ser.h"
#include <google/protobuf/util/message_differencer.h>

BEGIN_NAMESPACE_COMMON
namespace ser {

using net::ByteBuffer;

Result<ByteBuffer> ProtobufSerializer::serialize(const ByteBuffer& data) {
    // 输入已经是 protobuf 二进制格式，直接返回
    // 这个方法主要用于接口一致性
    return data;
}

Result<ByteBuffer> ProtobufSerializer::deserialize(const ByteBuffer& data) {
    // 输入是 protobuf 二进制格式，直接返回
    // 这个方法主要用于接口一致性
    return data;
}

} // namespace ser
END_NAMESPACE_COMMON
