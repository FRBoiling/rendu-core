#include <catch2/catch_test_macros.hpp>
#include "core/actor/message.h"
#include <atomic>
#include <thread>
#include <chrono>


using namespace Rendu;

// ============================================================================
// 测试消息类型
// ============================================================================

class CounterMessage : public Rendu::Message {
public:
    explicit CounterMessage(int delta = 1)
        : Rendu::Message(0), delta_(delta) {}

    const char* get_type() const override { return "CounterMessage"; }
    int delta() const { return delta_; }

    // 序列化支持
    std::string serialize() const override {
        return "CounterMessage|" + std::to_string(request_id_) + "|" + std::to_string(delta_);
    }

    void deserialize_data(const std::string& data) override {
        try {
            delta_ = std::stoi(data);
        } catch (...) {
            delta_ = 0;
        }
    }

private:
    int delta_;
};

// 注册消息类型（用于反序列化）
static Rendu::MessageRegistrar<CounterMessage> g_CounterMessage_registrar("CounterMessage");

class PingMessage : public Rendu::Message {
public:
    PingMessage() : Rendu::Message(0) {}
    const char* get_type() const override { return "PingMessage"; }

    // 序列化支持
    std::string serialize() const override {
        return "PingMessage|" + std::to_string(request_id_) + "|ping";
    }

    void deserialize_data(const std::string& data) override {
        // 空消息无数据需要反序列化
    }
};

static Rendu::MessageRegistrar<PingMessage> g_PingMessage_registrar("PingMessage");

// Ask 模式请求消息
class RequestMessage : public Rendu::Message {
public:
    RequestMessage() : Rendu::Message(0) {}
    const char* get_type() const override { return "RequestMessage"; }

    // 序列化支持
    std::string serialize() const override {
        return "RequestMessage|" + std::to_string(request_id_) + "|request";
    }

    void deserialize_data(const std::string& data) override {
        // 空消息无数据需要反序列化
    }
};

static Rendu::MessageRegistrar<RequestMessage> g_RequestMessage_registrar("RequestMessage");

// Ask 模式响应消息
class AskResponseMsg : public Rendu::Message {
public:
    AskResponseMsg() : Rendu::Message(0), value_(0) {}
    explicit AskResponseMsg(int value) : Rendu::Message(0), value_(value) {}
    const char* get_type() const override { return "AskResponseMsg"; }
    int value() const { return value_; }

    // 序列化支持
    std::string serialize() const override {
        return "AskResponseMsg|" + std::to_string(request_id_) + "|" + std::to_string(value_);
    }

    void deserialize_data(const std::string& data) override {
        try {
            value_ = std::stoi(data);
        } catch (...) {
            value_ = 0;
        }
    }

private:
    int value_;
};

static Rendu::MessageRegistrar<AskResponseMsg> g_AskResponseMsg_registrar("AskResponseMsg");

// ============================================================================
// Message 基础功能测试
// ============================================================================

TEST_CASE("Message 基础功能", "[message]") {
    SECTION("创建消息") {
        auto msg = std::make_shared<CounterMessage>(42);
        REQUIRE(msg != nullptr);
        REQUIRE(msg->get_type() == std::string("CounterMessage"));
    }

    SECTION("请求 ID 设置和获取") {
        auto msg = std::make_shared<CounterMessage>(10);
        REQUIRE(msg->request_id() == 0);

        msg->set_request_id(123);
        REQUIRE(msg->request_id() == 123);
    }

    SECTION("消息数据访问") {
        auto msg = std::make_shared<CounterMessage>(99);
        auto* counter_msg = dynamic_cast<CounterMessage*>(msg.get());
        REQUIRE(counter_msg != nullptr);
        REQUIRE(counter_msg->delta() == 99);
    }
}

// ============================================================================
// Message 序列化和反序列化测试
// ============================================================================

TEST_CASE("Message 序列化和反序列化", "[message][serialize]") {
    SECTION("CounterMessage 序列化反序列化") {
        auto msg1 = std::make_shared<CounterMessage>(42);
        msg1->set_request_id(123);

        std::string serialized = msg1->serialize();

        auto msg2 = Rendu::MessageRegistry::deserialize(serialized);
        REQUIRE(msg2 != nullptr);
        REQUIRE(msg2->get_type() == std::string("CounterMessage"));
        REQUIRE(msg2->request_id() == 123);

        auto* counter_msg = dynamic_cast<CounterMessage*>(msg2.get());
        REQUIRE(counter_msg != nullptr);
        REQUIRE(counter_msg->delta() == 42);
    }

    SECTION("PingMessage 序列化反序列化") {
        auto msg1 = std::make_shared<PingMessage>();
        msg1->set_request_id(456);

        std::string serialized = msg1->serialize();

        auto msg2 = Rendu::MessageRegistry::deserialize(serialized);
        REQUIRE(msg2 != nullptr);
        REQUIRE(msg2->get_type() == std::string("PingMessage"));
        REQUIRE(msg2->request_id() == 456);

        auto* ping_msg = dynamic_cast<PingMessage*>(msg2.get());
        REQUIRE(ping_msg != nullptr);
    }

    SECTION("AskResponseMsg 序列化反序列化") {
        auto msg1 = std::make_shared<AskResponseMsg>(100);
        msg1->set_request_id(789);

        std::string serialized = msg1->serialize();

        auto msg2 = Rendu::MessageRegistry::deserialize(serialized);
        REQUIRE(msg2 != nullptr);
        REQUIRE(msg2->get_type() == std::string("AskResponseMsg"));
        REQUIRE(msg2->request_id() == 789);

        auto* response_msg = dynamic_cast<AskResponseMsg*>(msg2.get());
        REQUIRE(response_msg != nullptr);
        REQUIRE(response_msg->value() == 100);
    }

    SECTION("EmptyMessage 序列化反序列化") {
        auto msg1 = std::make_shared<EmptyMessage>();
        msg1->set_request_id(999);

        std::string serialized = msg1->serialize();

        auto msg2 = Rendu::MessageRegistry::deserialize(serialized);
        REQUIRE(msg2 != nullptr);
        REQUIRE(msg2->get_type() == std::string("EmptyMessage"));
        REQUIRE(msg2->request_id() == 999);
    }
}

TEST_CASE("Message 序列化格式验证", "[message][serialize]") {
    SECTION("验证序列化格式") {
        auto msg = std::make_shared<CounterMessage>(42);
        msg->set_request_id(123);

        std::string serialized = msg->serialize();

        // 格式应为: type|request_id|data
        REQUIRE(serialized.find("CounterMessage") == 0);
        REQUIRE(serialized.find("|123|") != std::string::npos);
        REQUIRE(serialized.find("42") != std::string::npos);
    }

    SECTION("多次序列化一致性") {
        auto msg1 = std::make_shared<CounterMessage>(100);
        msg1->set_request_id(1);

        auto msg2 = std::make_shared<CounterMessage>(100);
        msg2->set_request_id(1);

        REQUIRE(msg1->serialize() == msg2->serialize());
    }
}

TEST_CASE("Message 反序列化无效输入", "[message][serialize]") {
    SECTION("空字符串反序列化") {
        auto msg = Rendu::MessageRegistry::deserialize("");
        REQUIRE(msg == nullptr);
    }

    SECTION("无效格式反序列化") {
        auto msg = Rendu::MessageRegistry::deserialize("invalid");
        REQUIRE(msg == nullptr);
    }

    SECTION("缺少分隔符反序列化") {
        auto msg = Rendu::MessageRegistry::deserialize("CounterMessage123");
        REQUIRE(msg == nullptr);
    }

    SECTION("未知类型反序列化") {
        auto msg = Rendu::MessageRegistry::deserialize("UnknownType|123|data");
        REQUIRE(msg == nullptr);
    }

    SECTION("无效的请求 ID 反序列化") {
        auto msg = Rendu::MessageRegistry::deserialize("CounterMessage|abc|10");
        REQUIRE(msg == nullptr);
    }

    SECTION("部分缺失的数据反序列化") {
        auto msg = Rendu::MessageRegistry::deserialize("CounterMessage|123|");
        // 应该成功，但数据部分为空
        REQUIRE(msg != nullptr);
        auto* counter_msg = dynamic_cast<CounterMessage*>(msg.get());
        REQUIRE(counter_msg != nullptr);
        REQUIRE(counter_msg->delta() == 0); // 空数据解析为 0
    }
}

// ============================================================================
// MessageRegistry 功能测试
// ============================================================================

TEST_CASE("MessageRegistry 创建和检查", "[message][registry]") {
    SECTION("检查已注册的消息类型") {
        REQUIRE(Rendu::MessageRegistry::is_registered("CounterMessage"));
        REQUIRE(Rendu::MessageRegistry::is_registered("PingMessage"));
        REQUIRE(Rendu::MessageRegistry::is_registered("RequestMessage"));
        REQUIRE(Rendu::MessageRegistry::is_registered("AskResponseMsg"));
        REQUIRE(Rendu::MessageRegistry::is_registered("EmptyMessage"));
    }

    SECTION("检查未注册的消息类型") {
        REQUIRE_FALSE(Rendu::MessageRegistry::is_registered("UnknownMessage"));
        REQUIRE_FALSE(Rendu::MessageRegistry::is_registered(""));
    }

    SECTION("创建已注册的消息") {
        auto counter_msg = Rendu::MessageRegistry::create("CounterMessage");
        REQUIRE(counter_msg != nullptr);
        REQUIRE(counter_msg->get_type() == std::string("CounterMessage"));

        auto ping_msg = Rendu::MessageRegistry::create("PingMessage");
        REQUIRE(ping_msg != nullptr);
        REQUIRE(ping_msg->get_type() == std::string("PingMessage"));

        auto request_msg = Rendu::MessageRegistry::create("RequestMessage");
        REQUIRE(request_msg != nullptr);
        REQUIRE(request_msg->get_type() == std::string("RequestMessage"));

        auto response_msg = Rendu::MessageRegistry::create("AskResponseMsg");
        REQUIRE(response_msg != nullptr);
        REQUIRE(response_msg->get_type() == std::string("AskResponseMsg"));

        auto empty_msg = Rendu::MessageRegistry::create("EmptyMessage");
        REQUIRE(empty_msg != nullptr);
        REQUIRE(empty_msg->get_type() == std::string("EmptyMessage"));
    }

    SECTION("创建未注册的消息") {
        auto msg = Rendu::MessageRegistry::create("UnknownMessage");
        REQUIRE(msg == nullptr);

        auto msg2 = Rendu::MessageRegistry::create("");
        REQUIRE(msg2 == nullptr);
    }
}

TEST_CASE("MessageRegistry 批量操作", "[message][registry]") {
    SECTION("批量创建和反序列化") {
        std::vector<std::string> types = {
            "CounterMessage",
            "PingMessage",
            "RequestMessage",
            "AskResponseMsg",
            "EmptyMessage"
        };

        for (const auto& type : types) {
            auto msg = Rendu::MessageRegistry::create(type);
            REQUIRE(msg != nullptr);
            REQUIRE(msg->get_type() == type);

            msg->set_request_id(100);
            std::string serialized = msg->serialize();

            auto deserialized = Rendu::MessageRegistry::deserialize(serialized);
            REQUIRE(deserialized != nullptr);
            REQUIRE(deserialized->get_type() == type);
            REQUIRE(deserialized->request_id() == 100);
        }
    }
}

// ============================================================================
// Message 派生类测试
// ============================================================================

TEST_CASE("Message 派生类功能", "[message][derived]") {
    SECTION("CounterMessage 默认值") {
        auto msg = std::make_shared<CounterMessage>();
        REQUIRE(msg->delta() == 1);
    }

    SECTION("CounterMessage 自定义值") {
        auto msg = std::make_shared<CounterMessage>(100);
        REQUIRE(msg->delta() == 100);
    }

    SECTION("AskResponseMsg 默认值") {
        auto msg = std::make_shared<AskResponseMsg>();
        REQUIRE(msg->value() == 0);
    }

    SECTION("AskResponseMsg 自定义值") {
        auto msg = std::make_shared<AskResponseMsg>(999);
        REQUIRE(msg->value() == 999);
    }

    SECTION("空消息类型") {
        auto msg = std::make_shared<PingMessage>();
        auto msg2 = std::make_shared<RequestMessage>();
        REQUIRE(msg->get_type() != msg2->get_type());
    }
}

// ============================================================================
// Message 线程安全测试
// ============================================================================

TEST_CASE("Message 多线程操作", "[message][concurrent]") {
    SECTION("并发序列化") {
        const int thread_count = 10;
        const int operations_per_thread = 100;

        std::vector<std::thread> threads;
        std::atomic<int> success_count(0);

        for (int t = 0; t < thread_count; ++t) {
            threads.emplace_back([&success_count, operations_per_thread]() {
                for (int i = 0; i < operations_per_thread; ++i) {
                    auto msg = std::make_shared<CounterMessage>(i);
                    msg->set_request_id(i);
                    std::string serialized = msg->serialize();

                    auto deserialized = Rendu::MessageRegistry::deserialize(serialized);
                    if (deserialized != nullptr &&
                        deserialized->get_type() == std::string("CounterMessage") &&
                        deserialized->request_id() == i) {
                        success_count++;
                    }
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        REQUIRE(success_count == thread_count * operations_per_thread);
    }

    SECTION("并发消息创建") {
        const int thread_count = 10;
        const int operations_per_thread = 100;

        std::vector<std::thread> threads;
        std::atomic<int> success_count(0);

        for (int t = 0; t < thread_count; ++t) {
            threads.emplace_back([&success_count, operations_per_thread]() {
                for (int i = 0; i < operations_per_thread; ++i) {
                    auto msg = Rendu::MessageRegistry::create("CounterMessage");
                    if (msg != nullptr && msg->get_type() == std::string("CounterMessage")) {
                        success_count++;
                    }
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        REQUIRE(success_count == thread_count * operations_per_thread);
    }
}

// ============================================================================
// Message 边界条件测试
// ============================================================================

TEST_CASE("Message 边界条件", "[message][boundary]") {
    SECTION("零请求 ID") {
        auto msg = std::make_shared<CounterMessage>(10);
        msg->set_request_id(0);

        std::string serialized = msg->serialize();
        auto deserialized = Rendu::MessageRegistry::deserialize(serialized);

        REQUIRE(deserialized != nullptr);
        REQUIRE(deserialized->request_id() == 0);
    }

    SECTION("极大请求 ID") {
        uint64_t max_id = std::numeric_limits<uint64_t>::max();
        auto msg = std::make_shared<CounterMessage>(10);
        msg->set_request_id(max_id);

        std::string serialized = msg->serialize();
        auto deserialized = Rendu::MessageRegistry::deserialize(serialized);

        REQUIRE(deserialized != nullptr);
        REQUIRE(deserialized->request_id() == max_id);
    }

    SECTION("空数据反序列化处理") {
        auto msg1 = std::make_shared<CounterMessage>(100);
        msg1->set_request_id(123);

        // 手动构造只有类型和请求 ID 的序列化字符串
        std::string serialized = "CounterMessage|123|";
        auto msg2 = Rendu::MessageRegistry::deserialize(serialized);

        REQUIRE(msg2 != nullptr);
        REQUIRE(msg2->request_id() == 123);
        auto* counter_msg = dynamic_cast<CounterMessage*>(msg2.get());
        REQUIRE(counter_msg != nullptr);
        REQUIRE(counter_msg->delta() == 0); // 空数据应该解析为默认值
    }

    SECTION("特殊字符数据") {
        auto msg1 = std::make_shared<CounterMessage>(42);
        msg1->set_request_id(123);

        std::string serialized = msg1->serialize();
        auto msg2 = Rendu::MessageRegistry::deserialize(serialized);

        REQUIRE(msg2 != nullptr);
        auto* counter_msg = dynamic_cast<CounterMessage*>(msg2.get());
        REQUIRE(counter_msg != nullptr);
        REQUIRE(counter_msg->delta() == 42);
    }
}
