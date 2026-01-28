//**********************************
//  Created by boil on 2026/01/28.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include <common/log/logger.h>
#include <common/log/console_sink.h>
#include <common/log/file_sink.h>
#include <common/log/formatter.h>
#include <common/io/io_context.h>
#include <thread>
#include <sstream>
#include <chrono>

using namespace Rendu::log;
using namespace Rendu::io;

TEST_CASE("Logger 基本功能", "[log][logger]") {
    IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();

    Logger logger("test", io);
    logger.add_sink(std::make_shared<ConsoleSink>(Level::Info));

    REQUIRE_NOTHROW(logger.info("test message"));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    io.stop();
}

TEST_CASE("Logger 异步日志", "[log][logger]") {
    IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();

    Logger logger("test", io);
    logger.add_sink(std::make_shared<ConsoleSink>(Level::Info));

    // 快速发送多条日志，验证异步处理
    for (int i = 0; i < 10; ++i) {
        logger.info("async message " + std::to_string(i));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    io.stop();
}

TEST_CASE("Logger 级别过滤", "[log][logger]") {
    IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();

    Logger logger("test", io);
    logger.set_level(Level::Warn);
    logger.add_sink(std::make_shared<ConsoleSink>(Level::Info));

    logger.info("should not appear");
    logger.warn("should appear");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    io.stop();
}

TEST_CASE("Logger convenience methods", "[log][logger]") {
    IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();

    Logger logger("test", io);
    logger.add_sink(std::make_shared<ConsoleSink>(Level::Trace));

    REQUIRE_NOTHROW(logger.trace("trace message"));
    REQUIRE_NOTHROW(logger.debug("debug message"));
    REQUIRE_NOTHROW(logger.info("info message"));
    REQUIRE_NOTHROW(logger.warn("warn message"));
    REQUIRE_NOTHROW(logger.error("error message"));
    REQUIRE_NOTHROW(logger.critical("critical message"));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    io.stop();
}

TEST_CASE("Logger default logger", "[log][logger]") {
    IoContext io(1);
    init_default_io_context(io);

    std::thread([&io]() { io.run(); }).detach();

    auto& logger = default_logger();

    REQUIRE_NOTHROW(logger.info("default logger test"));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    io.stop();
}

TEST_CASE("Logger named logger", "[log][logger]") {
    IoContext io(1);
    init_default_io_context(io);

    auto& logger1 = get_logger("module1");
    auto& logger2 = get_logger("module1");

    REQUIRE(logger1.name() == logger2.name());
}

TEST_CASE("Logger add/remove sink", "[log][logger]") {
    IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();

    Logger logger("test", io);
    auto sink = std::make_shared<ConsoleSink>(Level::Info);

    logger.add_sink(sink);
    logger.info("with sink");

    logger.remove_sink(sink);
    logger.clear_sinks();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    io.stop();
}

TEST_CASE("Logger structured logging", "[log][logger]") {
    IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();

    Logger logger("test", io);
    logger.add_sink(std::make_shared<ConsoleSink>(Level::Info));

    logger.info_fields("user login", "user_id", "12345", "ip", "192.168.1.1", "status", "success");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    io.stop();
}

TEST_CASE("Logger with file sink", "[log][logger]") {
    IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();

    Logger logger("test", io);
    auto file_sink = std::make_shared<FileSink>("test.log", Level::Info);
    logger.add_sink(file_sink);

    logger.info("message to file");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    io.stop();

    // 清理测试文件
    std::remove("test.log");
}

TEST_CASE("PatternFormatter custom format", "[log][logger]") {
    PatternFormatter formatter("%t|%l|%n|%m");

    LogMessage msg;
    msg.level = Level::Info;
    msg.logger_name = "test";
    msg.message = "hello";
    msg.timestamp = "2024-01-01 12:00:00.000";
    msg.thread_id = std::this_thread::get_id();

    std::string result = formatter.format(msg);

    REQUIRE(result == "2024-01-01 12:00:00.000|INFO|test|hello");
}

TEST_CASE("PatternFormatter with fields", "[log][logger]") {
    PatternFormatter formatter("%m [%f]");

    LogMessage msg;
    msg.level = Level::Info;
    msg.logger_name = "test";
    msg.message = "event";
    msg.timestamp = "2024-01-01 12:00:00.000";
    msg.thread_id = std::this_thread::get_id();
    msg.fields["key1"] = "value1";
    msg.fields["key2"] = "value2";

    std::string result = formatter.format(msg);

    REQUIRE(result == "event [key1=value1 key2=value2]");
}
