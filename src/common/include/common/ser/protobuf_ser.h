#pragma once

#include "common/define.h"
#include "common/ser/serializer.h"
#include "common/net/codec.h"
#include <google/protobuf/message.h>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

BEGIN_NAMESPACE_COMMON
namespace ser {

// 引入 net 命名空间的 ByteBuffer
using net::ByteBuffer;

/**
 * @brief Protobuf 序列化器
 * 
 * 封装 Google Protocol Buffers 的序列化/反序列化操作
 * 支持任意 protobuf 消息类型
 * 
 * 性能优化:
 * - 预分配缓冲区大小,减少重分配
 * - 支持 string_view 避免不必要的拷贝
 * - 支持外部缓冲区直接序列化
 */
class ProtobufSerializer : public Serializer {
public:
    /**
     * @brief 构造函数
     */
    ProtobufSerializer() = default;

    /**
     * @brief 析构函数
     */
    ~ProtobufSerializer() override = default;

    // 禁止拷贝和移动
    ProtobufSerializer(const ProtobufSerializer&) = delete;
    ProtobufSerializer& operator=(const ProtobufSerializer&) = delete;
    ProtobufSerializer(ProtobufSerializer&&) = delete;
    ProtobufSerializer& operator=(ProtobufSerializer&&) = delete;

    /**
     * @brief 序列化 protobuf 消息
     * @param data 包含序列化后 protobuf 数据的缓冲区
     * @return 同样的数据（因为输入已经是 protobuf 格式）或错误
     * 
     * 注意：这个方法主要用于接口一致性。
     * 实际使用时，推荐使用下面的便利方法。
     */
    Result<ByteBuffer> serialize(const ByteBuffer& data) override;

    /**
     * @brief 反序列化 protobuf 数据
     * @param data protobuf 二进制数据
     * @return 原始数据或错误
     */
    Result<ByteBuffer> deserialize(const ByteBuffer& data) override;

    /**
     * @brief 序列化 protobuf 消息到 ByteBuffer (优化版本)
     * @tparam T protobuf 消息类型
     * @param message protobuf 消息
     * @return 序列化后的字节数据或错误
     * 
     * 性能优化: 预分配缓冲区大小,避免多次重分配
     */
    template<typename T>
    static Result<ByteBuffer> serialize_message(const T& message) {
        static_assert(std::is_base_of<google::protobuf::Message, T>::value,
                      "T must be a protobuf Message");

        size_t size = message.ByteSizeLong();
        ByteBuffer buffer;
        buffer.reserve(size);  // 预分配,避免重分配
        buffer.resize(size);
        
        if (!message.SerializeToArray(buffer.data(), static_cast<int>(size))) {
            return Error(ErrorCode::SerializationError, "Failed to serialize protobuf message");
        }

        return buffer;
    }

    /**
     * @brief 从 ByteBuffer 反序列化 protobuf 消息 (优化版本)
     * @tparam T protobuf 消息类型
     * @param data protobuf 二进制数据
     * @return 反序列化后的消息或错误
     * 
     * 性能优化: 直接从内存解析,无需额外拷贝
     */
    template<typename T>
    static Result<T> deserialize_message(const ByteBuffer& data) {
        static_assert(std::is_base_of<google::protobuf::Message, T>::value,
                      "T must be a protobuf Message");

        T message;
        if (!message.ParseFromArray(data.data(), static_cast<int>(data.size()))) {
            return Error(ErrorCode::SerializationError, "Failed to deserialize protobuf message");
        }

        return message;
    }

    /**
     * @brief 反序列化 protobuf 消息（指针版本）
     * @tparam T protobuf 消息类型
     * @param data protobuf 二进制数据
     * @param output 输出消息指针
     * @return 成功或错误
     */
    template<typename T>
    static Error deserialize_message(const ByteBuffer& data, T* output) {
        static_assert(std::is_base_of<google::protobuf::Message, T>::value,
                      "T must be a protobuf Message");

        if (!output) {
            return Error(ErrorCode::InvalidArgument, "Output message is null");
        }

        if (!output->ParseFromArray(data.data(), static_cast<int>(data.size()))) {
            return Error(ErrorCode::SerializationError, "Failed to deserialize protobuf message");
        }

        return Error(ErrorCode::Success);
    }

    /**
     * @brief 序列化到 string (避免中间拷贝)
     * @tparam T protobuf 消息类型
     * @param message protobuf 消息
     * @return 序列化后的字符串或错误
     * 
     * 性能优化: 预分配字符串大小,避免多次重分配
     */
    template<typename T>
    static Result<std::string> serialize_to_string(const T& message) {
        static_assert(std::is_base_of<google::protobuf::Message, T>::value,
                      "T must be a protobuf Message");

        std::string output;
        output.reserve(message.ByteSizeLong());  // 预分配

        if (!message.SerializeToString(&output)) {
            return Error(ErrorCode::SerializationError, "Failed to serialize protobuf message to string");
        }

        return output;
    }

    /**
     * @brief 从 string_view 反序列化 (避免拷贝)
     * @tparam T protobuf 消息类型
     * @param data string_view 包装的二进制数据
     * @return 反序列化后的消息或错误
     * 
     * 性能优化: 使用 string_view 避免数据拷贝
     */
    template<typename T>
    static Result<T> deserialize_from_string_view(std::string_view data) {
        static_assert(std::is_base_of<google::protobuf::Message, T>::value,
                      "T must be a protobuf Message");

        T message;
        if (!message.ParseFromArray(data.data(), static_cast<int>(data.size()))) {
            return Error(ErrorCode::SerializationError, "Failed to deserialize protobuf message from string_view");
        }

        return message;
    }

    /**
     * @brief 序列化到外部缓冲区 (零拷贝接口)
     * @tparam T protobuf 消息类型
     * @param message protobuf 消息
     * @param buffer 外部缓冲区
     * @return 成功或错误
     *
     * 性能优化: 直接写入外部提供的缓冲区,无需额外内存分配
     */
    template<typename T>
    static Error serialize_to_buffer(const T& message, std::vector<char>& buffer) {
        static_assert(std::is_base_of<google::protobuf::Message, T>::value,
                      "T must be a protobuf Message");

        auto size = message.ByteSizeLong();
        buffer.resize(size);

        if (!message.SerializeToArray(buffer.data(), static_cast<int>(size))) {
            return Error(ErrorCode::SerializationError, "Failed to serialize protobuf message to buffer");
        }

        return Error(ErrorCode::Success);
    }

    /**
     * @brief 序列化到字节数组 (零拷贝接口)
     * @tparam T protobuf 消息类型
     * @param message protobuf 消息
     * @param buffer 字节数组指针
     * @param buffer_size 缓冲区大小
     * @return 实际序列化大小,或错误
     * 
     * 性能优化: 直接写入外部数组,无需额外内存分配
     */
    template<typename T>
    static Result<size_t> serialize_to_array(const T& message, void* buffer, size_t buffer_size) {
        static_assert(std::is_base_of<google::protobuf::Message, T>::value,
                      "T must be a protobuf Message");

        if (!buffer || buffer_size == 0) {
            return Error(ErrorCode::InvalidArgument, "Invalid buffer");
        }

        auto required_size = message.ByteSizeLong();
        if (required_size > buffer_size) {
            return Error(ErrorCode::InvalidArgument, "Buffer too small");
        }

        if (!message.SerializeToArray(buffer, static_cast<int>(required_size))) {
            return Error(ErrorCode::SerializationError, "Failed to serialize protobuf message to array");
        }

        return required_size;
    }

    /**
     * @brief 获取消息的字节大小
     * @tparam T protobuf 消息类型
     * @param message protobuf 消息
     * @return 消息的字节大小
     */
    template<typename T>
    static size_t message_size(const T& message) {
        static_assert(std::is_base_of<google::protobuf::Message, T>::value,
                      "T must be a protobuf Message");
        return message.ByteSizeLong();
    }

    /**
     * @brief 检查数据是否是有效的 protobuf 消息
     * @tparam T protobuf 消息类型
     * @param data 二进制数据
     * @return 是否有效
     */
    template<typename T>
    static bool is_valid_message(const ByteBuffer& data) {
        static_assert(std::is_base_of<google::protobuf::Message, T>::value,
                      "T must be a protobuf Message");

        T message;
        return message.ParseFromArray(data.data(), static_cast<int>(data.size()));
    }

    /**
     * @brief 检查数据是否是有效的 protobuf 消息 (string_view 版本)
     * @tparam T protobuf 消息类型
     * @param data string_view 包装的二进制数据
     * @return 是否有效
     * 
     * 性能优化: 使用 string_view 避免数据拷贝
     */
    template<typename T>
    static bool is_valid_message_view(std::string_view data) {
        static_assert(std::is_base_of<google::protobuf::Message, T>::value,
                      "T must be a protobuf Message");

        T message;
        return message.ParseFromArray(data.data(), static_cast<int>(data.size()));
    }
};

} // namespace ser
END_NAMESPACE_COMMON
