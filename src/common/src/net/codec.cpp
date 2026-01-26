#include "common/net/codec.h"
#include "common/log/logger.h"
#include <algorithm>
#include <cstring>
#include <sstream>

BEGIN_NAMESPACE_COMMON
namespace net {

// ============================================================================
// LengthPrefixCodec 实现
// ============================================================================

LengthPrefixCodec::LengthPrefixCodec(uint32_t max_length)
    : max_length_(max_length)
    , state_(DecodeState::WaitingLength)
    , expected_length_(0)
    , bytes_received_(0)
{
}

ByteBuffer LengthPrefixCodec::encode(const ByteBuffer& data) {
    // 检查消息长度
    if (data.size() > max_length_) {
        RENDU_LOG_ERROR("Message too large: {} (max: {})", data.size(), max_length_);
        throw std::runtime_error("Message too large");
    }
    
    // 编码格式：[4字节长度(大端序)][数据体]
    ByteBuffer encoded;
    encoded.reserve(sizeof(uint32_t) + data.size());
    
    // 写入长度（大端序）
    uint32_t length = static_cast<uint32_t>(data.size());
    encoded.push_back((length >> 24) & 0xFF);
    encoded.push_back((length >> 16) & 0xFF);
    encoded.push_back((length >> 8) & 0xFF);
    encoded.push_back(length & 0xFF);
    
    // 写入数据体
    encoded.insert(encoded.end(), data.begin(), data.end());
    
    return encoded;
}

std::vector<ByteBuffer> LengthPrefixCodec::decode(ByteBuffer& buffer) {
    std::vector<ByteBuffer> messages;
    
    while (!buffer.empty()) {
        if (state_ == DecodeState::WaitingLength) {
            // 等待长度前缀
            if (buffer.size() < sizeof(uint32_t)) {
                // 数据不足，等待更多数据
                break;
            }
            
            // 读取长度（大端序）
            expected_length_ = (static_cast<uint32_t>(buffer[0]) << 24) |
                             (static_cast<uint32_t>(buffer[1]) << 16) |
                             (static_cast<uint32_t>(buffer[2]) << 8) |
                             static_cast<uint32_t>(buffer[3]);

            // 检查长度合法性
            // 注意：长度为 0 是合法的空消息
            if (expected_length_ > max_length_) {
                RENDU_LOG_ERROR("Message length exceeds limit: {} (max: {})", expected_length_, max_length_);
                reset();
                break;
            }

            // 移除长度前缀
            buffer.erase(buffer.begin(), buffer.begin() + sizeof(uint32_t));

            // 特殊处理空消息（expected_length_ == 0）
            if (expected_length_ == 0) {
                // 空消息直接返回一个空 ByteBuffer
                messages.emplace_back();

                // 重置状态，等待下一个消息
                expected_length_ = 0;
                bytes_received_ = 0;
                // 状态保持为 WaitingLength，继续处理下一个消息
                continue;
            }

            bytes_received_ = 0;
            state_ = DecodeState::WaitingData;
            
        } else if (state_ == DecodeState::WaitingData) {
            // 特殊处理空消息（expected_length_ == 0）
            if (expected_length_ == 0) {
                // 空消息直接返回一个空 ByteBuffer
                messages.emplace_back();

                // 重置状态，等待下一个消息
                state_ = DecodeState::WaitingLength;
                expected_length_ = 0;
                bytes_received_ = 0;

                // 继续处理下一个消息
                continue;
            }

            // 等待数据体
            size_t available = std::min(
                static_cast<size_t>(expected_length_ - bytes_received_),
                buffer.size()
            );

            bytes_received_ += available;

            // 检查是否收到完整消息
            if (bytes_received_ == expected_length_) {
                // 提取完整消息
                ByteBuffer message(buffer.begin(), buffer.begin() + expected_length_);
                messages.push_back(std::move(message));

                // 移除已解码的数据
                buffer.erase(buffer.begin(), buffer.begin() + expected_length_);

                // 重置状态，等待下一个消息
                state_ = DecodeState::WaitingLength;
                expected_length_ = 0;
                bytes_received_ = 0;
            } else {
                // 数据不完整，等待更多数据
                break;
            }
        }
    }
    
    return messages;
}

void LengthPrefixCodec::reset() {
    state_ = DecodeState::WaitingLength;
    expected_length_ = 0;
    bytes_received_ = 0;
}

// ============================================================================
// DelimiterCodec 实现
// ============================================================================

DelimiterCodec::DelimiterCodec(byte delimiter)
    : delimiter_(delimiter)
{
}

ByteBuffer DelimiterCodec::encode(const ByteBuffer& data) {
    // 编码格式：[数据][分隔符]
    ByteBuffer encoded;
    encoded.reserve(data.size() + 1);
    encoded.insert(encoded.end(), data.begin(), data.end());
    encoded.push_back(delimiter_);
    return encoded;
}

std::vector<ByteBuffer> DelimiterCodec::decode(ByteBuffer& buffer) {
    std::vector<ByteBuffer> messages;
    
    while (true) {
        // 查找分隔符
        auto it = std::find(buffer.begin(), buffer.end(), delimiter_);
        
        if (it == buffer.end()) {
            // 没有找到分隔符，等待更多数据
            break;
        }
        
        // 提取分隔符前的数据作为一条消息
        size_t message_size = std::distance(buffer.begin(), it);
        ByteBuffer message(buffer.begin(), it);
        messages.push_back(std::move(message));
        
        // 移除已解码的数据（包括分隔符）
        buffer.erase(buffer.begin(), it + 1);
    }
    
    return messages;
}

} // namespace net
END_NAMESPACE_COMMON
