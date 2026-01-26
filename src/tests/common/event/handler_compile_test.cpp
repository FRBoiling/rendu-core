//**********************************
//  Created by boil on 2026/1/26.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include "common/event/handler.h"
#include "common/event/event.h"
#include "common/event/event_bus.h"
#include <string>

using namespace Rendu;
using namespace Rendu::event;

// 测试事件类型
class TestEvent : public TypedEvent<TestEvent> {
public:
    int value = 0;
    TestEvent() = default;
    explicit TestEvent(int v) : value(v) {}
};

// 测试 EventHandler 类型是否正确定义
TEST_CASE("Handler: EventHandler 类型定义", "[event][handler][compile]") {
    // 测试 EventHandler 类型可以正常声明
    EventHandler handler1 = [](const Event& e) {
        const TestEvent* te = dynamic_cast<const TestEvent*>(&e);
        (void)te;
    };

    REQUIRE(handler1 != nullptr);

    // 测试 Handler 可以转换为 EventHandler
    Handler<TestEvent> typed_handler([](const TestEvent& e) {
        (void)e.value;
    });

    EventHandler handler2 = typed_handler;
    REQUIRE(handler2 != nullptr);
}

// 测试 make_helper 函数
TEST_CASE("Handler: make_helper 函数", "[event][handler][compile]") {
    auto handler = make_handler<TestEvent>([](const TestEvent& e) {
        (void)e.value;
    });

    // 确保返回类型是 EventHandler
    EventHandler eh = handler;
    REQUIRE(eh != nullptr);
}

// 测试类型转换
TEST_CASE("Handler: 类型转换", "[event][handler][compile]") {
    bool called = false;
    int received_value = 0;

    Handler<TestEvent> handler([&called, &received_value](const TestEvent& e) {
        called = true;
        received_value = e.value;
    });

    // 转换为 EventHandler
    EventHandler eh = handler;

    // 调用 EventHandler
    TestEvent event(42);
    eh(event);

    REQUIRE(called == true);
    REQUIRE(received_value == 42);
}
