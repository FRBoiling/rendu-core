#pragma once

#include "common/define.h"
#include "common/util/error.h"
#include "common/net/codec.h"
#include <variant>
#include <memory>
#include <string_view>

BEGIN_NAMESPACE_COMMON
namespace ser {

// 引入 net 命名空间的 ByteBuffer
using net::ByteBuffer;

/**
 * @brief 序列化器抽象基类
 * 
 * 定义序列化/反序列化的统一接口，支持多种数据格式
 */
class Serializer {
public:
    virtual ~Serializer() = default;

    /**
     * @brief 序列化数据
     * @param data 要序列化的字节数据
     * @return 序列化后的字节数据或错误
     */
    virtual Result<ByteBuffer> serialize(const ByteBuffer& data) = 0;

    /**
     * @brief 反序列化数据
     * @param data 要反序列化的字节数据
     * @return 反序列化后的字节数据或错误
     */
    virtual Result<ByteBuffer> deserialize(const ByteBuffer& data) = 0;

    /**
     * @brief 重置序列化器状态
     */
    virtual void reset() {}
};

/**
 * @brief 便捷的序列化函数
 * @tparam T 可序列化的数据类型
 * @param serializer 序列化器
 * @param data 要序列化的数据
 * @return 序列化后的字节数据或错误
 */
template<typename T>
Result<ByteBuffer> serialize(Serializer& serializer, const T& data) {
    // 将 T 转换为 ByteBuffer（需要 T 支持此转换）
    ByteBuffer buffer(data.begin(), data.end());
    return serializer.serialize(buffer);
}

/**
 * @brief 便捷的反序列化函数
 * @tparam T 目标数据类型
 * @param serializer 序列化器
 * @param data 要反序列化的字节数据
 * @return 反序列化后的数据或错误
 */
template<typename T>
Result<T> deserialize(Serializer& serializer, const ByteBuffer& data) {
    auto buffer_result = serializer.deserialize(data);
    if (std::holds_alternative<Error>(buffer_result)) {
        return std::get<Error>(buffer_result);
    }

    auto buffer = std::get<ByteBuffer>(buffer_result);
    // 将 ByteBuffer 转换为 T（需要 T 支持此转换）
    T result;
    std::copy(buffer.begin(), buffer.end(), std::back_inserter(result));
    return result;
}

} // namespace ser
END_NAMESPACE_COMMON
