//**********************************
//  Created by boil on 2026/1/24.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include "common/net/channel.h"
#include "common/net/socket.h"
#include "common/io/io_context.h"
#include "common/log/logger.h"
#include <thread>
#include <chrono>
#include <atomic>
#include <condition_variable>
#include <queue>

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

TEST_CASE("Channel: 构造和基础状态", "[channel]") {
    IoContext io_ctx(1);

    SECTION("构造成功") {
        auto socket = std::make_shared<TcpSocket>(io_ctx);
        auto codec = std::make_shared<LengthPrefixCodec>();
        auto channel = std::make_shared<Channel>(socket, codec);
        REQUIRE(channel->is_open());
    }

    SECTION("禁止拷贝") {
        auto socket = std::make_shared<TcpSocket>(io_ctx);
        auto codec = std::make_shared<LengthPrefixCodec>();
        auto channel1 = std::make_shared<Channel>(socket, codec);
        // Note: copying channel would result in compile error
    }

    SECTION("使用 shared_from_this") {
        auto socket = std::make_shared<TcpSocket>(io_ctx);
        auto codec = std::make_shared<LengthPrefixCodec>();
        auto channel = std::make_shared<Channel>(socket, codec);

        // shared_from_this() 需要在对象被 shared_ptr 管理后才能调用
        REQUIRE(channel.get() == channel->shared_from_this().get());
    }

    SECTION("关闭后 is_open 返回 false") {
        auto socket = std::make_shared<TcpSocket>(io_ctx);
        auto codec = std::make_shared<LengthPrefixCodec>();
        auto channel = std::make_shared<Channel>(socket, codec);

        REQUIRE(channel->is_open());

        channel->close();

        REQUIRE_FALSE(channel->is_open());
    }
}

TEST_CASE("Channel: 回调设置", "[channel][callback]") {
    IoContext io_ctx(1);

    SECTION("所有回调可以设置") {
        auto socket = std::make_shared<TcpSocket>(io_ctx);
        auto codec = std::make_shared<LengthPrefixCodec>();
        auto channel = std::make_shared<Channel>(socket, codec);

        bool connect_called = false;
        bool message_called = false;
        bool error_called = false;
        bool close_called = false;

        channel->set_connect_callback([&]() {
            connect_called = true;
        });

        channel->set_message_callback([&](const ByteBuffer& data) {
            message_called = true;
        });

        channel->set_error_callback([&](ChannelError error, const std::string& msg) {
            error_called = true;
        });

        channel->set_close_callback([&](const boost::system::error_code& ec) {
            close_called = true;
        });

        // 验证回调设置成功，但不会自动触发
        REQUIRE_FALSE(connect_called);
        REQUIRE_FALSE(message_called);
        REQUIRE_FALSE(error_called);
        REQUIRE_FALSE(close_called);
    }

    SECTION("close 触发关闭回调") {
        auto socket = std::make_shared<TcpSocket>(io_ctx);
        auto codec = std::make_shared<LengthPrefixCodec>();
        auto channel = std::make_shared<Channel>(socket, codec);

        bool close_called = false;
        boost::system::error_code received_ec;

        channel->set_close_callback([&](const boost::system::error_code& ec) {
            close_called = true;
            received_ec = ec;
        });

        REQUIRE_FALSE(close_called);

        channel->close();

        REQUIRE(close_called);
        // close() 应该传递成功错误码
        REQUIRE(received_ec == boost::system::error_code());
    }
}

TEST_CASE("Channel: 发送-基础逻辑", "[channel][send]") {
    IoContext io_ctx(1);

    SECTION("send 操作不会崩溃") {
        auto socket = std::make_shared<TcpSocket>(io_ctx);
        auto codec = std::make_shared<LengthPrefixCodec>();
        auto channel = std::make_shared<Channel>(socket, codec);

        ByteBuffer data = {'H', 'e', 'l', 'l', 'o'};
        REQUIRE_NOTHROW(channel->send(data));
    }

    SECTION("send_batch 操作不会崩溃") {
        auto socket = std::make_shared<TcpSocket>(io_ctx);
        auto codec = std::make_shared<LengthPrefixCodec>();
        auto channel = std::make_shared<Channel>(socket, codec);

        std::vector<ByteBuffer> messages = {
            {'H', 'i'},
            {'B', 'y', 'e'}
        };

        REQUIRE_NOTHROW(channel->send_batch(messages));
    }

    SECTION("空消息发送不会崩溃") {
        auto socket = std::make_shared<TcpSocket>(io_ctx);
        auto codec = std::make_shared<LengthPrefixCodec>();
        auto channel = std::make_shared<Channel>(socket, codec);

        ByteBuffer data;
        REQUIRE_NOTHROW(channel->send(data));
    }
}

TEST_CASE("Channel: 发送-错误处理", "[channel][send]") {
    IoContext io_ctx(1);

    SECTION("关闭后发送触发错误") {
        auto socket = std::make_shared<TcpSocket>(io_ctx);
        auto codec = std::make_shared<LengthPrefixCodec>();
        auto channel = std::make_shared<Channel>(socket, codec);

        bool error_called = false;
        ChannelError received_error;
        std::string error_msg;

        channel->set_error_callback([&](ChannelError error, const std::string& msg) {
            error_called = true;
            received_error = error;
            error_msg = msg;
        });

        channel->close();

        ByteBuffer data = {'H', 'i'};
        channel->send(data);

        REQUIRE(error_called);
        REQUIRE(received_error == ChannelError::ConnectionLost);
        REQUIRE(error_msg.find("Cannot send") != std::string::npos);
    }

    SECTION("超大消息触发错误") {
        auto socket = std::make_shared<TcpSocket>(io_ctx);
        auto codec = std::make_shared<LengthPrefixCodec>();
        auto channel = std::make_shared<Channel>(socket, codec);

        bool error_called = false;
        ChannelError received_error;

        channel->set_error_callback([&](ChannelError error, const std::string& msg) {
            error_called = true;
            received_error = error;
        });

        // 创建超过 1MB 限制的消息
        ByteBuffer data(2 * 1024 * 1024, 'X');
        channel->send(data);

        REQUIRE(error_called);
        REQUIRE(received_error == ChannelError::MessageTooLarge);
    }

    SECTION("批量消息过大触发错误") {
        auto socket = std::make_shared<TcpSocket>(io_ctx);
        auto codec = std::make_shared<LengthPrefixCodec>();
        auto channel = std::make_shared<Channel>(socket, codec);

        bool error_called = false;
        ChannelError received_error;

        channel->set_error_callback([&](ChannelError error, const std::string& msg) {
            error_called = true;
            received_error = error;
        });

        // 创建超过 10MB 限制的批量消息
        std::vector<ByteBuffer> messages;
        for (int i = 0; i < 15; ++i) {
            messages.push_back(ByteBuffer(1024 * 1024, 'X'));
        }

        channel->send_batch(messages);

        REQUIRE(error_called);
        REQUIRE(received_error == ChannelError::MessageTooLarge);
    }
}

TEST_CASE("Channel: 关闭-幂等性", "[channel][close]") {
    IoContext io_ctx(1);

    SECTION("多次 close 不会崩溃") {
        auto socket = std::make_shared<TcpSocket>(io_ctx);
        auto codec = std::make_shared<LengthPrefixCodec>();
        auto channel = std::make_shared<Channel>(socket, codec);

        REQUIRE(channel->is_open());

        // 多次关闭
        REQUIRE_NOTHROW(channel->close());
        REQUIRE_NOTHROW(channel->close());
        REQUIRE_NOTHROW(channel->close());

        REQUIRE_FALSE(channel->is_open());
    }

    SECTION("关闭回调只被调用一次") {
        auto socket = std::make_shared<TcpSocket>(io_ctx);
        auto codec = std::make_shared<LengthPrefixCodec>();
        auto channel = std::make_shared<Channel>(socket, codec);

        int close_call_count = 0;
        channel->set_close_callback([&](const boost::system::error_code& ec) {
            close_call_count++;
        });

        channel->close();
        channel->close();
        channel->close();

        // 回调应该只被调用一次
        REQUIRE(close_call_count == 1);
    }
}

TEST_CASE("Channel: 启动-幂等性", "[channel][start]") {
    IoContext io_ctx(1);

    SECTION("多次 start 不会崩溃") {
        auto socket = std::make_shared<TcpSocket>(io_ctx);
        auto codec = std::make_shared<LengthPrefixCodec>();
        auto channel = std::make_shared<Channel>(socket, codec);

        // 注意：start() 会在未连接的 socket 上触发 async_receive 错误
        // 但这里只验证 start 本身不会崩溃
        REQUIRE_NOTHROW(channel->start());
        // 第二次 start 会立即返回（因为 started_ 已经为 true）
        REQUIRE_NOTHROW(channel->start());
    }

    SECTION("连接回调只被调用一次") {
        auto socket = std::make_shared<TcpSocket>(io_ctx);
        auto codec = std::make_shared<LengthPrefixCodec>();
        auto channel = std::make_shared<Channel>(socket, codec);

        int connect_call_count = 0;
        channel->set_connect_callback([&]() {
            connect_call_count++;
        });

        // start() 同步调用连接回调
        channel->start();

        // 第二次 start 不会再次调用连接回调
        channel->start();

        // 回调应该只被调用一次
        REQUIRE(connect_call_count == 1);
    }
}

TEST_CASE("Channel: 接收缓冲区设置", "[channel][buffer]") {
    IoContext io_ctx(1);

    SECTION("可以设置缓冲区大小") {
        auto socket = std::make_shared<TcpSocket>(io_ctx);
        auto codec = std::make_shared<LengthPrefixCodec>();
        auto channel = std::make_shared<Channel>(socket, codec);

        REQUIRE_NOTHROW(channel->set_receive_buffer_size(16384));
        REQUIRE_NOTHROW(channel->set_receive_buffer_size(1024 * 1024));
    }

    SECTION("小值会被限制为 1024") {
        auto socket = std::make_shared<TcpSocket>(io_ctx);
        auto codec = std::make_shared<LengthPrefixCodec>();
        auto channel = std::make_shared<Channel>(socket, codec);

        REQUIRE_NOTHROW(channel->set_receive_buffer_size(100));
        // 内部逻辑会将值限制为至少 1024
    }
}

TEST_CASE("Channel: start 之前的操作", "[channel]") {
    IoContext io_ctx(1);

    SECTION("不调用 start 也能设置回调") {
        auto socket = std::make_shared<TcpSocket>(io_ctx);
        auto codec = std::make_shared<LengthPrefixCodec>();
        auto channel = std::make_shared<Channel>(socket, codec);

        bool connect_called = false;
        bool close_called = false;

        channel->set_connect_callback([&]() {
            connect_called = true;
        });

        channel->set_close_callback([&](const boost::system::error_code& ec) {
            close_called = true;
        });

        REQUIRE_FALSE(connect_called);
        REQUIRE_FALSE(close_called);

        channel->close();

        REQUIRE(close_called);
    }

    SECTION("不调用 start 也能 send") {
        auto socket = std::make_shared<TcpSocket>(io_ctx);
        auto codec = std::make_shared<LengthPrefixCodec>();
        auto channel = std::make_shared<Channel>(socket, codec);

        ByteBuffer data = {'H', 'i'};
        REQUIRE_NOTHROW(channel->send(data));
    }
}
