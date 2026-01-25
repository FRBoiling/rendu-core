//**********************************
//  Created by boil on 2026/1/25.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include "common/net/socket.h"
#include "common/io/io_context.h"
#include "common/log/logger.h"
#include <thread>
#include <chrono>

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

TEST_CASE("UdpSocket: 构造和基础状态", "[socket][udp]") {
    IoContext io(1);

    SECTION("构造成功") {
        UdpSocket socket(io);
        REQUIRE(socket.is_open());
    }

    SECTION("指定端口构造") {
        UdpSocket socket(io, 12345);
        REQUIRE(socket.is_open());
        auto endpoint = socket.local_endpoint();
        REQUIRE(endpoint.port() == 12345);
    }
}

TEST_CASE("UdpSocket: 发送接收", "[socket][udp]") {
    IoContext io_server(1);
    IoContext io_client(1);

    // 启动服务器线程
    std::thread server_thread([&io_server]() {
        io_server.run();
    });

    // 启动客户端线程
    std::thread client_thread([&io_client]() {
        io_client.run();
    });

    // 等待线程池启动
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    SECTION("发送和接收成功") {
        std::atomic<bool> received(false);
        std::vector<byte> received_data;

        // 创建服务器 socket
        UdpSocket server(io_server, 12346);

        // 异步接收
        server.async_receive_from(1024,
            [&](const boost::system::error_code& ec, std::vector<byte> data) {
                if (!ec) {
                    received_data = std::move(data);
                    received.store(true);
                }
            }
        );

        // 创建客户端 socket
        UdpSocket client(io_client);
        boost::asio::ip::udp::endpoint server_endpoint(
            boost::asio::ip::make_address("127.0.0.1"), 12346);

        // 发送数据
        std::vector<byte> send_data = {'H', 'e', 'l', 'l', 'o'};
        client.async_send_to(send_data, server_endpoint,
            [send_data](const boost::system::error_code& ec, size_t bytes_sent) {
                REQUIRE(!ec);
                REQUIRE(bytes_sent == send_data.size());
            }
        );

        // 等待接收
        for (int i = 0; i < 50 && !received.load(); ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        REQUIRE(received.load());
        REQUIRE(received_data == send_data);
    }

    // 清理
    io_server.stop();
    io_client.stop();
    if (server_thread.joinable()) {
        server_thread.join();
    }
    if (client_thread.joinable()) {
        client_thread.join();
    }
}

TEST_CASE("UdpSocket: 绑定和端口", "[socket][udp]") {
    IoContext io(1);

    SECTION("自动分配端口") {
        UdpSocket socket(io);
        auto endpoint = socket.local_endpoint();
        REQUIRE(endpoint.port() > 0);
    }

    SECTION("指定端口") {
        UdpSocket socket(io, 12347);
        auto endpoint = socket.local_endpoint();
        REQUIRE(endpoint.port() == 12347);
    }
}

TEST_CASE("UdpSocket: 广播", "[socket][udp]") {
    IoContext io(1);

    SECTION("启用广播") {
        UdpSocket socket(io);
        REQUIRE_NOTHROW(socket.set_broadcast(true));
        REQUIRE_NOTHROW(socket.set_broadcast(false));
    }
}

TEST_CASE("UdpSocket: 关闭", "[socket][udp]") {
    IoContext io(1);

    SECTION("关闭后 is_open 返回 false") {
        UdpSocket socket(io);
        REQUIRE(socket.is_open());

        socket.close();

        REQUIRE_FALSE(socket.is_open());
    }

    SECTION("多次关闭不会崩溃") {
        UdpSocket socket(io);
        REQUIRE_NOTHROW(socket.close());
        REQUIRE_NOTHROW(socket.close());
        REQUIRE_NOTHROW(socket.close());
    }
}
