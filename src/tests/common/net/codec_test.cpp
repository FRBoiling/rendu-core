//**********************************
//  Created by boil on 2026/1/24.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include "common/net/codec.h"
#include "common/io/io_context.h"
#include "common/log/logger.h"
#include <thread>

using namespace Rendu;
using namespace Rendu::net;
using namespace Rendu::io;
using namespace Rendu::log;

// 测试辅助：为日志系统创建一个简单的 IoContext
struct TestLoggerSetup {
    IoContext io;
    std::thread io_thread;

    TestLoggerSetup() : io(1) {
        init_default_io_context(io);
        // 在单独的线程中运行 io_context
        io_thread = std::thread([this]() {
            io.run();
        });
    }

    ~TestLoggerSetup() {
        io.stop();
        if (io_thread.joinable()) {
            io_thread.join();
        }
    }
};

// 全局日志初始化（每个测试文件一个）
static TestLoggerSetup g_logger_setup;

TEST_CASE("LengthPrefixCodec: 基本编码测试", "[codec][length_prefix]") {
    LengthPrefixCodec codec;

    SECTION("空数据编码") {
        ByteBuffer empty_data;
        ByteBuffer encoded = codec.encode(empty_data);

        REQUIRE(encoded.size() == 4);  // 4字节长度
        REQUIRE(encoded[0] == 0);
        REQUIRE(encoded[1] == 0);
        REQUIRE(encoded[2] == 0);
        REQUIRE(encoded[3] == 0);
    }

    SECTION("单个字节编码") {
        ByteBuffer data = {0x42};
        ByteBuffer encoded = codec.encode(data);

        REQUIRE(encoded.size() == 5);  // 4字节长度 + 1字节数据
        REQUIRE(encoded[0] == 0);
        REQUIRE(encoded[1] == 0);
        REQUIRE(encoded[2] == 0);
        REQUIRE(encoded[3] == 1);
        REQUIRE(encoded[4] == 0x42);
    }

    SECTION("多个字节编码") {
        ByteBuffer data = {0x01, 0x02, 0x03};
        ByteBuffer encoded = codec.encode(data);

        REQUIRE(encoded.size() == 7);  // 4字节长度 + 3字节数据
        REQUIRE(encoded[0] == 0);
        REQUIRE(encoded[1] == 0);
        REQUIRE(encoded[2] == 0);
        REQUIRE(encoded[3] == 3);
        REQUIRE(encoded[4] == 0x01);
        REQUIRE(encoded[5] == 0x02);
        REQUIRE(encoded[6] == 0x03);
    }

    SECTION("大端序检查") {
        ByteBuffer data(256, 0xFF);  // 256 字节
        ByteBuffer encoded = codec.encode(data);

        REQUIRE(encoded.size() == 260);  // 4字节长度 + 256字节数据
        // 大端序: 0x00000100
        REQUIRE(encoded[0] == 0);
        REQUIRE(encoded[1] == 0);
        REQUIRE(encoded[2] == 1);
        REQUIRE(encoded[3] == 0);
    }
}

TEST_CASE("LengthPrefixCodec: 基本解码测试", "[codec][length_prefix]") {
    LengthPrefixCodec codec;

    SECTION("完整消息解码") {
        ByteBuffer buffer = {0x00, 0x00, 0x00, 0x03, 0x01, 0x02, 0x03};
        auto messages = codec.decode(buffer);

        REQUIRE(messages.size() == 1);
        REQUIRE(messages[0].size() == 3);
        REQUIRE(messages[0][0] == 0x01);
        REQUIRE(messages[0][1] == 0x02);
        REQUIRE(messages[0][2] == 0x03);
    }

    SECTION("空消息解码") {
        ByteBuffer buffer = {0x00, 0x00, 0x00, 0x00};
        auto messages = codec.decode(buffer);

        REQUIRE(messages.size() == 1);
        REQUIRE(messages[0].empty());
    }

    SECTION("不完整消息-缺少长度") {
        ByteBuffer buffer = {0x00, 0x00};
        auto messages = codec.decode(buffer);

        REQUIRE(messages.empty());
    }

    SECTION("不完整消息-缺少数据") {
        ByteBuffer buffer = {0x00, 0x00, 0x00, 0x05, 0x01, 0x02};
        auto messages = codec.decode(buffer);

        REQUIRE(messages.empty());
    }

    SECTION("多个消息解码") {
        ByteBuffer buffer = {
            0x00, 0x00, 0x00, 0x02, 0xAA, 0xBB,
            0x00, 0x00, 0x00, 0x03, 0x01, 0x02, 0x03
        };
        auto messages = codec.decode(buffer);

        REQUIRE(messages.size() == 2);
        REQUIRE(messages[0].size() == 2);
        REQUIRE(messages[0][0] == 0xAA);
        REQUIRE(messages[0][1] == 0xBB);
        REQUIRE(messages[1].size() == 3);
        REQUIRE(messages[1][0] == 0x01);
        REQUIRE(messages[1][1] == 0x02);
        REQUIRE(messages[1][2] == 0x03);
    }

    SECTION("解码后缓冲区应该被清理") {
        ByteBuffer buffer = {
            0x00, 0x00, 0x00, 0x03, 0x01, 0x02, 0x03,
            0x00, 0x00, 0x00, 0x02, 0xAA, 0xBB
        };
        codec.decode(buffer);

        // 解码后的缓冲区应该为空
        REQUIRE(buffer.empty());
    }
}

TEST_CASE("LengthPrefixCodec: 编码解码往返测试", "[codec][length_prefix]") {
    LengthPrefixCodec codec;

    SECTION("单次往返") {
        ByteBuffer original = {0x01, 0x02, 0x03, 0x04, 0x05};
        ByteBuffer encoded = codec.encode(original);

        ByteBuffer buffer = encoded;
        auto decoded = codec.decode(buffer);

        REQUIRE(decoded.size() == 1);
        REQUIRE(decoded[0] == original);
    }

    SECTION("多次往返") {
        std::vector<ByteBuffer> originals = {
            {0x01},
            {0x01, 0x02},
            {0x01, 0x02, 0x03},
            ByteBuffer(100, 0xFF)
        };

        ByteBuffer buffer;
        for (const auto& original : originals) {
            ByteBuffer encoded = codec.encode(original);
            buffer.insert(buffer.end(), encoded.begin(), encoded.end());
        }

        auto decoded = codec.decode(buffer);

        REQUIRE(decoded.size() == originals.size());
        for (size_t i = 0; i < originals.size(); ++i) {
            REQUIRE(decoded[i] == originals[i]);
        }
    }
}

TEST_CASE("LengthPrefixCodec: 重置状态测试", "[codec][length_prefix]") {
    LengthPrefixCodec codec;

    SECTION("不完整消息后重置") {
        ByteBuffer buffer = {0x00, 0x00, 0x00, 0x05, 0x01, 0x02};
        codec.decode(buffer);  // 不完整，不返回消息

        codec.reset();

        // 重置后，之前的部分数据应该被忽略
        ByteBuffer new_data = {0x00, 0x00, 0x00, 0x02, 0xAA, 0xBB};
        buffer = new_data;
        auto messages = codec.decode(buffer);

        REQUIRE(messages.size() == 1);
        REQUIRE(messages[0].size() == 2);
    }
}

TEST_CASE("DelimiterCodec: 基本编码测试", "[codec][delimiter]") {
    DelimiterCodec codec('\n');

    SECTION("单行编码") {
        ByteBuffer data = {'H', 'e', 'l', 'l', 'o'};
        ByteBuffer encoded = codec.encode(data);

        REQUIRE(encoded.size() == 6);  // 5字节数据 + 1字节分隔符
        REQUIRE(encoded.back() == '\n');
    }

    SECTION("多行编码需要分别调用") {
        ByteBuffer line1 = {'H', 'e', 'l', 'l', 'o'};
        ByteBuffer line2 = {'W', 'o', 'r', 'l', 'd'};

        ByteBuffer encoded1 = codec.encode(line1);
        ByteBuffer encoded2 = codec.encode(line2);

        REQUIRE(encoded1.back() == '\n');
        REQUIRE(encoded2.back() == '\n');
    }
}

TEST_CASE("DelimiterCodec: 基本解码测试", "[codec][delimiter]") {
    DelimiterCodec codec('\n');

    SECTION("单行解码") {
        ByteBuffer buffer = {'H', 'e', 'l', 'l', 'o', '\n'};
        auto messages = codec.decode(buffer);

        REQUIRE(messages.size() == 1);
        REQUIRE(messages[0].size() == 5);
        REQUIRE(std::string(messages[0].begin(), messages[0].end()) == "Hello");
    }

    SECTION("多行解码") {
        ByteBuffer buffer = {
            'H', 'e', 'l', 'l', 'o', '\n',
            'W', 'o', 'r', 'l', 'd', '\n'
        };
        auto messages = codec.decode(buffer);

        REQUIRE(messages.size() == 2);
        REQUIRE(std::string(messages[0].begin(), messages[0].end()) == "Hello");
        REQUIRE(std::string(messages[1].begin(), messages[1].end()) == "World");
    }

    SECTION("无分隔符不返回消息") {
        ByteBuffer buffer = {'H', 'e', 'l', 'l', 'o'};
        auto messages = codec.decode(buffer);

        REQUIRE(messages.empty());
    }

    SECTION("空行") {
        ByteBuffer buffer = {'\n'};
        auto messages = codec.decode(buffer);

        REQUIRE(messages.size() == 1);
        REQUIRE(messages[0].empty());
    }
}

TEST_CASE("LineCodec: 默认换行符测试", "[codec][line]") {
    LineCodec codec;

    SECTION("自动使用 \\n 分隔符") {
        ByteBuffer buffer = {'H', 'i', '\n'};
        auto messages = codec.decode(buffer);

        REQUIRE(messages.size() == 1);
        REQUIRE(messages[0].size() == 2);
    }
}
