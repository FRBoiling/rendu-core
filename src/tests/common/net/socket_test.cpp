//**********************************
//  Created by boil on 2026/1/24.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "common/net/socket.h"
#include "common/io/io_context.h"
#include "common/log/logger.h"
#include <thread>
#include <chrono>
#include <atomic>

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

TEST_CASE("TcpSocket: 构造和基础状态", "[socket][tcp]") {
    IoContext io(1);  // 单线程

    SECTION("构造成功") {
        TcpSocket socket(io);
        REQUIRE(socket.is_open());
        REQUIRE_FALSE(socket.is_connected());
    }

    SECTION("移动构造被禁用") {
        TcpSocket socket1(io);
        // TcpSocket socket2(std::move(socket1));  // 编译错误
    }
}

TEST_CASE("TcpSocket: 连接测试", "[socket][tcp][connect]") {
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

    // 创建 acceptor
    TcpAcceptor acceptor(io_server, 0);
    auto endpoint = acceptor.local_endpoint();
    uint16_t port = endpoint.port();

    SECTION("连接成功") {
        std::atomic<bool> connected(false);
        std::atomic<bool> has_error(false);

        TcpSocket socket(io_client);
        socket.async_connect("127.0.0.1", port,
            [&](const boost::system::error_code& ec) {
                if (ec) {
                    has_error.store(true);
                } else {
                    connected.store(true);
                }
            }
        );

        // 等待连接完成
        for (int i = 0; i < 50; ++i) {
            if (connected.load()) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        REQUIRE(connected.load());
        REQUIRE_FALSE(has_error.load());
        REQUIRE(socket.is_connected());
    }

    SECTION("连接失败-端口未被监听") {
        std::atomic<bool> connected(false);
        std::atomic<bool> has_error(false);

        TcpSocket socket(io_client);
        socket.async_connect("127.0.0.1", 9999,
            [&](const boost::system::error_code& ec) {
                if (ec) {
                    has_error.store(true);
                }
                connected.store(true);
            }
        );

        // 等待连接完成（失败）
        for (int i = 0; i < 50; ++i) {
            if (connected.load()) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        REQUIRE(connected.load());
        REQUIRE(has_error.load());
        REQUIRE_FALSE(socket.is_connected());
    }

    // 清理
    io_server.stop();
    io_client.stop();
    server_thread.join();
    client_thread.join();
}

TEST_CASE("TcpSocket: 端点查询测试", "[socket][tcp][endpoint]") {
    IoContext io_server(1);
    IoContext io_client(1);

    std::thread server_thread([&io_server]() {
        io_server.run();
    });

    std::thread client_thread([&io_client]() {
        io_client.run();
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    TcpAcceptor acceptor(io_server, 0);
    auto server_endpoint = acceptor.local_endpoint();
    uint16_t port = server_endpoint.port();

    SECTION("客户端获取端点") {
        std::atomic<bool> connected(false);

        TcpSocket client_socket(io_client);
        client_socket.async_connect("127.0.0.1", port,
            [&](const boost::system::error_code& ec) {
                if (!ec) {
                    connected.store(true);
                    auto local_ep = client_socket.local_endpoint();
                    auto remote_ep = client_socket.remote_endpoint();

                    REQUIRE(remote_ep.port() == port);
                    REQUIRE(remote_ep.address() == boost::asio::ip::make_address("127.0.0.1"));
                }
            }
        );

        for (int i = 0; i < 50; ++i) {
            if (connected.load()) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        REQUIRE(connected.load());
    }

    io_server.stop();
    io_client.stop();
    server_thread.join();
    client_thread.join();
}

TEST_CASE("TcpSocket: 关闭测试", "[socket][tcp][close]") {
    IoContext io_server(1);
    IoContext io_client(1);

    std::thread server_thread([&io_server]() {
        io_server.run();
    });

    std::thread client_thread([&io_client]() {
        io_client.run();
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    TcpAcceptor acceptor(io_server, 0);
    auto server_endpoint = acceptor.local_endpoint();
    uint16_t port = server_endpoint.port();

    SECTION("正常关闭") {
        std::atomic<bool> connected(false);
        std::atomic<bool> closed(false);

        TcpSocket client_socket(io_client);
        client_socket.async_connect("127.0.0.1", port,
            [&](const boost::system::error_code& ec) {
                if (!ec) {
                    connected.store(true);
                    client_socket.close();
                    closed.store(true);
                }
            }
        );

        for (int i = 0; i < 50; ++i) {
            if (closed.load()) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        REQUIRE(connected.load());
        REQUIRE(closed.load());
        REQUIRE_FALSE(client_socket.is_open());
        REQUIRE_FALSE(client_socket.is_connected());
    }

    SECTION("双重关闭-幂等性") {
        std::atomic<bool> connected(false);

        TcpSocket client_socket(io_client);
        client_socket.async_connect("127.0.0.1", port,
            [&](const boost::system::error_code& ec) {
                if (!ec) {
                    connected.store(true);
                    client_socket.close();
                    client_socket.close();  // 第二次关闭不应该崩溃
                }
            }
        );

        for (int i = 0; i < 50; ++i) {
            if (connected.load()) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        REQUIRE(connected.load());
    }

    io_server.stop();
    io_client.stop();
    server_thread.join();
    client_thread.join();
}

TEST_CASE("TcpAcceptor: 监听测试", "[socket][acceptor][listen]") {
    IoContext io(1);

    SECTION("指定端口监听") {
        TcpAcceptor acceptor(io, 12345);
        REQUIRE(acceptor.is_listening());
        REQUIRE(acceptor.local_endpoint().port() == 12345);
    }

    SECTION("随机端口监听") {
        TcpAcceptor acceptor(io, 0);
        REQUIRE(acceptor.is_listening());
        REQUIRE(acceptor.local_endpoint().port() > 0);
    }
}

TEST_CASE("TcpAcceptor: 接受连接测试", "[socket][acceptor][accept]") {
    IoContext io_server(1);
    IoContext io_client(1);

    std::thread server_thread([&io_server]() {
        io_server.run();
    });

    std::thread client_thread([&io_client]() {
        io_client.run();
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    TcpAcceptor acceptor(io_server, 0);
    auto server_endpoint = acceptor.local_endpoint();
    uint16_t port = server_endpoint.port();

    std::atomic<bool> accepted(false);
    std::shared_ptr<TcpSocket> accepted_socket;

    // 服务器接受连接
    acceptor.async_accept(
        [&](std::shared_ptr<TcpSocket> socket, const boost::system::error_code& ec) {
            if (!ec) {
                accepted.store(true);
                accepted_socket = socket;
            }
        }
    );

    // 客户端连接
    TcpSocket client_socket(io_client);
    std::atomic<bool> connected(false);

    client_socket.async_connect("127.0.0.1", port,
        [&](const boost::system::error_code& ec) {
            if (!ec) {
                connected.store(true);
            }
        }
    );

    // 等待连接建立
    for (int i = 0; i < 50; ++i) {
        if (accepted.load() && connected.load()) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    REQUIRE(connected.load());
    REQUIRE(accepted.load());
    REQUIRE(accepted_socket != nullptr);
    REQUIRE(accepted_socket->is_connected());

    io_server.stop();
    io_client.stop();
    server_thread.join();
    client_thread.join();
}
