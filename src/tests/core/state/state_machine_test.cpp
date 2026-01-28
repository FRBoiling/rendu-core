//**********************************
//  Created by boil on 2026/01/28.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <core/state/state_machine.h>
#include <atomic>

using namespace Rendu;

class TestState : public State {
public:
    explicit TestState(std::string name) : State(std::move(name)) {}
    std::atomic<bool> entered{false};
    std::atomic<bool> exited{false};
    std::atomic<int> update_count{0};

    void on_enter() override { entered = true; }
    void on_exit() override { exited = true; }
    void on_update(float delta_time) override { update_count++; }
};

TEST_CASE("StateMachine basic", "[core][state]") {
    StateMachine sm;
    auto s1 = std::make_shared<TestState>("Idle");
    auto s2 = std::make_shared<TestState>("Running");

    sm.register_state(s1);
    sm.register_state(s2);
    sm.set_initial_state("Idle");

    sm.start();
    sm.update(0.016f);

    REQUIRE(sm.current_state()->name() == "Idle");
    REQUIRE(s1->entered == true);
    REQUIRE(s1->update_count > 0);
}

TEST_CASE("StateMachine transition", "[core][state]") {
    StateMachine sm;
    auto s1 = std::make_shared<TestState>("Idle");
    auto s2 = std::make_shared<TestState>("Running");

    sm.register_state(s1);
    sm.register_state(s2);
    sm.set_initial_state("Idle");

    sm.start();
    sm.update(0.016f);

    REQUIRE(sm.current_state()->name() == "Idle");
    REQUIRE(s1->entered == true);

    // 添加转换并触发
    sm.add_transition("Idle", "Running", []() { return true; });
    sm.update(0.016f); // 触发转换

    REQUIRE(sm.current_state()->name() == "Running");
    REQUIRE(s1->exited == true);
    REQUIRE(s2->entered == true);
}

TEST_CASE("StateMachine conditional transition", "[core][state]") {
    StateMachine sm;
    auto s1 = std::make_shared<TestState>("Idle");
    auto s2 = std::make_shared<TestState>("Running");

    sm.register_state(s1);
    sm.register_state(s2);
    sm.set_initial_state("Idle");

    bool should_transition = false;
    sm.add_transition("Idle", "Running", [&]() { return should_transition; });

    sm.start();
    sm.update(0.016f);

    // 条件不满足，不转换
    REQUIRE(sm.current_state()->name() == "Idle");

    // 满足条件
    should_transition = true;
    sm.update(0.016f);

    REQUIRE(sm.current_state()->name() == "Running");
}

TEST_CASE("StateMachine manual change", "[core][state]") {
    StateMachine sm;
    auto s1 = std::make_shared<TestState>("Idle");
    auto s2 = std::make_shared<TestState>("Running");

    sm.register_state(s1);
    sm.register_state(s2);
    sm.set_initial_state("Idle");

    sm.start();

    REQUIRE(sm.current_state()->name() == "Idle");

    sm.change_state("Running");

    REQUIRE(sm.current_state()->name() == "Running");
    REQUIRE(s1->exited == true);
    REQUIRE(s2->entered == true);
}

TEST_CASE("StateMachine multiple transitions", "[core][state]") {
    StateMachine sm;
    auto s1 = std::make_shared<TestState>("Idle");
    auto s2 = std::make_shared<TestState>("Running");
    auto s3 = std::make_shared<TestState>("Stopped");

    sm.register_state(s1);
    sm.register_state(s2);
    sm.register_state(s3);
    sm.set_initial_state("Idle");

    sm.start();
    sm.update(0.016f);
    REQUIRE(sm.current_state()->name() == "Idle");

    // 添加 Idle -> Running 转换
    sm.add_transition("Idle", "Running");
    sm.update(0.016f);
    REQUIRE(sm.current_state()->name() == "Running");

    // 添加 Running -> Stopped 转换
    sm.add_transition("Running", "Stopped");
    sm.update(0.016f);
    REQUIRE(sm.current_state()->name() == "Stopped");

    REQUIRE(s1->exited == true);
    REQUIRE(s2->exited == true);
    REQUIRE(s3->entered == true);
}

TEST_CASE("StateMachine has_state", "[core][state]") {
    StateMachine sm;
    auto s1 = std::make_shared<TestState>("Idle");
    auto s2 = std::make_shared<TestState>("Running");

    sm.register_state(s1);
    sm.register_state(s2);

    REQUIRE(sm.has_state("Idle") == true);
    REQUIRE(sm.has_state("Running") == true);
    REQUIRE(sm.has_state("Stopped") == false);
}

TEST_CASE("StateMachine error handling", "[core][state]") {
    StateMachine sm;

    SECTION("Register null state") {
        REQUIRE_THROWS(sm.register_state(nullptr));
    }

    SECTION("Set initial state without registration") {
        REQUIRE_THROWS(sm.set_initial_state("Idle"));
    }

    SECTION("Add transition without source state") {
        sm.register_state(std::make_shared<TestState>("Idle"));
        REQUIRE_THROWS(sm.add_transition("Running", "Idle"));
    }

    SECTION("Add transition without target state") {
        sm.register_state(std::make_shared<TestState>("Idle"));
        REQUIRE_THROWS(sm.add_transition("Idle", "Running"));
    }

    SECTION("Start without initial state") {
        sm.register_state(std::make_shared<TestState>("Idle"));
        REQUIRE_THROWS(sm.start());
    }

    SECTION("Change to non-existent state") {
        sm.register_state(std::make_shared<TestState>("Idle"));
        sm.set_initial_state("Idle");
        sm.start();
        sm.change_state("Running"); // 不应该抛出异常，只是记录错误日志
        REQUIRE(sm.current_state()->name() == "Idle");
    }
}

TEST_CASE("StateMachine replace state", "[core][state]") {
    StateMachine sm;
    auto s1 = std::make_shared<TestState>("Idle");
    auto s1_new = std::make_shared<TestState>("Idle");

    sm.register_state(s1);
    sm.register_state(s1_new); // 替换同名状态（会输出警告日志）
    sm.set_initial_state("Idle");
    sm.start();

    REQUIRE(sm.current_state()->name() == "Idle");
}

TEST_CASE("StateMachine update not started", "[core][state]") {
    StateMachine sm;
    auto s1 = std::make_shared<TestState>("Idle");

    sm.register_state(s1);
    sm.set_initial_state("Idle");

    // 没有启动，更新不会执行
    sm.update(0.016f);

    REQUIRE(s1->entered == false);
}
