//**********************************
//  Created by boil on 2026/1/26.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include "common/event/event.h"
#include "common/io/io_context.h"
#include "common/log/logger.h"
#include <string>
#include <thread>

using namespace Rendu;
using namespace Rendu::event;
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

// 测试事件类型
class TestEvent : public TypedEvent<TestEvent> {
public:
    int value = 0;
    std::string message;

    TestEvent() = default;
    TestEvent(int v, const std::string& msg) : value(v), message(msg) {}
};

class AnotherEvent : public TypedEvent<AnotherEvent> {
public:
    double data = 0.0;
};

TEST_CASE("Event: 类型识别", "[event]") {
    TestEvent event;
    REQUIRE(event.type() == typeid(TestEvent).name());

    AnotherEvent another;
    REQUIRE(another.type() == typeid(AnotherEvent).name());

    REQUIRE(event.type() != another.type());
}

TEST_CASE("Event: 类型信息", "[event]") {
    TestEvent event;
    REQUIRE(event.type_info() == typeid(TestEvent));
    REQUIRE(event.type_info() != typeid(AnotherEvent));
}

TEST_CASE("Event: 克隆功能", "[event]") {
    TestEvent original(42, "test");
    original.value = 100;
    original.message = "hello";

    auto cloned = original.clone();
    REQUIRE(cloned != nullptr);

    const TestEvent* cloned_typed = dynamic_cast<const TestEvent*>(cloned.get());
    REQUIRE(cloned_typed != nullptr);
    REQUIRE(cloned_typed->value == original.value);
    REQUIRE(cloned_typed->message == original.message);

    // 修改原始不影响克隆
    original.value = 999;
    original.message = "modified";
    REQUIRE(cloned_typed->value == 100);
    REQUIRE(cloned_typed->message == "hello");
}

TEST_CASE("TypedEvent: 继承关系", "[event]") {
    TestEvent event;

    // 是 Event 的派生类
    Event* base_ptr = &event;
    REQUIRE(base_ptr != nullptr);

    // 可以通过基类接口调用
    std::string type = base_ptr->type();
    REQUIRE(type == typeid(TestEvent).name());
}

TEST_CASE("Event: 多个事件", "[event]") {
    TestEvent e1(1, "first");
    TestEvent e2(2, "second");
    AnotherEvent e3;

    REQUIRE(e1.type() == e2.type());
    REQUIRE(e1.type() != e3.type());

    auto clone1 = e1.clone();
    auto clone2 = e2.clone();

    const TestEvent* t1 = dynamic_cast<const TestEvent*>(clone1.get());
    const TestEvent* t2 = dynamic_cast<const TestEvent*>(clone2.get());

    REQUIRE(t1->value == 1);
    REQUIRE(t2->value == 2);
    REQUIRE(t1->message == "first");
    REQUIRE(t2->message == "second");
}
