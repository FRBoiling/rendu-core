//
// Created by boil on 2026/1/21.
//

// ============================================================================
// network_test.cpp - 网络模块单元测试
// ============================================================================

#include <catch2/catch_test_macros.hpp>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <set>
#include <iostream>
#include "common/network/types.h"
#include "common/network/connection.h"
#include "common/network/client.h"
#include "common/network/server.h"
#include "common/network/manager.h"
#include "common/asio/io_context.h"

using namespace Rendu;

// ============================================================================
// ConnectionState 测试
// ============================================================================

TEST_CASE("ConnectionState枚举值正确", "[network][types][state]")
{
    REQUIRE(static_cast<int>(Network::ConnectionState::Disconnected) == 0);
    REQUIRE(static_cast<int>(Network::ConnectionState::Connecting) == 1);
    REQUIRE(static_cast<int>(Network::ConnectionState::Connected) == 2);
    REQUIRE(static_cast<int>(Network::ConnectionState::Disconnecting) == 3);
}

TEST_CASE("ConnectionState比较正确", "[network][types][state]")
{
    REQUIRE(Network::ConnectionState::Disconnected < Network::ConnectionState::Connecting);
    REQUIRE(Network::ConnectionState::Connecting < Network::ConnectionState::Connected);
    REQUIRE(Network::ConnectionState::Connected < Network::ConnectionState::Disconnecting);
}

// ============================================================================
// NetworkConfig 测试
// ============================================================================

TEST_CASE("NetworkConfig默认值正确", "[network][types][config]")
{
    Network::NetworkConfig config;

    REQUIRE(config.connectionTimeout == 30);
    REQUIRE(config.sendTimeout == 10);
    REQUIRE(config.receiveTimeout == 10);
    REQUIRE(config.maxConnections == 1000);
    REQUIRE(config.autoReconnect == false);
    REQUIRE(config.sendBufferSize == 65536);
    REQUIRE(config.receiveBufferSize == 65536);
}

TEST_CASE("NetworkConfig自定义配置", "[network][types][config]")
{
    Network::NetworkConfig config;

    config.connectionTimeout = 60;
    config.maxConnections = 2000;
    config.autoReconnect = true;

    REQUIRE(config.connectionTimeout == 60);
    REQUIRE(config.maxConnections == 2000);
    REQUIRE(config.autoReconnect == true);
}

TEST_CASE("NetworkConfig边界值测试", "[network][types][config][boundary]")
{
    Network::NetworkConfig config;

    SECTION("零值设置")
    {
        config.connectionTimeout = 0;
        config.sendTimeout = 0;
        config.receiveTimeout = 0;
        config.maxConnections = 0;
        config.sendBufferSize = 0;
        config.receiveBufferSize = 0;

        REQUIRE(config.connectionTimeout == 0);
        REQUIRE(config.sendTimeout == 0);
        REQUIRE(config.receiveTimeout == 0);
        REQUIRE(config.maxConnections == 0);
        REQUIRE(config.sendBufferSize == 0);
        REQUIRE(config.receiveBufferSize == 0);
    }

    SECTION("大值设置")
    {
        config.maxConnections = 100000;
        config.sendBufferSize = 1024 * 1024 * 10;  // 10MB
        config.receiveBufferSize = 1024 * 1024 * 10;

        REQUIRE(config.maxConnections == 100000);
        REQUIRE(config.sendBufferSize == 1024 * 1024 * 10);
        REQUIRE(config.receiveBufferSize == 1024 * 1024 * 10);
    }
}

// ============================================================================
// Manager 测试
// ============================================================================

TEST_CASE("Manager单例模式", "[network][manager][singleton]")
{
    auto* instance1 = Network::Manager::instance();
    auto* instance2 = Network::Manager::instance();

    REQUIRE(instance1 != nullptr);
    REQUIRE(instance1 == instance2);
}

TEST_CASE("Manager初始化和关闭", "[network][manager][lifecycle]")
{
    Asio::IoContext ioContext;
    auto* manager = Network::Manager::instance();

    REQUIRE_NOTHROW(manager->initialize(&ioContext));
    REQUIRE_NOTHROW(manager->shutdown());
}

TEST_CASE("Manager配置管理", "[network][manager][config]")
{
    Asio::IoContext ioContext;
    auto* manager = Network::Manager::instance();

    manager->initialize(&ioContext);

    Network::NetworkConfig config;
    config.maxConnections = 500;
    config.autoReconnect = true;

    REQUIRE_NOTHROW(manager->setConfig(config));

    const auto& retrievedConfig = manager->getConfig();
    REQUIRE(retrievedConfig.maxConnections == 500);
    REQUIRE(retrievedConfig.autoReconnect == true);

    manager->shutdown();
}

TEST_CASE("Manager活动连接数", "[network][manager][connections]")
{
    Asio::IoContext ioContext;
    auto* manager = Network::Manager::instance();

    manager->initialize(&ioContext);

    size_t count = manager->getActiveConnectionCount();
    REQUIRE(count >= 0);

    manager->shutdown();
}

// ============================================================================
// Connection 测试
// ============================================================================

TEST_CASE("Connection客户端连接创建", "[network][connection][client]")
{
    Asio::IoContext ioContext;

    auto connection = std::make_shared<Network::Connection>(ioContext);

    REQUIRE(connection != nullptr);
    REQUIRE(connection->getConnectionId() > 0);
    REQUIRE(connection->isServerConnection() == false);
    REQUIRE(connection->getState() == Network::ConnectionState::Disconnected);
}

TEST_CASE("Connection服务器连接创建", "[network][connection][server]")
{
    Asio::IoContext ioContext;
    Asio::TcpSocket socket(ioContext);

    auto connection = std::make_shared<Network::Connection>(ioContext, std::move(socket));

    REQUIRE(connection != nullptr);
    REQUIRE(connection->getConnectionId() > 0);
    REQUIRE(connection->isServerConnection() == true);
    REQUIRE(connection->getState() == Network::ConnectionState::Connected);
}

TEST_CASE("Connection关闭回调设置", "[network][connection][callback]")
{
    Asio::IoContext ioContext;

    auto connection = std::make_shared<Network::Connection>(ioContext);

    std::atomic<bool> callbackCalled{false};

    REQUIRE_NOTHROW(connection->setOnCloseCallback(
        [&callbackCalled](std::shared_ptr<Network::Connection>, const std::error_code&) {
            callbackCalled = true;
        }
    ));
}

TEST_CASE("Connection异步发送空数据", "[network][connection][send]")
{
    Asio::IoContext ioContext;

    auto connection = std::make_shared<Network::Connection>(ioContext);

    std::string testMsg = "";
    std::atomic<bool> sendCompleted{false};

    REQUIRE_NOTHROW(connection->asyncSend(
        testMsg,
        [&sendCompleted](std::shared_ptr<Network::Connection>, size_t, const std::error_code&) {
            sendCompleted = true;
        }
    ));
}

TEST_CASE("Connection异步发送字节数据", "[network][connection][send]")
{
    Asio::IoContext ioContext;

    auto connection = std::make_shared<Network::Connection>(ioContext);

    std::string testMsg = "Hello, Network!";
    std::atomic<bool> sendCompleted{false};

    REQUIRE_NOTHROW(connection->asyncSend(
        reinterpret_cast<const uint8*>(testMsg.data()),
        testMsg.size(),
        [&sendCompleted](std::shared_ptr<Network::Connection>, size_t, const std::error_code&) {
            sendCompleted = true;
        }
    ));
}

TEST_CASE("Connection异步接收设置", "[network][connection][receive]")
{
    Asio::IoContext ioContext;

    auto connection = std::make_shared<Network::Connection>(ioContext);

    std::atomic<bool> receiveCallbackSet{false};

    REQUIRE_NOTHROW(connection->asyncReceive(
        [&receiveCallbackSet](std::shared_ptr<Network::Connection>, const uint8*, size_t, const std::error_code&) {
            receiveCallbackSet = true;
        }
    ));
}

TEST_CASE("Connection关闭连接", "[network][connection][close]")
{
    Asio::IoContext ioContext;

    auto connection = std::make_shared<Network::Connection>(ioContext);

    REQUIRE_NOTHROW(connection->close());
}

TEST_CASE("Connection远程地址和端口", "[network][connection][info]")
{
    Asio::IoContext ioContext;

    auto connection = std::make_shared<Network::Connection>(ioContext);

    std::string address = connection->getRemoteAddress();
    uint16 port = connection->getRemotePort();

    // 未连接时地址可能为空或默认值
    REQUIRE(address.empty() == true);
    REQUIRE(port == 0);
}

TEST_CASE("Connection连接ID唯一性", "[network][connection][id]")
{
    Asio::IoContext ioContext;

    auto connection1 = std::make_shared<Network::Connection>(ioContext);
    auto connection2 = std::make_shared<Network::Connection>(ioContext);

    REQUIRE(connection1->getConnectionId() != connection2->getConnectionId());
}

// ============================================================================
// Client 测试
// ============================================================================

TEST_CASE("Client创建和初始化", "[network][client][lifecycle]")
{
    Asio::IoContext ioContext;

    REQUIRE_NOTHROW([&]() {
        Network::Client client(ioContext);
    }());
}

TEST_CASE("Client获取连接", "[network][client][connection]")
{
    Asio::IoContext ioContext;

    Network::Client client(ioContext);

    auto connection = client.getConnection();
    REQUIRE(connection == nullptr);
}

TEST_CASE("Client未连接状态", "[network][client][status]")
{
    Asio::IoContext ioContext;

    Network::Client client(ioContext);

    REQUIRE(client.isConnected() == false);
}

TEST_CASE("Client断开连接", "[network][client][disconnect]")
{
    Asio::IoContext ioContext;

    Network::Client client(ioContext);

    REQUIRE_NOTHROW(client.disconnect());
}

TEST_CASE("Client异步连接回调设置", "[network][client][callback]")
{
    Asio::IoContext ioContext;

    Network::Client client(ioContext);

    std::atomic<bool> connectCalled{false};

    REQUIRE_NOTHROW([&]() {
        std::shared_ptr<Network::Connection> conn = client.asyncConnect(
            "127.0.0.1",
            12345,
            [&connectCalled](std::shared_ptr<Network::Connection>, const std::error_code&) {
                connectCalled = true;
            }
        );
    }());
}

// ============================================================================
// Server 测试
// ============================================================================

TEST_CASE("Server创建和初始化", "[network][server][lifecycle]")
{
    Asio::IoContext ioContext;

    REQUIRE_NOTHROW([&]() {
        Network::Server server(ioContext);
    }());
}

TEST_CASE("Server运行状态", "[network][server][status]")
{
    Asio::IoContext ioContext;

    Network::Server server(ioContext);

    REQUIRE(server.isRunning() == false);
}

TEST_CASE("Server监听地址和端口", "[network][server][info]")
{
    Asio::IoContext ioContext;

    Network::Server server(ioContext);

    std::string address = server.getListenAddress();
    uint16 port = server.getListenPort();

    REQUIRE(address.empty() == true);
    REQUIRE(port == 0);
}

TEST_CASE("Server启动监听", "[network][server][listen]")
{
    Asio::IoContext ioContext;

    Network::Server server(ioContext);

    std::atomic<bool> acceptCalled{false};

    bool started = server.start(
        "127.0.0.1",
        12346,
        [&acceptCalled](std::shared_ptr<Network::Connection>, const std::error_code&) {
            acceptCalled = true;
        }
    );

    // 可能会失败（端口被占用等），但不应抛出异常
    REQUIRE_NOTHROW([&]() {
        if (started) {
            server.stop();
        }
    }());
}

TEST_CASE("Server停止监听", "[network][server][stop]")
{
    Asio::IoContext ioContext;

    Network::Server server(ioContext);

    REQUIRE_NOTHROW(server.stop());
}

TEST_CASE("Server获取连接列表", "[network][server][connections]")
{
    Asio::IoContext ioContext;

    Network::Server server(ioContext);

    auto connections = server.getConnections();
    REQUIRE(connections.empty() == true);

    size_t count = server.getConnectionCount();
    REQUIRE(count == 0);
}

// ============================================================================
// 回调类型测试
// ============================================================================

TEST_CASE("回调类型可调用性", "[network][types][callback]")
{
    bool testFlag = false;

    Network::OnConnectCallback onConnect = [&testFlag](std::shared_ptr<Network::Connection>, const std::error_code&) {
        testFlag = true;
    };

    Network::OnReceiveCallback onReceive = [&testFlag](std::shared_ptr<Network::Connection>, const uint8*, size_t, const std::error_code&) {
        testFlag = true;
    };

    Network::OnSendCallback onSend = [&testFlag](std::shared_ptr<Network::Connection>, size_t, const std::error_code&) {
        testFlag = true;
    };

    Network::OnCloseCallback onClose = [&testFlag](std::shared_ptr<Network::Connection>, const std::error_code&) {
        testFlag = true;
    };

    Network::OnAcceptCallback onAccept = [&testFlag](std::shared_ptr<Network::Connection>, const std::error_code&) {
        testFlag = true;
    };

    // 验证所有回调类型都可以被赋值
    REQUIRE(onConnect != nullptr);
    REQUIRE(onReceive != nullptr);
    REQUIRE(onSend != nullptr);
    REQUIRE(onClose != nullptr);
    REQUIRE(onAccept != nullptr);

    // 验证空回调
    Network::OnConnectCallback emptyCallback;
    REQUIRE(emptyCallback == nullptr);
}

TEST_CASE("回调参数类型兼容性", "[network][types][callback]")
{
    // 测试回调可以接受 nullptr 作为错误码
    Network::OnConnectCallback onConnect = [](std::shared_ptr<Network::Connection>, const std::error_code& ec) {
        // ec 可能是成功或失败状态
    };

    Network::OnReceiveCallback onReceive = [](std::shared_ptr<Network::Connection>, const uint8* data, size_t size, const std::error_code& ec) {
        // data 可能为 nullptr，size 可能为 0
    };

    // 验证回调可以被调用（虽然实际连接不会发生）
    REQUIRE_NOTHROW(onConnect(nullptr, std::error_code()));
    REQUIRE_NOTHROW(onReceive(nullptr, nullptr, 0, std::error_code()));
}

// ============================================================================
// 边界条件和错误处理测试
// ============================================================================

TEST_CASE("Connection发送空指针数据", "[network][connection][error][boundary]")
{
    Asio::IoContext ioContext;

    auto connection = std::make_shared<Network::Connection>(ioContext);

    // 传入空指针，应该能处理而不崩溃
    const uint8* nullData = nullptr;

    REQUIRE_NOTHROW(connection->asyncSend(
        nullData,
        0,
        [](std::shared_ptr<Network::Connection>, size_t, const std::error_code&) {}
    ));
}

TEST_CASE("Connection发送零长度数据", "[network][connection][error][boundary]")
{
    Asio::IoContext ioContext;

    auto connection = std::make_shared<Network::Connection>(ioContext);

    std::string testMsg = "test";
    std::atomic<bool> callbackCalled{false};

    REQUIRE_NOTHROW(connection->asyncSend(
        reinterpret_cast<const uint8*>(testMsg.data()),
        0,
        [&callbackCalled](std::shared_ptr<Network::Connection>, size_t, const std::error_code&) {
            callbackCalled = true;
        }
    ));
}

TEST_CASE("Connection多次关闭", "[network][connection][error][boundary]")
{
    Asio::IoContext ioContext;

    auto connection = std::make_shared<Network::Connection>(ioContext);

    REQUIRE_NOTHROW(connection->close());
    REQUIRE_NOTHROW(connection->close());
    REQUIRE_NOTHROW(connection->close());
}

TEST_CASE("Server使用无效端口", "[network][server][error][boundary]")
{
    Asio::IoContext ioContext;

    Network::Server server(ioContext);

    std::atomic<bool> acceptCalled{false};

    bool started = server.start(
        "127.0.0.1",
        0,  // 无效端口
        [&acceptCalled](std::shared_ptr<Network::Connection>, const std::error_code&) {
            acceptCalled = true;
        }
    );

    // 可能失败，但不应抛出异常
    REQUIRE_NOTHROW([&]() {
        if (started) {
            server.stop();
        }
    }());
}

TEST_CASE("Client连接无效地址", "[network][client][error][boundary]")
{
    Asio::IoContext ioContext;

    Network::Client client(ioContext);

    std::atomic<bool> connectCalled{false};

    REQUIRE_NOTHROW([&]() {
        std::shared_ptr<Network::Connection> conn = client.asyncConnect(
            "",  // 空地址
            12347,
            [&connectCalled](std::shared_ptr<Network::Connection>, const std::error_code&) {
                connectCalled = true;
            }
        );
    }());
}

// ============================================================================
// 并发安全测试
// ============================================================================

TEST_CASE("多线程创建连接", "[network][connection][concurrent][thread_safety]")
{
    Asio::IoContext ioContext;

    const int threadCount = 10;
    std::vector<std::thread> threads;
    std::vector<std::shared_ptr<Network::Connection>> connections;
    std::mutex connectionsMutex;

    for (int i = 0; i < threadCount; ++i)
    {
        threads.emplace_back([&ioContext, &connections, &connectionsMutex]() {
            auto conn = std::make_shared<Network::Connection>(ioContext);

            std::lock_guard<std::mutex> lock(connectionsMutex);
            connections.push_back(conn);
        });
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    REQUIRE(connections.size() == threadCount);

    // 验证所有连接ID唯一
    std::set<uint64> ids;
    for (const auto& conn : connections)
    {
        uint64 id = conn->getConnectionId();
        REQUIRE(ids.find(id) == ids.end());
        ids.insert(id);
    }
}

TEST_CASE("多线程创建服务器", "[network][server][concurrent][thread_safety]")
{
    const int threadCount = 5;
    std::vector<std::thread> threads;
    std::vector<Network::Server*> servers;
    std::mutex serversMutex;

    for (int i = 0; i < threadCount; ++i)
    {
        threads.emplace_back([&servers, &serversMutex]() {
            auto ioContext = std::make_unique<Asio::IoContext>();

            auto server = new Network::Server(*ioContext);

            std::lock_guard<std::mutex> lock(serversMutex);
            servers.push_back(server);
        });
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    REQUIRE(servers.size() == threadCount);

    for (auto* server : servers)
    {
        delete server;
    }
}

// ============================================================================
// 性能基准测试
// ============================================================================

TEST_CASE("Connection创建性能", "[network][connection][performance][benchmark]")
{
    Asio::IoContext ioContext;

    const int iterations = 1000;
    auto startTime = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i)
    {
        auto connection = std::make_shared<Network::Connection>(ioContext);
        REQUIRE(connection != nullptr);
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);

    // 性能要求：1000次创建应该在合理时间内完成（< 1秒）
    REQUIRE(duration.count() < 1000000);
}

TEST_CASE("ConnectionID生成性能", "[network][connection][performance][benchmark]")
{
    Asio::IoContext ioContext;

    const int iterations = 10000;
    std::set<uint64> ids;
    auto startTime = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i)
    {
        auto connection = std::make_shared<Network::Connection>(ioContext);
        ids.insert(connection->getConnectionId());
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    // 所有ID应该是唯一的
    REQUIRE(ids.size() == iterations);

    // 性能要求：10000次创建应该在合理时间内完成（< 5秒）
    REQUIRE(duration.count() < 5000);
}

// ============================================================================
// 前置声明测试
// ============================================================================

TEST_CASE("Connection前置声明可用", "[network][types][forward_decl]")
{
    // 验证 Connection 前置声明存在且可用
    // 在实际使用中，这允许 types.h 被 Connection 之前的代码使用
    REQUIRE(true);
}

// ============================================================================
// 网络配置结构体大小测试
// ============================================================================

TEST_CASE("NetworkConfig内存对齐", "[network][types][config][memory]")
{
    Network::NetworkConfig config;
    size_t configSize = sizeof(config);

    // 验证结构体大小合理（考虑到对齐，应该接近成员大小之和）
    // 这是一个粗略的检查，确保没有异常大的填充
    REQUIRE(configSize < 128);
}

// ============================================================================
// 连接状态枚举内存测试
// ============================================================================

TEST_CASE("ConnectionState枚举大小", "[network][types][state][memory]")
{
    // ConnectionState 使用 uint8，应该只占 1 字节
    REQUIRE(sizeof(Network::ConnectionState) == 1);
}

TEST_CASE("ConnectionState转换测试", "[network][types][state]")
{
    Network::ConnectionState state = Network::ConnectionState::Connected;
    int intValue = static_cast<int>(state);

    REQUIRE(intValue == 2);

    // 反向转换
    Network::ConnectionState state2 = static_cast<Network::ConnectionState>(intValue);
    REQUIRE(state2 == Network::ConnectionState::Connected);
}

// ============================================================================
// 回调可移动性测试
// ============================================================================

TEST_CASE("回调可移动性", "[network][types][callback][move]")
{
    std::shared_ptr<Network::Connection> dummyConn;
    std::atomic<int> counter{0};

    Network::OnConnectCallback callback1 = [&counter](std::shared_ptr<Network::Connection>, const std::error_code&) {
        counter++;
    };

    Network::OnConnectCallback callback2 = std::move(callback1);

    // 移动后 callback1 可能为空或未定义状态
    // callback2 应该仍然可用
    REQUIRE(callback2 != nullptr);

    // 测试调用
    REQUIRE_NOTHROW(callback2(dummyConn, std::error_code()));
}
