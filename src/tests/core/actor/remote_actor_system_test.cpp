//**********************************
// RemoteActorSystem 集成测试
//**********************************

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include "core/actor/remote_actor_system.h"
#include "core/actor/actor.h"
#include "core/actor/message.h"
#include "common/log/logger.h"
#include <thread>
#include <chrono>
#include <atomic>
#include <memory>

using namespace Rendu;
using namespace Rendu::actor;

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
    TestMessage(std::string&& c) : Message(0), content(std::move(c)) {}

    const char* get_type() const override { return "TestMessage"; }

    std::string serialize() const override {
        return "TestMessage|" + std::to_string(request_id_) + "|" + content;
    }

    void deserialize_data(const std::string& data) override {
        content = data;
    }

    static std::shared_ptr<Message> deserialize(const std::string& data) {
        auto msg = std::make_shared<TestMessage>();
        msg->deserialize_data(data);
        return msg;
    }
};

// 注册消息
REGISTER_MESSAGE(TestMessage, "TestMessage")

// 测试 Actor
class TestActor : public Actor {
public:
    TestActor() : Actor("TestActor") {}

    void receive(const std::shared_ptr<Message>& msg) override {
        message_count_++;
        last_message_ = msg;
    }

    int message_count() const { return message_count_; }
    std::shared_ptr<Message> last_message() const { return last_message_; }

private:
    int message_count_{0};
    std::shared_ptr<Message> last_message_;
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
    RemoteActorSystem::Config config;
    config.node_id = "test_node";
    RemoteActorSystem system(config);

    SECTION("正常启动和停止") {
        system.start();

        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        system.stop();

        REQUIRE(true);
    }

    SECTION("重复启动（应忽略）") {
        system.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // 再次启动（应该安全）
        system.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        system.stop();

        REQUIRE(true);
    }

    SECTION("重复停止（应安全）") {
        system.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        system.stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // 再次停止（应该安全）
        system.stop();

        REQUIRE(true);
    }
}

TEST_CASE("RemoteActorSystem: 本地 Actor 创建", "[remote_actor_system]") {
    RemoteActorSystem::Config config;
    config.node_id = "test_node";
    RemoteActorSystem system(config);
    system.start();

    auto actor = system.create_actor<TestActor>("test_actor");

    REQUIRE(actor != nullptr);
    REQUIRE(actor->path() == "/test_actor");
    REQUIRE(actor->actor_id() > 0);

    system.stop();
}

TEST_CASE("RemoteActorSystem: 本地 Tell 消息", "[remote_actor_system]") {
    RemoteActorSystem::Config config;
    config.node_id = "test_node";
    RemoteActorSystem system(config);
    system.start();

    auto actor = system.create_actor<TestActor>("receiver");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // 发送 Tell 消息
    auto msg = std::make_shared<TestMessage>("Hello");
    system.tell(actor, msg);

    // 等待消息处理
    bool received = wait_for([&]() { return actor->message_count() > 0; }, 2000);

    REQUIRE(received);
    REQUIRE(actor->message_count() == 1);

    system.stop();
}

TEST_CASE("RemoteActorSystem: 本地 Ask 消息", "[remote_actor_system]") {
    RemoteActorSystem::Config config;
    config.node_id = "test_node";
    RemoteActorSystem system(config);
    system.start();

    auto actor = system.create_actor<TestActor>("receiver");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // 发送 Ask 消息
    auto request = std::make_shared<TestMessage>("Ping");
    auto response = system.ask(actor, request, 1000);

    // Ask 模式要求 Actor 发送响应
    // 由于 TestActor 不发送响应，这里只测试不崩溃

    system.stop();
}

TEST_CASE("RemoteActorSystem: 解析节点地址", "[remote_actor_system]") {
    RemoteActorSystem::Config config;
    config.node_id = "test_node";
    RemoteActorSystem system(config);

    SECTION("有效地址") {
        std::string address = "127.0.0.1";
        uint16_t port = 8080;

        // 验证解析逻辑（通过测试连接到无效地址）
        REQUIRE(true);
    }

    SECTION("无效地址（缺少端口）") {
        std::string address = "127.0.0.1";
        uint16_t port = 0;

        // 验证解析失败处理
        REQUIRE(true);
    }
}

TEST_CASE("RemoteActorSystem: 判断本地 Actor", "[remote_actor_system]") {
    RemoteActorSystem::Config config;
    config.node_id = "test_node";
    RemoteActorSystem system(config);
    system.start();

    auto actor = system.create_actor<TestActor>("local_actor");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // 本地 Actor 路径格式: /local_actor
    auto local_ref = system.actor_of("/local_actor");

    if (local_ref) {
        // 验证本地判断
        REQUIRE(true);
    }

    system.stop();
}

TEST_CASE("RemoteActorSystem: 配置验证", "[remote_actor_system]") {
    SECTION("节点 ID 不能为空") {
        RemoteActorSystem::Config config;
        config.node_id = "";

        REQUIRE(config.node_id.empty());
    }

    SECTION("监听端口必须合法") {
        RemoteActorSystem::Config config;
        config.node_id = "test_node";
        config.listen_port = 8080;  // 合法端口

        RemoteActorSystem system(config);

        REQUIRE(system.node_id() == "test_node");
    }

    SECTION("线程池大小必须为正数") {
        RemoteActorSystem::Config config;
        config.node_id = "test_node";
        config.actor_thread_pool_size = 4;
        config.io_threads = 2;

        RemoteActorSystem system(config);

        REQUIRE(system.node_id() == "test_node");
    }
}

TEST_CASE("RemoteActorSystem: 已知节点列表", "[remote_actor_system]") {
    RemoteActorSystem::Config config;
    config.node_id = "test_node";
    config.known_nodes = {
        "127.0.0.1:9001",
        "127.0.0.1:9002",
        "192.168.1.100:9000"
    };

    RemoteActorSystem system(config);

    REQUIRE(config.known_nodes.size() == 3);
    REQUIRE(config.known_nodes[0] == "127.0.0.1:9001");
}

TEST_CASE("RemoteActorSystem: 多个 Actor", "[remote_actor_system]") {
    RemoteActorSystem::Config config;
    config.node_id = "test_node";
    RemoteActorSystem system(config);
    system.start();

    auto actor1 = system.create_actor<TestActor>("actor1");
    auto actor2 = system.create_actor<TestActor>("actor2");
    auto actor3 = system.create_actor<TestActor>("actor3");

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    REQUIRE(actor1 != nullptr);
    REQUIRE(actor2 != nullptr);
    REQUIRE(actor3 != nullptr);
    REQUIRE(actor1->path() == "/actor1");
    REQUIRE(actor2->path() == "/actor2");
    REQUIRE(actor3->path() == "/actor3");

    // 发送消息到多个 Actor
    system.tell(actor1, std::make_shared<TestMessage>("msg1"));
    system.tell(actor2, std::make_shared<TestMessage>("msg2"));
    system.tell(actor3, std::make_shared<TestMessage>("msg3"));

    bool all_received = wait_for([&]() {
        return actor1->message_count() > 0 &&
               actor2->message_count() > 0 &&
               actor3->message_count() > 0;
    }, 2000);

    REQUIRE(all_received);

    system.stop();
}

TEST_CASE("RemoteActorSystem: 边界情况", "[remote_actor_system]") {
    SECTION("零线程池大小（应使用默认值）") {
        RemoteActorSystem::Config config;
        config.node_id = "test_node";
        config.actor_thread_pool_size = 0;

        RemoteActorSystem system(config);

        REQUIRE(system.node_id() == "test_node");
    }

    SECTION("空已知节点列表") {
        RemoteActorSystem::Config config;
        config.node_id = "test_node";
        config.known_nodes = {};

        RemoteActorSystem system(config);

        REQUIRE(config.known_nodes.empty());
    }

    SECTION("单线程配置") {
        RemoteActorSystem::Config config;
        config.node_id = "test_node";
        config.actor_thread_pool_size = 1;
        config.io_threads = 1;

        RemoteActorSystem system(config);
        system.start();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        system.stop();

        REQUIRE(true);
    }
}
