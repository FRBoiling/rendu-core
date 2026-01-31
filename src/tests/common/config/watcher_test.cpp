//**********************************
//  Created by boil on 2026/1/31.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include "common/config/watcher.h"
#include "common/io/io_context.h"
#include "common/log/logger.h"
#include "common/log/console_sink.h"
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>

using namespace Rendu;
using namespace Rendu::config;

// 全局 io_context 用于日志
namespace {
    std::unique_ptr<io::IoContext> g_test_io_context;
}

// 在所有测试前初始化日志
struct LogInitializer {
    LogInitializer() {
        g_test_io_context = std::make_unique<io::IoContext>(2);
        log::init_default_io_context(*g_test_io_context);
        log::default_logger().add_sink(std::make_shared<log::ConsoleSink>());
        std::thread([&]() { g_test_io_context->run(); }).detach();
    }

    ~LogInitializer() {
        if (g_test_io_context) {
            g_test_io_context->stop();
        }
    }
};

static LogInitializer g_log_initializer;

// 创建临时测试文件
class TempConfigFile {
public:
    explicit TempConfigFile(const std::string& content) {
        path_ = std::filesystem::temp_directory_path() / ("test_config_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".json");

        std::ofstream file(path_);
        file << content;
        file.close();
    }

    ~TempConfigFile() {
        std::error_code ec;
        std::filesystem::remove(path_, ec);
    }

    const std::string& path() const {
        return path_;
    }

    void update(const std::string& content) {
        std::ofstream file(path_);
        file << content;
        file.close();
    }

private:
    std::string path_;
};

TEST_CASE("ConfigWatcher: 基本启动和停止", "[config][watcher]") {
    std::string json_content = R"({
        "port": 8080,
        "host": "localhost"
    })";

    TempConfigFile file(json_content);

    IoContext io(2);
    ConfigWatcher watcher(io, file.path());

    REQUIRE_FALSE(watcher.is_running());

    watcher.start();
    REQUIRE(watcher.is_running());

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    watcher.stop();
    REQUIRE_FALSE(watcher.is_running());
}

TEST_CASE("ConfigWatcher: 文件修改监控", "[config][watcher]") {
    std::string json_content = R"({
        "port": 8080,
        "host": "localhost"
    })";

    TempConfigFile file(json_content);

    IoContext io(2);
    ConfigWatcher watcher(io, file.path(), std::chrono::milliseconds(100));

    bool callback_called = false;
    watcher.on_change([&callback_called](const Config&) {
        callback_called = true;
    });

    std::thread io_thread([&io]() { io.run(); });

    watcher.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    file.update(R"({
        "port": 9090,
        "host": "127.0.0.1"
    })");

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    REQUIRE(callback_called);

    auto port_result = watcher.config().get<int64_t>("port");
    REQUIRE(std::holds_alternative<int64_t>(port_result));
    REQUIRE(std::get<int64_t>(port_result) == 9090);

    watcher.stop();
    io.stop();
    io_thread.join();
}

TEST_CASE("ConfigWatcher: 手动重载配置", "[config][watcher]") {
    std::string json_content = R"({
        "value": 100
    })";

    TempConfigFile file(json_content);

    IoContext io(2);
    ConfigWatcher watcher(io, file.path());

    auto initial_result = watcher.config().get<int64_t>("value");
    REQUIRE(std::holds_alternative<int64_t>(initial_result));
    REQUIRE(std::get<int64_t>(initial_result) == 100);

    file.update(R"({
        "value": 200
    })");

    auto reload_result = watcher.reload();
    REQUIRE(std::holds_alternative<Config>(reload_result));

    auto new_value = watcher.config().get<int64_t>("value");
    REQUIRE(std::holds_alternative<int64_t>(new_value));
    REQUIRE(std::get<int64_t>(new_value) == 200);
}

TEST_CASE("ConfigWatcher: 文件不存在", "[config][watcher]") {
    IoContext io(2);
    ConfigWatcher watcher(io, "/nonexistent/config.json");

    watcher.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    REQUIRE_FALSE(watcher.is_running());
}

TEST_CASE("ConfigWatcher: 多个回调", "[config][watcher]") {
    std::string json_content = R"({
        "data": "test"
    })";

    TempConfigFile file(json_content);

    IoContext io(2);
    ConfigWatcher watcher(io, file.path(), std::chrono::milliseconds(100));

    int callback1_count = 0;
    int callback2_count = 0;

    watcher.on_change([&callback1_count](const Config&) {
        callback1_count++;
    });

    watcher.on_change([&callback2_count](const Config&) {
        callback2_count++;
    });

    std::thread io_thread([&io]() { io.run(); });

    watcher.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    file.update(R"({
        "data": "updated"
    })");

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    REQUIRE(callback1_count == 1);
    REQUIRE(callback2_count == 1);

    watcher.stop();
    io.stop();
    io_thread.join();
}

TEST_CASE("ConfigWatcher: 嵌套配置", "[config][watcher]") {
    std::string json_content = R"({
        "database": {
            "host": "localhost",
            "port": 5432
        }
    })";

    TempConfigFile file(json_content);

    IoContext io(2);
    ConfigWatcher watcher(io, file.path(), std::chrono::milliseconds(100));

    std::thread io_thread([&io]() { io.run(); });

    watcher.start();

    auto initial_host = watcher.config().get<std::string>("database.host");
    REQUIRE(std::holds_alternative<std::string>(initial_host));
    REQUIRE(std::get<std::string>(initial_host) == "localhost");

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    file.update(R"({
        "database": {
            "host": "192.168.1.1",
            "port": 3306
        }
    })");

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    auto updated_host = watcher.config().get<std::string>("database.host");
    REQUIRE(std::holds_alternative<std::string>(updated_host));
    REQUIRE(std::get<std::string>(updated_host) == "192.168.1.1");

    watcher.stop();
    io.stop();
    io_thread.join();
}
