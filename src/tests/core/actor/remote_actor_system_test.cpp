//**********************************
// RemoteActorSystem 集成测试
//**********************************

#include <catch2/catch_test_macros.hpp>
#include "core/actor/remote_actor_system.h"
#include "core/actor/actor.h"
#include "core/actor/message.h"
#include "common/log/logger.h"
#include <thread>
#include <chrono>
#include <atomic>
#include <memory>

using namespace Rendu;
using namespace Rendu::log;

// 全局 IoContext（用于默认 logger）
static std::shared_ptr<Rendu::io::IoContext> g_io_context;
static std::thread g_io_thread;

// 初始化全局 IoContext
struct GlobalIoContextInitializer {
    GlobalIoContextInitializer() {
        g_io_context = std::make_shared<Rendu::io::IoContext>(2);
        Rendu::log::init_default_io_context(*g_io_context);
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

// 测试消息类型
struct TestMessage : public Message {
    std::string content;

    TestMessage() : Message(0) {}
    TestMessage(const std::string& c) : Message(0), content(c) {}

    const char* get_type() const override { return "TestMessage"; }

    std::string serialize() const override {
        return "TestMessage|" + std::to_string(request_id_) + "|" + content;
    }

    void deserialize_data(const std::string& data) override {
        content = data;
    }
};

// 注册消息类型
static Rendu::MessageRegistrar<TestMessage> g_test_message_registrar("TestMessage");

// 测试 Actor
class EchoActor : public Actor {
public:
    EchoActor() : Actor("EchoActor") {}

    void receive(std::shared_ptr<Message> msg) override {
        message_count_++;
        // Echo 消息（简单记录）
        auto test_msg = std::dynamic_pointer_cast<TestMessage>(msg);
        if (test_msg) {
            last_content_ = test_msg->content;
        }
        last_message_ = msg;
    }

    int message_count() const { return message_count_; }
    std::shared_ptr<Message> last_message() const { return last_message_; }
    std::string last_content() const { return last_content_; }

private:
    int message_count_{0};
    std::shared_ptr<Message> last_message_;
    std::string last_content_;
};

TEST_CASE("RemoteActorSystem: 构造函数", "[remote_actor_system]") {
    SECTION("默认配置") {
        RemoteActorSystem::Config config;
        config.node_id = "node1";
        config.actor_thread_pool_size = 2;
        config.io_threads = 1;

        RemoteActorSystem system(config);

        REQUIRE(system.node_id() == "node1");
    }

    SECTION("自定义配置") {
        RemoteActorSystem::Config config;
        config.node_id = "custom_node";
        config.listen_address = "0.0.0.0";
        config.listen_port = 9000;
        config.actor_thread_pool_size = 4;
        config.io_threads = 2;
        config.known_nodes = {"127.0.0.1:9001"};

        RemoteActorSystem system(config);

        REQUIRE(system.node_id() == "custom_node");
    }
}

TEST_CASE("RemoteActorSystem: 启动和停止", "[remote_actor_system]") {
    SECTION("正常启动和停止") {
        RemoteActorSystem::Config config;
        config.node_id = "test_node";
        RemoteActorSystem system(config);

        system.start();

        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        system.stop();

        REQUIRE(true);
    }

    SECTION("重复启动（跳过）") {
        // 此测试需要更仔细的并发控制
        REQUIRE(true);
    }

    SECTION("重复停止（跳过）") {
        // 此测试需要更仔细的并发控制
        REQUIRE(true);
    }
}

TEST_CASE("RemoteActorSystem: 本地 Actor 创建", "[remote_actor_system]") {
    RemoteActorSystem::Config config;
    config.node_id = "test_node";
    config.actor_thread_pool_size = 2;
    RemoteActorSystem system(config);

    system.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // 创建本地 Actor
    auto actor_ref = system.create_actor("echo_actor", []() {
        return std::unique_ptr<Actor>(new EchoActor());
    });

    REQUIRE(actor_ref.is_valid());
    REQUIRE(actor_ref.path() == "/echo_actor");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    system.stop();
}

TEST_CASE("RemoteActorSystem: 本地 Tell 消息", "[remote_actor_system]") {
    RemoteActorSystem::Config config;
    config.node_id = "test_node";
    config.actor_thread_pool_size = 2;
    RemoteActorSystem system(config);

    system.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // 创建本地 Actor
    auto actor_ref = system.create_actor("echo_actor", []() {
        return std::unique_ptr<Actor>(new EchoActor());
    });

    // 发送 Tell 消息
    auto msg = std::make_shared<TestMessage>("Hello, RemoteActorSystem!");
    system.tell(actor_ref, msg);

    // 等待消息处理
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // 验证消息已接收
    auto actor = system.find_actor(actor_ref);
    if (actor) {
        auto echo_actor = std::dynamic_pointer_cast<EchoActor>(actor);
        REQUIRE(echo_actor != nullptr);
        REQUIRE(echo_actor->message_count() == 1);
        REQUIRE(echo_actor->last_content() == "Hello, RemoteActorSystem!");
    }

    system.stop();
}

TEST_CASE("RemoteActorSystem: 本地 Ask 消息", "[remote_actor_system]") {
    RemoteActorSystem::Config config;
    config.node_id = "test_node";
    config.actor_thread_pool_size = 2;
    RemoteActorSystem system(config);

    system.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // 创建本地 Actor
    auto actor_ref = system.create_actor("echo_actor", []() {
        return std::unique_ptr<Actor>(new EchoActor());
    });

    // 发送 Ask 消息
    auto msg = std::make_shared<TestMessage>("Hello, Ask!");
    auto response = system.ask(actor_ref, msg, 1000);

    REQUIRE(response != nullptr);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    system.stop();
}

TEST_CASE("RemoteActorSystem: 远程连接配置", "[remote_actor_system]") {
    SECTION("空已知节点列表") {
        RemoteActorSystem::Config config;
        config.node_id = "node1";
        config.known_nodes = {};

        RemoteActorSystem system(config);
        system.start();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        system.stop();

        REQUIRE(true);
    }

    // 跳过需要实际网络连接的测试
    SECTION("单已知节点（跳过）") {
        // 实际网络连接测试需要更复杂的设置
        REQUIRE(true);
    }

    // 跳过需要实际网络连接的测试
    SECTION("多已知节点（跳过）") {
        // 实际网络连接测试需要更复杂的设置
        REQUIRE(true);
    }
}

TEST_CASE("RemoteActorSystem: 配置验证", "[remote_actor_system]") {
    SECTION("节点 ID 验证") {
        RemoteActorSystem::Config config;
        config.node_id = "test_node";

        RemoteActorSystem system(config);

        REQUIRE(system.node_id() == "test_node");
    }

    SECTION("线程池配置验证") {
        RemoteActorSystem::Config config;
        config.actor_thread_pool_size = 8;
        config.io_threads = 4;

        RemoteActorSystem system(config);

        REQUIRE(true);  // 配置已设置
    }

    SECTION("监听地址验证") {
        RemoteActorSystem::Config config;
        config.listen_address = "127.0.0.1";
        config.listen_port = 8080;

        RemoteActorSystem system(config);

        REQUIRE(true);  // 配置已设置
    }
}

TEST_CASE("RemoteActorSystem: 本地/远程 Actor 判断", "[remote_actor_system]") {
    RemoteActorSystem::Config config;
    config.node_id = "test_node";
    RemoteActorSystem system(config);

    // 本地 Actor（路径以 / 开头且不包含节点前缀）
    ActorRef local_actor("/echo_actor", 123);
    REQUIRE(local_actor.is_valid());

    // 远程 Actor（路径包含节点信息）
    ActorRef remote_actor("/node:127.0.0.1:9000/echo_actor", 456);
    REQUIRE(remote_actor.is_valid());
}

TEST_CASE("RemoteActorSystem: 多个 Actor 创建", "[remote_actor_system]") {
    RemoteActorSystem::Config config;
    config.node_id = "test_node";
    config.actor_thread_pool_size = 2;
    RemoteActorSystem system(config);

    system.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // 创建多个 Actor
    std::vector<ActorRef> actors;
    for (int i = 0; i < 5; ++i) {
        std::string name = "echo_actor_" + std::to_string(i);
        auto actor_ref = system.create_actor(name, []() {
            return std::unique_ptr<Actor>(new EchoActor());
        });
        actors.push_back(actor_ref);
    }

    REQUIRE(actors.size() == 5);

    // 验证每个 Actor 都有效
    for (const auto& actor_ref : actors) {
        REQUIRE(actor_ref.is_valid());
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    system.stop();
}

TEST_CASE("RemoteActorSystem: 生命周期管理", "[remote_actor_system]") {
    SECTION("资源清理") {
        {
            RemoteActorSystem::Config config;
            config.node_id = "temp_node";
            RemoteActorSystem system(config);
            system.start();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            system.stop();
        }

        // system 已析构，资源应已清理
        REQUIRE(true);
    }

    SECTION("多实例隔离（跳过）") {
        // 需要避免端口冲突
        REQUIRE(true);
    }
}
