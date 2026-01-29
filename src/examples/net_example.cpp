#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <cstring>

#include "common/io/io_context.h"
#include "common/net/socket.h"
#include "common/net/channel.h"
#include "common/net/codec.h"

using namespace Rendu;

// ========== 自定义编解码器 ==========

/// 简单的文本编解码器
class TextCodec : public net::Codec {
public:
    /// 编码：在消息末尾添加换行符
    std::vector<byte> encode(const std::vector<byte>& data) override {
        std::vector<byte> result = data;
        result.push_back('\n');
        return result;
    }

    /// 解码：按换行符分割消息
    std::vector<std::vector<byte>> decode(std::vector<byte>& buffer) override {
        std::vector<std::vector<byte>> messages;

        size_t start = 0;
        for (size_t i = 0; i < buffer.size(); ++i) {
            if (buffer[i] == '\n') {
                std::vector<byte> message(buffer.begin() + start, buffer.begin() + i);
                if (!message.empty()) {
                    messages.push_back(std::move(message));
                }
                start = i + 1;
            }
        }

        // 移除已处理的数据
        if (start > 0) {
            buffer.erase(buffer.begin(), buffer.begin() + start);
        }

        return messages;
    }
};

/// 简单的长度前缀编解码器
class LengthPrefixCodec : public net::Codec {
public:
    /// 编码：[4字节长度][数据]
    std::vector<byte> encode(const std::vector<byte>& data) override {
        std::vector<byte> result;
        uint32_t length = static_cast<uint32_t>(data.size());

        // 写入长度（大端序）
        result.push_back((length >> 24) & 0xFF);
        result.push_back((length >> 16) & 0xFF);
        result.push_back((length >> 8) & 0xFF);
        result.push_back(length & 0xFF);

        // 写入数据
        result.insert(result.end(), data.begin(), data.end());

        return result;
    }

    /// 解码：解析长度前缀
    std::vector<std::vector<byte>> decode(std::vector<byte>& buffer) override {
        std::vector<std::vector<byte>> messages;

        size_t pos = 0;
        while (pos + 4 <= buffer.size()) {
            // 读取长度
            uint32_t length = (static_cast<uint32_t>(buffer[pos]) << 24) |
                             (static_cast<uint32_t>(buffer[pos+1]) << 16) |
                             (static_cast<uint32_t>(buffer[pos+2]) << 8) |
                             static_cast<uint32_t>(buffer[pos+3]);

            // 检查是否有足够的数据
            if (pos + 4 + length > buffer.size()) {
                break;  // 数据不完整，等待更多数据
            }

            // 提取消息
            std::vector<byte> message(buffer.begin() + pos + 4, buffer.begin() + pos + 4 + length);
            messages.push_back(std::move(message));

            pos += 4 + length;
        }

        // 移除已处理的数据
        if (pos > 0) {
            buffer.erase(buffer.begin(), buffer.begin() + pos);
        }

        return messages;
    }
};

// ========== 辅助函数 ==========

/// 将字符串转换为字节数组
std::vector<byte> to_bytes(const std::string& str) {
    return std::vector<byte>(str.begin(), str.end());
}

/// 将字节数组转换为字符串
std::string to_string(const std::vector<byte>& data) {
    return std::string(data.begin(), data.end());
}

int main() {
    std::cout << "========== 网络 示例程序 ==========" << std::endl;
    std::cout << std::endl;

    // 编解码器演示
    std::cout << "=== 编解码器演示 ===" << std::endl;

    TextCodec text_codec;
    LengthPrefixCodec length_codec;

    // 测试文本编解码器
    std::string original_msg = "Hello, World!";
    auto encoded = text_codec.encode(to_bytes(original_msg));
    std::cout << "[编解码] 原始消息: " << original_msg << std::endl;
    std::cout << "[编解码] 编码后: ";
    for (auto b : encoded) {
        if (b == '\n') {
            std::cout << "\\n";
        } else {
            std::cout << static_cast<char>(b);
        }
    }
    std::cout << std::endl;

    auto decoded = text_codec.decode(encoded);
    if (!decoded.empty()) {
        std::cout << "[编解码] 解码后: " << to_string(decoded[0]) << std::endl;
    }

    // 测试长度前缀编解码器
    std::cout << std::endl;
    std::cout << "[编解码] 测试长度前缀编解码器..." << std::endl;
    auto encoded2 = length_codec.encode(to_bytes("Test Message"));
    auto decoded2 = length_codec.decode(encoded2);
    if (!decoded2.empty()) {
        std::cout << "[编解码] 解码结果: " << to_string(decoded2[0]) << std::endl;
    }

    std::cout << std::endl;

    // TCP Echo 服务器示例
    std::cout << "=== TCP Echo 服务器示例 ===" << std::endl;
    std::cout << "注意：此示例仅展示代码结构，实际运行需要完整的网络环境" << std::endl;
    std::cout << std::endl;

    // 创建 IO Context
    io::IoContext io(2);

    // 创建服务器 Channel（假设存在 TcpServer 类）
    // 示例代码（注释掉，因为 TcpServer 可能不存在）
    /*
    auto codec = std::make_shared<TextCodec>();

    auto server = std::make_shared<net::TcpServer>(io, "127.0.0.1", 8080, codec);

    server->set_message_handler([](const std::string& client_id, const std::vector<byte>& data) {
        std::cout << "[服务器] 收到来自 " << client_id << " 的消息: "
                  << to_string(data) << std::endl;
        return to_bytes("Echo: " + to_string(data));
    });

    server->set_connection_handler([](const std::string& client_id, bool connected) {
        if (connected) {
            std::cout << "[服务器] 客户端连接: " << client_id << std::endl;
        } else {
            std::cout << "[服务器] 客户端断开: " << client_id << std::endl;
        }
    });

    server->start();
    std::cout << "[服务器] 已在 127.0.0.1:8080 启动" << std::endl;
    */

    // 客户端示例（注释掉）
    /*
    auto client = std::make_shared<net::TcpClient>(io, codec);

    client->set_message_handler([](const std::vector<byte>& data) {
        std::cout << "[客户端] 收到响应: " << to_string(data) << std::endl;
    });

    client->connect("127.0.0.1", 8080, [](bool success) {
        if (success) {
            std::cout << "[客户端] 连接成功" << std::endl;
            // 发送消息
            client->send(to_bytes("Hello Server"));
        } else {
            std::cout << "[客户端] 连接失败" << std::endl;
        }
    });
    */

    std::cout << std::endl;
    std::cout << "=== Socket 操作演示（伪代码）===" << std::endl;
    std::cout << std::endl;

    std::cout << "// 创建 TCP Socket" << std::endl;
    std::cout << "auto socket = std::make_shared<net::TcpSocket>(io);" << std::endl;
    std::cout << std::endl;

    std::cout << "// 异步连接" << std::endl;
    std::cout << "socket->async_connect(\"127.0.0.1\", 8080, [](const auto& ec) {" << std::endl;
    std::cout << "    if (ec) {" << std::endl;
    std::cout << "        std::cout << \"连接失败: \" << ec.message() << std::endl;" << std::endl;
    std::cout << "    } else {" << std::endl;
    std::cout << "        std::cout << \"连接成功\" << std::endl;" << std::endl;
    std::cout << "        // 发送数据" << std::endl;
    std::cout << "        socket->async_send(to_bytes(\"Hello\"), [](const auto& ec, size_t sent) {" << std::endl;
    std::cout << "            std::cout << \"已发送 \" << sent << \" 字节\" << std::endl;" << std::endl;
    std::cout << "        });" << std::endl;
    std::cout << "    }" << std::endl;
    std::cout << "});" << std::endl;
    std::cout << std::endl;

    std::cout << "// 异步接收数据" << std::endl;
    std::cout << "socket->async_receive(1024, [](const auto& ec, const auto& data) {" << std::endl;
    std::cout << "    if (ec) {" << std::endl;
    std::cout << "        std::cout << \"接收失败: \" << ec.message() << std::endl;" << std::endl;
    std::cout << "    } else {" << std::endl;
    std::cout << "        std::cout << \"收到数据: \" << to_string(data) << std::endl;" << std::endl;
    std::cout << "    }" << std::endl;
    std::cout << "});" << std::endl;
    std::cout << std::endl;

    std::cout << "// 关闭连接" << std::endl;
    std::cout << "socket->close();" << std::endl;

    std::cout << std::endl;
    std::cout << "=== 多条消息编解码演示 ===" << std::endl;

    // 测试多条消息的编解码
    std::string multi_msg = "第一条消息\n第二条消息\n第三条消息\n";
    auto encoded_multi = text_codec.encode(to_bytes(multi_msg));
    auto decoded_multi = text_codec.decode(encoded_multi);

    std::cout << "[编解码] 解码得到 " << decoded_multi.size() << " 条消息:" << std::endl;
    for (size_t i = 0; i < decoded_multi.size(); ++i) {
        std::cout << "  " << (i+1) << ". " << to_string(decoded_multi[i]) << std::endl;
    }

    std::cout << std::endl;
    std::cout << "========== 示例程序结束 ==========" << std::endl;

    return 0;
}
