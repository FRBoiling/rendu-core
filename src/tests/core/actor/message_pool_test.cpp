#include <catch2/catch_test_macros.hpp>
#include "core/actor/message_pool.h"
#include "core/actor/message.h"

using namespace Rendu;

struct TestMessage : public Message {
    int value{0};
    std::string text{""};

    TestMessage() = default;
    explicit TestMessage(int v) : value(v) {}
    TestMessage(int v, const std::string& t) : value(v), text(t) {}

    const char* get_type() const override { return "TestMessage"; }
    std::string serialize() const override { return ""; }
    void deserialize_data(const std::string&) override {}
};

TEST_CASE("MessagePool - 分配新消息", "[actor][message_pool]") {
    auto& pool = MessagePool::instance();
    pool.clear();

    auto msg = pool.allocate<TestMessage>(42, "hello");
    REQUIRE(msg != nullptr);

    auto test_msg = static_cast<TestMessage*>(msg);
    REQUIRE(test_msg->value == 42);
    REQUIRE(test_msg->text == "hello");

    pool.deallocate(msg);
    REQUIRE(pool.free_size() == 1);
}

TEST_CASE("MessagePool - 复用消息", "[actor][message_pool]") {
    auto& pool = MessagePool::instance();
    pool.clear();

    auto msg1 = pool.allocate<TestMessage>(100);
    pool.deallocate(msg1);
    REQUIRE(pool.free_size() == 1);

    auto msg2 = pool.allocate<TestMessage>(200);
    REQUIRE(pool.free_size() == 0);

    auto test_msg2 = static_cast<TestMessage*>(msg2);
    REQUIRE(test_msg2->value == 200);

    pool.deallocate(msg2);
    REQUIRE(pool.free_size() == 1);
}

TEST_CASE("MessagePool - 多次分配释放", "[actor][message_pool]") {
    auto& pool = MessagePool::instance();
    pool.clear();

    std::vector<Message*> messages;
    for (int i = 0; i < 10; ++i) {
        messages.push_back(pool.allocate<TestMessage>(i));
    }

    REQUIRE(pool.free_size() == 0);

    for (auto msg : messages) {
        pool.deallocate(msg);
    }

    REQUIRE(pool.free_size() == 10);
}

TEST_CASE("MessagePool - 清理功能", "[actor][message_pool]") {
    auto& pool = MessagePool::instance();
    pool.clear();

    // 分配5个消息但不释放(这样它们不会进入 free_list)
    std::vector<Message*> messages;
    for (int i = 0; i < 5; ++i) {
        messages.push_back(pool.allocate<TestMessage>(i));
    }

    // free_size 应该是0,因为还没有释放任何消息
    REQUIRE(pool.free_size() == 0);

    // 释放所有消息
    for (auto msg : messages) {
        pool.deallocate(msg);
    }

    // 现在应该有5个消息在 free_list 中
    REQUIRE(pool.free_size() == 5);

    // 清理 free_list
    pool.clear();

    // free_size 应该是0
    REQUIRE(pool.free_size() == 0);
}

TEST_CASE("MessagePtr - 自动释放", "[actor][message_pool]") {
    auto& pool = MessagePool::instance();
    pool.clear();

    {
        MessagePtr<TestMessage> msg_ptr(
            static_cast<TestMessage*>(pool.allocate<TestMessage>(123))
        );

        REQUIRE(msg_ptr != nullptr);
        REQUIRE(msg_ptr->value == 123);
        REQUIRE(pool.free_size() == 0);
    }

    REQUIRE(pool.free_size() == 1);
}

TEST_CASE("MessagePtr - 移动语义", "[actor][message_pool]") {
    auto& pool = MessagePool::instance();
    pool.clear();

    MessagePtr<TestMessage> msg_ptr1(
        static_cast<TestMessage*>(pool.allocate<TestMessage>(456))
    );

    REQUIRE(msg_ptr1->value == 456);

    MessagePtr<TestMessage> msg_ptr2(std::move(msg_ptr1));
    REQUIRE(msg_ptr1 == nullptr);
    REQUIRE(msg_ptr2->value == 456);

    MessagePtr<TestMessage> msg_ptr3(
        static_cast<TestMessage*>(pool.allocate<TestMessage>(789))
    );

    msg_ptr3 = std::move(msg_ptr2);
    REQUIRE(msg_ptr2 == nullptr);
    REQUIRE(msg_ptr3->value == 456);
    REQUIRE(pool.free_size() == 1);
}

TEST_CASE("MessagePtr - bool转换", "[actor][message_pool]") {
    auto& pool = MessagePool::instance();
    pool.clear();

    MessagePtr<TestMessage> msg_ptr(
        static_cast<TestMessage*>(pool.allocate<TestMessage>(1))
    );

    REQUIRE(static_cast<bool>(msg_ptr) == true);

    MessagePtr<TestMessage> null_ptr(nullptr);
    REQUIRE(static_cast<bool>(null_ptr) == false);
}

// 多线程并发分配和释放测试
TEST_CASE("MessagePool - 多线程分配", "[actor][message_pool]") {
    auto& pool = MessagePool::instance();
    pool.clear();

    const int thread_count = 4;
    const int messages_per_thread = 50;

    std::vector<std::thread> threads;
    std::vector<std::vector<Message*>> thread_messages(thread_count);

    // 第一阶段:所有线程并发分配
    for (int t = 0; t < thread_count; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < messages_per_thread; ++i) {
                auto msg = pool.allocate<TestMessage>(t * 1000 + i);
                thread_messages[t].push_back(msg);
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // 此时所有消息都已分配,free_size 应该为 0
    REQUIRE(pool.free_size() == 0);

    // 第二阶段:所有线程并发释放
    threads.clear();
    for (int t = 0; t < thread_count; ++t) {
        threads.emplace_back([&, t]() {
            for (auto msg : thread_messages[t]) {
                pool.deallocate(msg);
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // 所有消息都应该回到 free list
    REQUIRE(pool.free_size() == static_cast<size_t>(thread_count * messages_per_thread));
}

// MessagePtr 多线程自动释放测试
TEST_CASE("MessagePtr - 多线程自动释放", "[actor][message_pool]") {
    auto& pool = MessagePool::instance();
    pool.clear();

    const int thread_count = 4;
    const int messages_per_thread = 20;

    std::vector<std::thread> threads;

    for (int t = 0; t < thread_count; ++t) {
        threads.emplace_back([t]() {
            // 在线程内创建作用域,确保 MessagePtr 在线程结束前析构
            {
                std::vector<MessagePtr<TestMessage>> ptrs;

                for (int i = 0; i < messages_per_thread; ++i) {
                    ptrs.emplace_back(MessagePtr<TestMessage>(
                        static_cast<TestMessage*>(
                            MessagePool::instance().allocate<TestMessage>(t * 100 + i)
                        )
                    ));
                }
                // ptrs 离开作用域,所有 MessagePtr 自动释放到 pool
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // 等待所有析构完成
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // 由于多线程并发执行和对象复用,free_size 可能小于总分配数
    // 这是正确的行为,因为内存池会复用已释放的对象
    // 我们只验证 free_size > 0,说明有消息被回收
    REQUIRE(pool.free_size() > 0);
}
