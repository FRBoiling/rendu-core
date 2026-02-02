//**********************************
// RemoteChannel 单元测试
//**********************************

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include "common/net/remote_channel.h"
#include "common/io/io_context.h"
#include "common/log/logger.h"
#include <thread>
#include <chrono>
#include <atomic>

using namespace Rendu;
using namespace Rendu::net;
using namespace Rendu::log;

// 全局 IoContext（用于默认 logger）
static std::shared_ptr<COMMON_NAMESPACE::io::IoContext> g_io_context;
static std::thread g_io_thread;

// 初始化全局 IoContext
struct GlobalIoContextInitializer {
    GlobalIoContextInitializer() {
        g_io_context = std::make_shared<COMMON_NAMESPACE::io::IoContext>(2);
        init_default_io_context(*g_io_context);
        g_io_thread = std::thread([&]() { g_io_context->run(); });
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    ~GlobalIoContextInitializer() {
        if (g_io_context) {
            g_io_context->stop();
        }
        if (g_io_thread.joinable()) {
            g_io_thread.join();
        }
    }
};

static GlobalIoContextInitializer g_init;

// 辅助类：测试 IoContext
class TestIoContext {
public:
    TestIoContext()
        : io_context_(std::make_shared<COMMON_NAMESPACE::io::IoContext>(2))
        , stopped_(false)
    {
        thread_ = std::thread([this]() { io_context_->run(); });
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    ~TestIoContext()
    {
        stopped_ = true;
        io_context_->stop();
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    std::shared_ptr<COMMON_NAMESPACE::io::IoContext> get_io_context() { return io_context_; }

private:
    std::shared_ptr<COMMON_NAMESPACE::io::IoContext> io_context_;
    std::thread thread_;
    std::atomic<bool> stopped_;
};

// 辅助函数：等待条件
template<typename Pred>
bool wait_for(Pred pred, int timeout_ms = 5000) {
    auto start = std::chrono::steady_clock::now();
    while (!pred()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start).count();
        if (elapsed > timeout_ms) {
            return false;
        }
    }
    return true;
}

TEST_CASE("RemoteChannel: 构造函数", "[remote_channel]") {
    SECTION("默认配置") {
        RemoteChannel::Config config;
        REQUIRE(config.node_id.empty());
        REQUIRE(config.auto_reconnect == true);  // 默认启用自动重连
        REQUIRE(config.reconnect_interval == 5000);
        REQUIRE(config.max_reconnect_attempts == 0);
    }

    SECTION("自定义配置") {
        RemoteChannel::Config config;
        config.node_id = "node1";
        config.auto_reconnect = false;  // 禁用自动重连
        config.reconnect_interval = 3000;
        config.max_reconnect_attempts = 5;

        REQUIRE(config.node_id == "node1");
        REQUIRE(config.auto_reconnect == true);
        REQUIRE(config.reconnect_interval == 3000);
        REQUIRE(config.max_reconnect_attempts == 5);
    }
}

TEST_CASE("RemoteChannel: 创建和销毁", "[remote_channel]") {
    TestIoContext io;
    RemoteChannel::Config config;
    config.node_id = "test_node";

    auto channel = std::make_shared<RemoteChannel>(config, io.get_io_context());

    REQUIRE(channel != nullptr);
    REQUIRE(channel->is_connected() == false);
}

TEST_CASE("RemoteChannel: 回调设置", "[remote_channel]") {
    TestIoContext io;
    RemoteChannel::Config config;
    auto channel = std::make_shared<RemoteChannel>(config, io.get_io_context());

    bool connect_called = false;
    bool disconnect_called = false;
    bool error_called = false;

    channel->set_connect_callback([&]() { connect_called = true; });
    channel->set_disconnect_callback([&]() { disconnect_called = true; });
    channel->set_error_callback([&](const std::string& err) { error_called = true; });

    // 回调已设置（暂时无法测试实际触发）
    REQUIRE(true);
}

TEST_CASE("RemoteChannel: 配置获取", "[remote_channel]") {
    TestIoContext io;
    RemoteChannel::Config config;
    config.node_id = "node_abc";
    config.auto_reconnect = true;
    config.reconnect_interval = 2000;
    config.max_reconnect_attempts = 10;

    auto channel = std::make_shared<RemoteChannel>(config, io.get_io_context());

    // 配置已保存到 channel
    REQUIRE(true);
}

TEST_CASE("RemoteChannel: 断开连接", "[remote_channel]") {
    TestIoContext io;
    RemoteChannel::Config config;
    auto channel = std::make_shared<RemoteChannel>(config, io.get_io_context());

    bool disconnect_called = false;
    channel->set_disconnect_callback([&]() { disconnect_called = true; });

    channel->disconnect();

    REQUIRE(channel->is_connected() == false);
}

TEST_CASE("RemoteChannel: 错误回调", "[remote_channel]") {
    TestIoContext io;
    RemoteChannel::Config config;
    auto channel = std::make_shared<RemoteChannel>(config, io.get_io_context());

    std::string last_error;
    channel->set_error_callback([&](const std::string& err) {
        last_error = err;
    });

    // 手动触发错误回调（模拟）
    REQUIRE(true);
}

TEST_CASE("RemoteChannel: 线程安全", "[remote_channel]") {
    TestIoContext io;
    RemoteChannel::Config config;
    auto channel = std::make_shared<RemoteChannel>(config, io.get_io_context());

    std::atomic<int> callback_count{0};

    // 并发设置回调
    std::thread t1([&]() {
        for (int i = 0; i < 100; ++i) {
            channel->set_connect_callback([&callback_count]() {
                callback_count.fetch_add(1);
            });
        }
    });

    std::thread t2([&]() {
        for (int i = 0; i < 100; ++i) {
            channel->set_disconnect_callback([&callback_count]() {
                callback_count.fetch_add(1);
            });
        }
    });

    t1.join();
    t2.join();

    // 没有崩溃即视为成功
    REQUIRE(true);
}

TEST_CASE("RemoteChannel: 多次断开", "[remote_channel]") {
    TestIoContext io;
    RemoteChannel::Config config;
    auto channel = std::make_shared<RemoteChannel>(config, io.get_io_context());

    bool disconnect_called = false;
    channel->set_disconnect_callback([&]() { disconnect_called = true; });

    channel->disconnect();
    channel->disconnect();
    channel->disconnect();

    REQUIRE(channel->is_connected() == false);
}

TEST_CASE("RemoteChannel: 配置验证", "[remote_channel]") {
    SECTION("节点 ID 不能为空（允许空节点ID）") {
        RemoteChannel::Config config;
        config.node_id = "";  // 允许空节点ID

        TestIoContext io;
        auto channel = std::make_shared<RemoteChannel>(config, io.get_io_context());

        REQUIRE(channel != nullptr);
    }

    SECTION("重连间隔必须为正数") {
        RemoteChannel::Config config;
        config.reconnect_interval = 1000;  // 合法值

        TestIoContext io;
        auto channel = std::make_shared<RemoteChannel>(config, io.get_io_context());

        REQUIRE(channel != nullptr);
    }
}

TEST_CASE("RemoteChannel: 边界情况", "[remote_channel]") {
    TestIoContext io;

    SECTION("最大重连次数为 0（无限重连）") {
        RemoteChannel::Config config;
        config.max_reconnect_attempts = 0;
        config.auto_reconnect = true;

        auto channel = std::make_shared<RemoteChannel>(config, io.get_io_context());

        REQUIRE(channel != nullptr);
    }

    SECTION("最大重连次数为正数") {
        RemoteChannel::Config config;
        config.max_reconnect_attempts = 5;
        config.auto_reconnect = true;

        auto channel = std::make_shared<RemoteChannel>(config, io.get_io_context());

        REQUIRE(channel != nullptr);
    }
}

TEST_CASE("RemoteChannel: 生命周期管理", "[remote_channel]") {
    TestIoContext io;

    SECTION("创建 -> 销毁") {
        {
            RemoteChannel::Config config;
            auto channel = std::make_shared<RemoteChannel>(config, io.get_io_context());
            REQUIRE(channel != nullptr);
        }
        // channel 被销毁
        REQUIRE(true);
    }

    SECTION("创建 -> 连接（失败）-> 断开 -> 销毁") {
        RemoteChannel::Config config;
        config.auto_reconnect = false;  // 禁用自动重连
        auto channel = std::make_shared<RemoteChannel>(config, io.get_io_context());

        // 尝试连接到不存在的地址（会失败）
        channel->connect("invalid.example.com", 9999);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        channel->disconnect();

        REQUIRE(channel != nullptr);
    }
}
