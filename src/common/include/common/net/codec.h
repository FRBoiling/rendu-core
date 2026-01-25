#pragma once

#include "common/define.h"
#include <vector>
#include <memory>
#include <cstdint>

BEGIN_NAMESPACE_COMMON
namespace net {

/**
 * @brief 字节缓冲区类型别名
 */
using ByteBuffer = std::vector<byte>;

/**
 * @brief 编解码器接口
 * 
 * 定义消息编解码的抽象接口，支持自定义协议
 */
class Codec {
public:
    virtual ~Codec() = default;

    /**
     * @brief 编码：将应用层数据编码为网络字节流
     * @param data 要编码的数据
     * @return 编码后的字节流
     */
    virtual ByteBuffer encode(const ByteBuffer& data) = 0;

    /**
     * @brief 解码：从网络字节流提取完整消息
     * @param buffer 接收缓冲区（会被修改，从前面移除已解码的字节）
     * @return 完整消息列表
     */
    virtual std::vector<ByteBuffer> decode(ByteBuffer& buffer) = 0;

    /**
     * @brief 重置解码器状态
     */
    virtual void reset() {}
};

/**
 * @brief 长度前缀编解码器
 * 
 * 格式：[4字节长度(大端序)][数据体]
 * 
 * 示例：
 *   数据: {0x01, 0x02, 0x03}
 *   编码: {0x00, 0x00, 0x00, 0x03, 0x01, 0x02, 0x03}
 */
class LengthPrefixCodec : public Codec {
public:
    /**
     * @brief 构造函数
     * @param max_length 单个消息最大长度（默认 1MB）
     */
    explicit LengthPrefixCodec(uint32_t max_length = 1024 * 1024);

    ~LengthPrefixCodec() override = default;

    ByteBuffer encode(const ByteBuffer& data) override;
    std::vector<ByteBuffer> decode(ByteBuffer& buffer) override;
    void reset() override;

private:
    uint32_t max_length_;
    enum class DecodeState {
        WaitingLength,
        WaitingData
    } state_;
    uint32_t expected_length_;
    uint32_t bytes_received_;
};

/**
 * @brief 分隔符编解码器
 * 
 * 格式：[数据][分隔符]
 * 
 * 适合文本协议（如 HTTP、Redis）
 */
class DelimiterCodec : public Codec {
public:
    /**
     * @brief 构造函数
     * @param delimiter 分隔符字节（默认 '\n'）
     */
    explicit DelimiterCodec(byte delimiter = '\n');

    ~DelimiterCodec() override = default;

    ByteBuffer encode(const ByteBuffer& data) override;
    std::vector<ByteBuffer> decode(ByteBuffer& buffer) override;

private:
    byte delimiter_;
};

/**
 * @brief 行编解码器（分隔符为 '\n' 的 DelimiterCodec 特例）
 */
class LineCodec : public DelimiterCodec {
public:
    LineCodec() : DelimiterCodec('\n') {}
};

} // namespace net
END_NAMESPACE_COMMON
