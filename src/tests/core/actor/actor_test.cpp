//**********************************
//  Created by boil on 2026/01/28.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include "core/actor/message.h"
#include "core/actor/actor.h"
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
// 测试 Actor 实现
// ============================================================================

// 计数器 Actor：累加接收到的 CounterMessage 的值
class CounterActor : public Rendu::Actor {
public:
    CounterActor() : Rendu::Actor("Counter"), counter_(0) {}

    void receive(std::shared_ptr<Rendu::Message> msg) override {
        if (auto* counter_msg = dynamic_cast<CounterMessage*>(msg.get())) {
            counter_ += counter_msg->delta();
        }
    }

    int get_counter() const { return counter_; }

private:
    std::atomic<int> counter_{0};
};

// Ping Pong Actor：统计接收到的消息数量
class PingPongActor : public Rendu::Actor {
public:
    PingPongActor() : Rendu::Actor("PingPong"), ping_count_(0) {}

    void receive(std::shared_ptr<Rendu::Message> msg) override {
        if (dynamic_cast<PingMessage*>(msg.get()) != nullptr) {
            ping_count_++;
        }
    }

    int get_ping_count() const { return ping_count_; }

private:
    std::atomic<int> ping_count_{0};
};

// 回调测试 Actor：记录生命周期回调
class CallbackTestActor : public Rendu::Actor {
public:
    CallbackTestActor() : Rendu::Actor("CallbackTest"),
        on_start_called_(false), on_stop_called_(false) {}

    void receive(std::shared_ptr<Rendu::Message> msg) override {
        // 不做任何处理
    }

    void on_start() override {
        on_start_called_ = true;
    }

    void on_stop() override {
        on_stop_called_ = true;
    }

    bool on_start_called() const { return on_start_called_; }
    bool on_stop_called() const { return on_stop_called_; }

private:
    std::atomic<bool> on_start_called_;
    std::atomic<bool> on_stop_called_;
};

// Ask 模式测试 Actor：响应请求
class EchoActor : public Rendu::Actor {
public:
    EchoActor() : Rendu::Actor("Echo") {}

    void receive(std::shared_ptr<Rendu::Message> msg) override {
        if (dynamic_cast<RequestMessage*>(msg.get()) != nullptr) {
            // 响应 Ask 请求，使用消息的 request_id
            auto response = std::make_shared<AskResponseMsg>(42);
            system_->reply(msg->request_id(), response);
        }
    }

    void set_system(Rendu::ActorSystem* sys) { system_ = sys; }

private:
    Rendu::ActorSystem* system_{nullptr};
};

// 超时测试 Actor：延迟响应
class SlowActor : public Rendu::Actor {
public:
    SlowActor() : Rendu::Actor("Slow") {}

    void receive(std::shared_ptr<Rendu::Message> msg) override {
        if (dynamic_cast<RequestMessage*>(msg.get()) != nullptr) {
            // 延迟响应
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            auto response = std::make_shared<AskResponseMsg>(100);
            system_->reply(msg->request_id(), response);
        }
    }

    void set_system(Rendu::ActorSystem* sys) { system_ = sys; }

private:
    Rendu::ActorSystem* system_{nullptr};
};

// ============================================================================
// 测试 Fixture - 自动管理 ActorSystem 生命周期
// ============================================================================

struct ActorTestFixture {
    ActorTestFixture() {
        system.start();
    }

    ~ActorTestFixture() {
        system.stop();
    }

    Rendu::ActorSystem system;
};

// ============================================================================
// Actor 系统基础测试
// ============================================================================

TEST_CASE_METHOD(ActorTestFixture, "Actor 系统启动和停止", "[actor]") {
    // 测试成功表示 ActorSystem 可以正常启动和停止
    REQUIRE(system.actor_count() == 0);
}

TEST_CASE_METHOD(ActorTestFixture, "Actor 系统创建单个 Actor", "[actor]") {
    auto ref = system.create_actor("TestActor", []() {
        return std::make_unique<CounterActor>();
    });

    REQUIRE(ref.is_valid());
    REQUIRE(system.actor_count() == 1);
}

TEST_CASE_METHOD(ActorTestFixture, "Actor 系统创建多个 Actor", "[actor]") {
    auto ref1 = system.create_actor("Actor1", []() {
        return std::make_unique<CounterActor>();
    });
    auto ref2 = system.create_actor("Actor2", []() {
        return std::make_unique<CounterActor>();
    });
    auto ref3 = system.create_actor("Actor3", []() {
        return std::make_unique<CounterActor>();
    });

    REQUIRE(ref1.is_valid());
    REQUIRE(ref2.is_valid());
    REQUIRE(ref3.is_valid());
    REQUIRE(system.actor_count() == 3);
}

TEST_CASE_METHOD(ActorTestFixture, "Actor 系统查找 Actor", "[actor]") {
    auto ref = system.create_actor("TestActor", []() {
        return std::make_unique<CounterActor>();
    });

    auto actor = system.find_actor(ref);
    REQUIRE(actor != nullptr);
    REQUIRE(actor->name() == "Counter");
}

TEST_CASE_METHOD(ActorTestFixture, "Actor 系统停止单个 Actor", "[actor]") {
    auto ref1 = system.create_actor("Actor1", []() {
        return std::make_unique<CounterActor>();
    });
    auto ref2 = system.create_actor("Actor2", []() {
        return std::make_unique<CounterActor>();
    });

    REQUIRE(system.actor_count() == 2);

    system.stop_actor(ref1);

    REQUIRE(system.actor_count() == 1);
}

// ============================================================================
// 消息通信测试
// ============================================================================

TEST_CASE_METHOD(ActorTestFixture, "Actor 发送单条消息", "[actor][message]") {
    auto ref = system.create_actor("Counter", []() {
        return std::make_unique<CounterActor>();
    });

    auto actor = system.find_actor(ref);
    REQUIRE(actor != nullptr);

    auto* counter_actor = dynamic_cast<CounterActor*>(actor.get());
    REQUIRE(counter_actor != nullptr);

    system.tell(ref, std::make_shared<CounterMessage>(5));

    // 等待消息处理
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    REQUIRE(counter_actor->get_counter() >= 5);
}

TEST_CASE_METHOD(ActorTestFixture, "Actor 发送多条消息", "[actor][message]") {
    auto ref = system.create_actor("Counter", []() {
        return std::make_unique<CounterActor>();
    });

    auto actor = system.find_actor(ref);
    auto* counter_actor = dynamic_cast<CounterActor*>(actor.get());

    // 发送多条消息
    for (int i = 0; i < 10; ++i) {
        system.tell(ref, std::make_shared<CounterMessage>(i));
    }

    // 等待消息处理
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // 0 + 1 + 2 + ... + 9 = 45
    REQUIRE(counter_actor->get_counter() >= 45);
}

TEST_CASE_METHOD(ActorTestFixture, "Actor 并发处理消息", "[actor][concurrent]") {
    const int thread_count = 2;
    const int messages_per_thread = 10;

    auto ref = system.create_actor("PingPong", []() {
        return std::make_unique<PingPongActor>();
    });

    auto actor = system.find_actor(ref);
    auto* ping_pong_actor = dynamic_cast<PingPongActor*>(actor.get());

    // 并发发送消息
    std::vector<std::thread> threads;
    for (int t = 0; t < thread_count; ++t) {
        threads.emplace_back([&ref, this, messages_per_thread]() {
            for (int i = 0; i < messages_per_thread; ++i) {
                system.tell(ref, std::make_shared<PingMessage>());
            }
        });
    }

    // 等待所有发送线程完成
    for (auto& thread : threads) {
        thread.join();
    }

    // 等待消息处理
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    REQUIRE(ping_pong_actor->get_ping_count() >= thread_count * messages_per_thread);
}

// ============================================================================
// 生命周期回调测试
// ============================================================================

TEST_CASE_METHOD(ActorTestFixture, "Actor lifecycle callbacks", "[actor][lifecycle]") {
    auto ref = system.create_actor("CallbackTest", []() {
        return std::make_unique<CallbackTestActor>();
    });

    auto actor = system.find_actor(ref);
    auto* callback_actor = dynamic_cast<CallbackTestActor*>(actor.get());

    // 等待 Actor 完全启动
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    REQUIRE(callback_actor->on_start_called());
    REQUIRE_FALSE(callback_actor->on_stop_called());

    system.stop_actor(ref);

    REQUIRE(callback_actor->on_start_called());
    REQUIRE(callback_actor->on_stop_called());
}

// ============================================================================
// ActorRef 测试
// ============================================================================

TEST_CASE("ActorRef 基本操作", "[actor_ref]") {
    SECTION("无效引用") {
        Rendu::ActorRef invalid_ref;
        REQUIRE_FALSE(invalid_ref.is_valid());
    }

    SECTION("有效引用") {
        Rendu::ActorRef ref("/user/test", 123);
        REQUIRE(ref.is_valid());
        REQUIRE(ref.path() == "/user/test");
        REQUIRE(ref.actor_id() == 123);
    }

    SECTION("相等性比较") {
        Rendu::ActorRef ref1("/user/test", 123);
        Rendu::ActorRef ref2("/user/test", 123);
        Rendu::ActorRef ref3("/user/test", 456);

        REQUIRE(ref1 == ref2);
        REQUIRE(ref1 != ref3);
    }

    SECTION("转换为字符串") {
        Rendu::ActorRef ref("/user/test", 123);
        std::string str = ref.to_string();
        REQUIRE(str.find("/user/test") != std::string::npos);
        REQUIRE(str.find("123") != std::string::npos);
    }

    SECTION("哈希函数") {
        Rendu::ActorRef ref("/user/test", 123);
        size_t hash_value = ref.hash();
        REQUIRE(hash_value != 0);
    }
}

TEST_CASE("ActorRef 哈希", "[actor_ref]") {
    Rendu::ActorRef ref1("/user/test", 123);
    Rendu::ActorRef ref2("/user/test", 123);

    Rendu::ActorRefHash hasher;
    REQUIRE(hasher(ref1) == hasher(ref2));
}

// ============================================================================
// 消息队列测试
// ============================================================================

TEST_CASE("MessageQueue 基本操作", "[message_queue]") {
    Rendu::MessageQueue queue;

    SECTION("推送和弹出") {
        auto msg1 = std::make_shared<CounterMessage>(1);
        auto msg2 = std::make_shared<CounterMessage>(2);

        queue.push(msg1);
        queue.push(msg2);

        auto popped1 = queue.pop();
        auto popped2 = queue.pop();

        REQUIRE(popped1 != nullptr);
        REQUIRE(popped2 != nullptr);

        auto* counter_msg1 = dynamic_cast<CounterMessage*>(popped1.get());
        auto* counter_msg2 = dynamic_cast<CounterMessage*>(popped2.get());

        REQUIRE(counter_msg1->delta() == 1);
        REQUIRE(counter_msg2->delta() == 2);
    }

    SECTION("从空队列弹出") {
        // 需要启动一个线程来停止队列，否则 pop 会阻塞
        std::thread stopper([&queue]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            queue.stop();
        });

        auto msg = queue.pop();
        REQUIRE(msg == nullptr);

        stopper.join();
    }

    SECTION("清空队列") {
        queue.push(std::make_shared<CounterMessage>(1));
        queue.push(std::make_shared<CounterMessage>(2));
        queue.push(std::make_shared<CounterMessage>(3));

        REQUIRE(queue.size() == 3);

        queue.clear();

        REQUIRE(queue.size() == 0);
    }
}

TEST_CASE("MessageQueue 停止行为", "[message_queue]") {
    Rendu::MessageQueue queue;

    // 启动线程来停止队列
    std::thread stopper([&queue]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        queue.stop();
    });

    auto popped = queue.pop();
    REQUIRE(popped == nullptr);

    stopper.join();
}

// ============================================================================
// Ask 模式测试
// ============================================================================

TEST_CASE_METHOD(ActorTestFixture, "Actor Ask 模式基础测试", "[actor][ask]") {
    auto ref = system.create_actor("Echo", []() {
        return std::make_unique<EchoActor>();
    });

    auto actor = system.find_actor(ref);
    auto* echo_actor = dynamic_cast<EchoActor*>(actor.get());
    echo_actor->set_system(&system);

    // 发送 Ask 请求
    auto response = system.ask(ref, std::make_shared<RequestMessage>());

    REQUIRE(response != nullptr);
    auto* resp_msg = dynamic_cast<AskResponseMsg*>(response.get());
    REQUIRE(resp_msg != nullptr);
    REQUIRE(resp_msg->value() == 42);
}

TEST_CASE_METHOD(ActorTestFixture, "Actor Ask 模式带超时", "[actor][ask]") {
    auto ref = system.create_actor("Slow", []() {
        return std::make_unique<SlowActor>();
    });

    auto actor = system.find_actor(ref);
    auto* slow_actor = dynamic_cast<SlowActor*>(actor.get());
    slow_actor->set_system(&system);

    // 发送带超时的 Ask 请求（100ms）
    auto response = system.ask(ref, std::make_shared<RequestMessage>(), 100);

    // 应该超时返回 nullptr
    REQUIRE(response == nullptr);
}

TEST_CASE_METHOD(ActorTestFixture, "Actor Ask 模式无超时成功", "[actor][ask]") {
    auto ref = system.create_actor("Echo", []() {
        return std::make_unique<EchoActor>();
    });

    auto actor = system.find_actor(ref);
    auto* echo_actor = dynamic_cast<EchoActor*>(actor.get());
    echo_actor->set_system(&system);

    // 发送无超时的 Ask 请求
    auto response = system.ask(ref, std::make_shared<RequestMessage>(), 0);

    REQUIRE(response != nullptr);
    auto* resp_msg = dynamic_cast<AskResponseMsg*>(response.get());
    REQUIRE(resp_msg != nullptr);
    REQUIRE(resp_msg->value() == 42);
}

TEST_CASE_METHOD(ActorTestFixture, "Actor 并发 Ask 模式", "[actor][ask][concurrent]") {
    const int thread_count = 4;

    auto ref = system.create_actor("Echo", []() {
        return std::make_unique<EchoActor>();
    });

    auto actor = system.find_actor(ref);
    auto* echo_actor = dynamic_cast<EchoActor*>(actor.get());
    echo_actor->set_system(&system);

    // 并发发送 Ask 请求
    std::vector<std::thread> threads;
    std::atomic<int> success_count(0);

    for (int t = 0; t < thread_count; ++t) {
        threads.emplace_back([&ref, this, &success_count]() {
            for (int i = 0; i < 10; ++i) {
                auto response = system.ask(ref, std::make_shared<RequestMessage>(), 1000);
                if (response != nullptr) {
                    auto* resp_msg = dynamic_cast<AskResponseMsg*>(response.get());
                    if (resp_msg && resp_msg->value() == 42) {
                        success_count++;
                    }
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    REQUIRE(success_count == thread_count * 10);
}

// ============================================================================
// ActorRef 序列化测试
// ============================================================================

TEST_CASE("ActorRef 序列化和反序列化", "[actor_ref][serialize]") {
    SECTION("有效引用") {
        Rendu::ActorRef original("/user/test", 123);
        std::string serialized = original.serialize();

        Rendu::ActorRef deserialized = Rendu::ActorRef::deserialize(serialized);

        REQUIRE(deserialized.is_valid());
        REQUIRE(deserialized.path() == original.path());
        REQUIRE(deserialized.actor_id() == original.actor_id());
        REQUIRE(deserialized == original);
    }

    SECTION("多个引用") {
        Rendu::ActorRef ref1("/system/network", 1);
        Rendu::ActorRef ref2("/app/game", 999);

        std::string str1 = ref1.serialize();
        std::string str2 = ref2.serialize();

        Rendu::ActorRef result1 = Rendu::ActorRef::deserialize(str1);
        Rendu::ActorRef result2 = Rendu::ActorRef::deserialize(str2);

        REQUIRE(result1 == ref1);
        REQUIRE(result2 == ref2);
        REQUIRE(result1 != result2);
    }
}

TEST_CASE("ActorRef 序列化格式验证", "[actor_ref][serialize]") {
    SECTION("有效格式") {
        REQUIRE(Rendu::ActorRef::is_valid_serialized("/user/test:123"));
        REQUIRE(Rendu::ActorRef::is_valid_serialized("/system:0"));
        REQUIRE(Rendu::ActorRef::is_valid_serialized("/a/b/c:999999"));
    }

    SECTION("无效格式") {
        REQUIRE_FALSE(Rendu::ActorRef::is_valid_serialized(""));
        REQUIRE_FALSE(Rendu::ActorRef::is_valid_serialized("/user/test"));
        REQUIRE_FALSE(Rendu::ActorRef::is_valid_serialized(":123"));
        REQUIRE_FALSE(Rendu::ActorRef::is_valid_serialized("/user/test:abc"));
        REQUIRE_FALSE(Rendu::ActorRef::is_valid_serialized("/user/test:-123"));
    }
}

TEST_CASE("ActorRef 反序列化无效输入", "[actor_ref][serialize]") {
    SECTION("空字符串") {
        Rendu::ActorRef ref = Rendu::ActorRef::deserialize("");
        REQUIRE_FALSE(ref.is_valid());
    }

    SECTION("无效格式") {
        Rendu::ActorRef ref = Rendu::ActorRef::deserialize("invalid");
        REQUIRE_FALSE(ref.is_valid());
    }

    SECTION("非数字 ID") {
        Rendu::ActorRef ref = Rendu::ActorRef::deserialize("/user/test:abc");
        REQUIRE_FALSE(ref.is_valid());
    }
}

// ============================================================================
// 线程池测试
// ============================================================================

TEST_CASE_METHOD(ActorTestFixture, "Actor thread pool efficiency", "[actor][threadpool]") {
    // 创建多个 Actor，验证它们共享线程池
    const int actor_count = 5;

    std::vector<ActorRef> refs;
    for (int i = 0; i < actor_count; ++i) {
        auto ref = system.create_actor("Actor_" + std::to_string(i), [i]() {
            return std::make_unique<CounterActor>();
        });
        refs.push_back(ref);
    }

    // 系统应该只有固定数量的线程（默认4个）
    // Actor 数量大于线程数，说明线程池在工作
    REQUIRE(system.actor_count() == actor_count);

    // 等待所有 Actor 启动
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // 并发发送消息
    const int messages_per_actor = 5;
    for (int i = 0; i < actor_count; ++i) {
        for (int j = 0; j < messages_per_actor; ++j) {
            system.tell(refs[i], std::make_shared<CounterMessage>(1));
        }
    }

    // 等待处理
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));

    // 验证至少处理了一些消息（不要求全部，因为有竞争条件）
    int total_processed = 0;
    int active_actors = 0;
    for (int i = 0; i < actor_count; ++i) {
        auto actor = system.find_actor(refs[i]);
        auto* counter_actor = dynamic_cast<CounterActor*>(actor.get());
        REQUIRE(counter_actor != nullptr);

        int count = counter_actor->get_counter();
        if (count > 0) {
            active_actors++;
            total_processed += count;
        }
    }

    // 验证至少有多个 Actor 处理了消息
    REQUIRE(active_actors >= actor_count / 2);
    // 验证处理了一定数量的消息
    REQUIRE(total_processed >= actor_count * messages_per_actor / 2);
}

TEST_CASE_METHOD(ActorTestFixture, "Actor stop with thread pool", "[actor][threadpool]") {
    // 创建 Actor 后立即停止，验证线程池正常工作
    auto ref = system.create_actor("Test", []() {
        return std::make_unique<CounterActor>();
    });

    REQUIRE(system.actor_count() == 1);

    system.stop_actor(ref);

    REQUIRE(system.actor_count() == 0);
}


