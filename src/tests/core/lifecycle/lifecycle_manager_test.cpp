#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <core/lifecycle/lifecycle_manager.h>
#include <common/log/logger.h>
#include <common/io/io_context.h>
#include <atomic>
#include <thread>
#include <chrono>

using namespace Rendu;
using namespace Rendu::log;

namespace {
    // 全局 io_context 和 logger，用于测试
    Rendu::io::IoContext* g_test_io = nullptr;
}

// Catch2 fixture 用于初始化和清理日志系统
struct TestLoggerFixture {
    TestLoggerFixture() {
        static Rendu::io::IoContext io(1);
        g_test_io = &io;
        log::init_default_io_context(io);
    }

    ~TestLoggerFixture() {
        if (g_test_io) {
            g_test_io->stop();
        }
    }
};

// 全局 fixture 实例
TestLoggerFixture g_logger_fixture;

class TestComponent : public ILifecycle {
public:
    explicit TestComponent(std::string name) : name_(std::move(name)) {}

    void initialize() override {
        if (initialized_) {
            throw std::runtime_error("Already initialized");
        }
        initialized_ = true;
        init_order_.push_back(name_);
    }

    void shutdown() override {
        if (shutdown_called_) {
            throw std::runtime_error("Already shutdown");
        }
        shutdown_called_ = true;
        shutdown_order_.push_back(name_);
    }

    std::vector<std::string> dependencies() const override {
        return deps_;
    }

    const std::string& name() const { return name_; }

    std::string name_;
    std::vector<std::string> deps_;
    std::atomic<bool> initialized_{false};
    std::atomic<bool> shutdown_called_{false};

    static std::vector<std::string> init_order_;
    static std::vector<std::string> shutdown_order_;
};

std::vector<std::string> TestComponent::init_order_;
std::vector<std::string> TestComponent::shutdown_order_;

TEST_CASE("LifecycleManager basic", "[core][lifecycle]") {
    TestComponent::init_order_.clear();
    TestComponent::shutdown_order_.clear();

    LifecycleManager manager;
    auto comp1 = std::make_shared<TestComponent>("Comp1");
    auto comp2 = std::make_shared<TestComponent>("Comp2");

    manager.register_component("Comp1", comp1);
    manager.register_component("Comp2", comp2);

    manager.initialize_all();

    REQUIRE(comp1->initialized_);
    REQUIRE(comp2->initialized_);
    REQUIRE(manager.is_initialized());

    manager.shutdown_all();

    REQUIRE(comp1->shutdown_called_);
    REQUIRE(comp2->shutdown_called_);
    REQUIRE(!manager.is_initialized());
}

TEST_CASE("LifecycleManager no components", "[core][lifecycle]") {
    LifecycleManager manager;
    manager.initialize_all();
    REQUIRE(manager.is_initialized());
    manager.shutdown_all();
    REQUIRE(!manager.is_initialized());
}

TEST_CASE("LifecycleManager dependencies", "[core][lifecycle]") {
    TestComponent::init_order_.clear();
    TestComponent::shutdown_order_.clear();

    LifecycleManager manager;
    auto comp1 = std::make_shared<TestComponent>("Comp1");
    auto comp2 = std::make_shared<TestComponent>("Comp2");
    comp2->deps_ = {"Comp1"}; // Comp2 依赖 Comp1

    manager.register_component("Comp1", comp1);
    manager.register_component("Comp2", comp2);

    manager.initialize_all();

    // 验证初始化顺序：Comp1 应先初始化
    REQUIRE(TestComponent::init_order_.size() == 2);
    REQUIRE(TestComponent::init_order_[0] == "Comp1");
    REQUIRE(TestComponent::init_order_[1] == "Comp2");

    manager.shutdown_all();

    // 验证关闭顺序：Comp2 应先关闭（初始化的逆序）
    REQUIRE(TestComponent::shutdown_order_.size() == 2);
    REQUIRE(TestComponent::shutdown_order_[0] == "Comp2");
    REQUIRE(TestComponent::shutdown_order_[1] == "Comp1");
}

TEST_CASE("LifecycleManager complex dependencies", "[core][lifecycle]") {
    TestComponent::init_order_.clear();
    TestComponent::shutdown_order_.clear();

    LifecycleManager manager;
    auto comp1 = std::make_shared<TestComponent>("Comp1");
    auto comp2 = std::make_shared<TestComponent>("Comp2");
    auto comp3 = std::make_shared<TestComponent>("Comp3");
    auto comp4 = std::make_shared<TestComponent>("Comp4");

    // 依赖关系：
    // Comp2 -> Comp1
    // Comp3 -> Comp1
    // Comp4 -> Comp2, Comp3
    comp2->deps_ = {"Comp1"};
    comp3->deps_ = {"Comp1"};
    comp4->deps_ = {"Comp2", "Comp3"};

    manager.register_component("Comp1", comp1);
    manager.register_component("Comp2", comp2);
    manager.register_component("Comp3", comp3);
    manager.register_component("Comp4", comp4);

    manager.initialize_all();

    // 验证初始化顺序
    REQUIRE(TestComponent::init_order_.size() == 4);
    REQUIRE(TestComponent::init_order_[0] == "Comp1"); // Comp1 无依赖，最先初始化
    bool match1 = (TestComponent::init_order_[1] == "Comp2") || (TestComponent::init_order_[1] == "Comp3");
    REQUIRE(match1);
    bool match2 = (TestComponent::init_order_[2] == "Comp2") || (TestComponent::init_order_[2] == "Comp3");
    REQUIRE(match2);
    REQUIRE(TestComponent::init_order_[3] == "Comp4"); // Comp4 依赖 Comp2 和 Comp3，最后初始化
}

TEST_CASE("LifecycleManager circular dependency", "[core][lifecycle]") {
    TestComponent::init_order_.clear();
    TestComponent::shutdown_order_.clear();

    LifecycleManager manager;
    auto comp1 = std::make_shared<TestComponent>("Comp1");
    auto comp2 = std::make_shared<TestComponent>("Comp2");

    // 循环依赖：Comp1 -> Comp2 -> Comp1
    comp1->deps_ = {"Comp2"};
    comp2->deps_ = {"Comp1"};

    manager.register_component("Comp1", comp1);
    manager.register_component("Comp2", comp2);

    REQUIRE_THROWS(manager.initialize_all());
}

TEST_CASE("LifecycleManager register after init", "[core][lifecycle]") {
    LifecycleManager manager;
    auto comp1 = std::make_shared<TestComponent>("Comp1");

    manager.register_component("Comp1", comp1);
    manager.initialize_all();

    auto comp2 = std::make_shared<TestComponent>("Comp2");

    REQUIRE_THROWS(manager.register_component("Comp2", comp2));
}

TEST_CASE("LifecycleManager null component", "[core][lifecycle]") {
    LifecycleManager manager;

    REQUIRE_THROWS(manager.register_component("Null", nullptr));
}

TEST_CASE("LifecycleManager replace component", "[core][lifecycle]") {
    TestComponent::init_order_.clear();
    TestComponent::shutdown_order_.clear();

    LifecycleManager manager;
    auto comp1 = std::make_shared<TestComponent>("Comp1");
    auto comp1_new = std::make_shared<TestComponent>("Comp1");

    manager.register_component("Comp1", comp1);
    manager.register_component("Comp1", comp1_new); // 替换同名组件（会输出警告日志）
    manager.initialize_all();

    REQUIRE(comp1_new->initialized_);
    REQUIRE(!comp1->initialized_);
}

TEST_CASE("LifecycleManager get component", "[core][lifecycle]") {
    LifecycleManager manager;
    auto comp1 = std::make_shared<TestComponent>("Comp1");

    manager.register_component("Comp1", comp1);

    auto* retrieved = manager.get<ILifecycle>("Comp1");
    REQUIRE(retrieved != nullptr);
    REQUIRE(retrieved->name() == "Comp1");

    auto* not_found = manager.get<ILifecycle>("NonExistent");
    REQUIRE(not_found == nullptr);
}

TEST_CASE("LifecycleManager has component", "[core][lifecycle]") {
    LifecycleManager manager;
    auto comp1 = std::make_shared<TestComponent>("Comp1");

    REQUIRE(!manager.has_component("Comp1"));

    manager.register_component("Comp1", comp1);

    REQUIRE(manager.has_component("Comp1"));
    REQUIRE(!manager.has_component("Comp2"));
}

TEST_CASE("LifecycleManager get component names", "[core][lifecycle]") {
    LifecycleManager manager;
    auto comp1 = std::make_shared<TestComponent>("Comp1");
    auto comp2 = std::make_shared<TestComponent>("Comp2");

    manager.register_component("Comp1", comp1);
    manager.register_component("Comp2", comp2);

    auto names = manager.get_component_names();
    REQUIRE(names.size() == 2);
    REQUIRE((names[0] == "Comp1" || names[0] == "Comp2"));
    REQUIRE((names[1] == "Comp1" || names[1] == "Comp2"));
}

TEST_CASE("LifecycleManager missing dependency", "[core][lifecycle]") {
    TestComponent::init_order_.clear();

    LifecycleManager manager;
    auto comp1 = std::make_shared<TestComponent>("Comp1");
    comp1->deps_ = {"NonExistent"}; // 依赖不存在的组件

    manager.register_component("Comp1", comp1);

    // 应该能初始化，但会输出警告日志
    manager.initialize_all();

    REQUIRE(comp1->initialized_);
}

TEST_CASE("LifecycleManager init error propagation", "[core][lifecycle]") {
    TestComponent::init_order_.clear();
    TestComponent::shutdown_order_.clear();

    LifecycleManager manager;
    auto comp1 = std::make_shared<TestComponent>("Comp1");
    auto comp2 = std::make_shared<TestComponent>("Comp2");

    class FailingComponent : public TestComponent {
    public:
        explicit FailingComponent(std::string name) : TestComponent(std::move(name)) {}
        void initialize() override {
            TestComponent::initialize();
            throw std::runtime_error("Initialization failed");
        }
    };

    auto comp_fail = std::make_shared<FailingComponent>("FailingComp");

    manager.register_component("Comp1", comp1);
    manager.register_component("FailingComp", comp_fail);
    manager.register_component("Comp2", comp2);

    REQUIRE_THROWS(manager.initialize_all());

    // 失败后应该关闭已初始化的组件
    REQUIRE(comp1->shutdown_called_);
}

TEST_CASE("LifecycleManager double init", "[core][lifecycle]") {
    LifecycleManager manager;
    auto comp1 = std::make_shared<TestComponent>("Comp1");

    manager.register_component("Comp1", comp1);
    manager.initialize_all();

    manager.initialize_all(); // 再次初始化应该被忽略
    REQUIRE(manager.is_initialized());
}

TEST_CASE("LifecycleManager double shutdown", "[core][lifecycle]") {
    LifecycleManager manager;
    auto comp1 = std::make_shared<TestComponent>("Comp1");

    manager.register_component("Comp1", comp1);
    manager.initialize_all();

    manager.shutdown_all();
    manager.shutdown_all(); // 再次关闭应该被忽略
    REQUIRE(!manager.is_initialized());
}

TEST_CASE("LifecycleManager thread safety", "[core][lifecycle]") {
    TestComponent::init_order_.clear();
    TestComponent::shutdown_order_.clear();

    LifecycleManager manager;

    std::vector<std::shared_ptr<TestComponent>> components;
    for (int i = 0; i < 10; ++i) {
        auto comp = std::make_shared<TestComponent>("Comp" + std::to_string(i));
        components.push_back(comp);
        manager.register_component(comp->name(), comp);
    }

    // 初始化
    std::thread init_thread([&]() {
        manager.initialize_all();
    });

    init_thread.join();

    // 验证所有组件都已初始化
    for (const auto& comp : components) {
        REQUIRE(comp->initialized_);
    }

    // 关闭
    std::thread shutdown_thread([&]() {
        manager.shutdown_all();
    });

    shutdown_thread.join();

    // 验证所有组件都已关闭
    for (const auto& comp : components) {
        REQUIRE(comp->shutdown_called_);
    }
}

TEST_CASE("LifecycleManager self dependency", "[core][lifecycle]") {
    TestComponent::init_order_.clear();

    LifecycleManager manager;
    auto comp1 = std::make_shared<TestComponent>("Comp1");
    comp1->deps_ = {"Comp1"}; // 自依赖

    manager.register_component("Comp1", comp1);

    // 自依赖应该被检测为循环依赖
    REQUIRE_THROWS(manager.initialize_all());
}
