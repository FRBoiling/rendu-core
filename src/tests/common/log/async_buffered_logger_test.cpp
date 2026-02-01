#include "common/log/async_buffered_logger.h"
#include "common/log/file_sink.h"
#include "common/io/io_context.h"
#include <catch2/catch_test_macros.hpp>
#include <thread>
#include <chrono>
#include <fstream>
#include <iostream>

// 全局 IO 对象
static Rendu::io::IoContext g_io(2);
static std::string g_test_log_file = "test_async_log.txt";

// 清理测试文件
struct TestFileCleanup {
    ~TestFileCleanup() {
        std::remove(g_test_log_file.c_str());
    }
};
static TestFileCleanup g_cleanup;

TEST_CASE("AsyncBufferedLogger - 基本构造", "[async_logger]") {
    Rendu::log::AsyncLoggerConfig config;
    config.buffer_size = 1024;
    config.flush_interval = std::chrono::milliseconds(50);

    Rendu::log::AsyncBufferedLogger logger("test", g_io, config);

    SECTION("Logger 创建成功") {
        REQUIRE(logger.name() == "test");
        REQUIRE(logger.config().buffer_size == 1024);
    }

    SECTION("默认日志级别为 Info") {
        REQUIRE(logger.level() == Rendu::log::Level::Info);
    }
}

TEST_CASE("AsyncBufferedLogger - 添加和移除 Sink", "[async_logger]") {
    Rendu::log::AsyncLoggerConfig config;
    Rendu::log::AsyncBufferedLogger logger("test", g_io, config);

    SECTION("添加 FileSink") {
        auto file_sink = std::make_shared<Rendu::log::FileSink>(g_test_log_file);
        logger.add_sink(file_sink);
        logger.flush();

        // 等待刷新
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        REQUIRE(logger.config().buffer_size > 0);
    }
}

TEST_CASE("AsyncBufferedLogger - 设置日志级别", "[async_logger]") {
    Rendu::log::AsyncLoggerConfig config;
    Rendu::log::AsyncBufferedLogger logger("test", g_io, config);

    SECTION("设置 Debug 级别") {
        logger.set_level(Rendu::log::Level::Debug);
        REQUIRE(logger.level() == Rendu::log::Level::Debug);
    }

    SECTION("设置 Warn 级别") {
        logger.set_level(Rendu::log::Level::Warn);
        REQUIRE(logger.level() == Rendu::log::Level::Warn);
    }
}

TEST_CASE("AsyncBufferedLogger - 基本日志记录", "[async_logger]") {
    Rendu::log::AsyncLoggerConfig config;
    config.buffer_size = 10;
    config.flush_interval = std::chrono::milliseconds(50);

    SECTION("记录 Info 日志") {
        auto file_sink = std::make_shared<Rendu::log::FileSink>(g_test_log_file, Rendu::log::Level::Trace);
        Rendu::log::AsyncBufferedLogger logger("test", g_io, config);
        logger.add_sink(file_sink);

        logger.info("Test info message");
        logger.flush();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        std::ifstream file(g_test_log_file);
        REQUIRE(file.is_open());

        std::string line;
        bool found = false;
        while (std::getline(file, line)) {
            if (!line.empty() && line.find("Test info message") != std::string::npos) {
                found = true;
                break;
            }
        }
        REQUIRE(found);
        file.close();
    }

    SECTION("记录 Debug 日志") {
        // 清理文件
        std::remove(g_test_log_file.c_str());

        auto file_sink = std::make_shared<Rendu::log::FileSink>(g_test_log_file, Rendu::log::Level::Trace);
        Rendu::log::AsyncBufferedLogger logger("test", g_io, config);
        logger.add_sink(file_sink);

        logger.set_level(Rendu::log::Level::Debug);
        logger.debug("Test debug message");
        logger.flush();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        std::ifstream file(g_test_log_file);
        REQUIRE(file.is_open());
        std::string line;
        bool found = false;
        while (std::getline(file, line)) {
            if (!line.empty() && line.find("Test debug message") != std::string::npos) {
                found = true;
                break;
            }
        }
        REQUIRE(found);
        file.close();
    }
}

TEST_CASE("AsyncBufferedLogger - 多级别日志", "[async_logger]") {
    Rendu::log::AsyncLoggerConfig config;
    config.buffer_size = 10;
    config.flush_interval = std::chrono::milliseconds(50);

    SECTION("记录所有级别") {
        // 清理文件
        std::remove(g_test_log_file.c_str());

        auto file_sink = std::make_shared<Rendu::log::FileSink>(g_test_log_file, Rendu::log::Level::Trace);
        Rendu::log::AsyncBufferedLogger logger("test", g_io, config);
        logger.set_level(Rendu::log::Level::Trace);
        logger.add_sink(file_sink);

        logger.trace("Trace message");
        logger.debug("Debug message");
        logger.info("Info message");
        logger.warn("Warn message");
        logger.error("Error message");
        logger.critical("Critical message");
        logger.flush();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        std::ifstream file(g_test_log_file);
        REQUIRE(file.is_open());
        std::string line;
        int line_count = 0;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                line_count++;
            }
        }
        REQUIRE(line_count >= 6);
        file.close();
    }
}

TEST_CASE("AsyncBufferedLogger - 自动刷新", "[async_logger]") {
    Rendu::log::AsyncLoggerConfig config;
    config.buffer_size = 1000;  // 大缓冲区
    config.flush_interval = std::chrono::milliseconds(100);
    config.auto_flush = true;

    auto file_sink = std::make_shared<Rendu::log::FileSink>(g_test_log_file);
    Rendu::log::AsyncBufferedLogger logger("test", g_io, config);
    logger.add_sink(file_sink);

    SECTION("自动刷新日志") {
        logger.info("Auto flush message");

        // 等待自动刷新
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        std::ifstream file(g_test_log_file);
        REQUIRE(file.is_open());
        std::string line;
        bool found = false;
        while (std::getline(file, line)) {
            if (!line.empty() && line.find("Auto flush message") != std::string::npos) {
                found = true;
                break;
            }
        }
        REQUIRE(found);
        file.close();
    }
}

TEST_CASE("AsyncBufferedLogger - 手动刷新", "[async_logger]") {
    Rendu::log::AsyncLoggerConfig config;
    config.buffer_size = 1000;
    config.auto_flush = false;  // 禁用自动刷新

    auto file_sink = std::make_shared<Rendu::log::FileSink>(g_test_log_file);
    Rendu::log::AsyncBufferedLogger logger("test", g_io, config);
    logger.add_sink(file_sink);

    SECTION("手动刷新日志") {
        logger.info("Manual flush message");

        // 不等待自动刷新，手动刷新
        logger.flush();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        std::ifstream file(g_test_log_file);
        REQUIRE(file.is_open());
        std::string line;
        bool found = false;
        while (std::getline(file, line)) {
            if (!line.empty() && line.find("Manual flush message") != std::string::npos) {
                found = true;
                break;
            }
        }
        REQUIRE(found);
        file.close();
    }
}

TEST_CASE("AsyncBufferedLogger - 缓冲区满时自动刷新", "[async_logger]") {
    Rendu::log::AsyncLoggerConfig config;
    config.buffer_size = 500;  // 小缓冲区，约能容纳 5-10 条消息
    config.flush_interval = std::chrono::milliseconds(10000);  // 很长的刷新间隔
    config.auto_flush = false;

    SECTION("缓冲区满时自动刷新") {
        // 清理文件
        std::remove(g_test_log_file.c_str());

        auto file_sink = std::make_shared<Rendu::log::FileSink>(g_test_log_file, Rendu::log::Level::Trace);
        Rendu::log::AsyncBufferedLogger logger("test", g_io, config);
        logger.add_sink(file_sink);

        // 写入超过缓冲区大小的消息
        for (int i = 0; i < 10; ++i) {
            logger.info("Message " + std::to_string(i));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        std::ifstream file(g_test_log_file);
        REQUIRE(file.is_open());
        std::string line;
        int line_count = 0;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                line_count++;
            }
        }
        REQUIRE(line_count >= 1);  // 缓冲区满时应该有消息被刷新
        file.close();
    }
}

TEST_CASE("AsyncBufferedLogger - 统计信息", "[async_logger]") {
    Rendu::log::AsyncLoggerConfig config;
    config.buffer_size = 10;
    config.flush_interval = std::chrono::milliseconds(50);

    Rendu::log::AsyncBufferedLogger logger("test", g_io, config);

    SECTION("初始统计信息") {
        auto stats = logger.get_stats();
        REQUIRE(stats.buffered_count == 0);
        REQUIRE(stats.flushed_count == 0);
        REQUIRE(stats.dropped_count == 0);
    }

    SECTION("记录日志后统计信息") {
        logger.info("Test message 1");
        logger.info("Test message 2");
        logger.flush();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        auto stats = logger.get_stats();
        REQUIRE(stats.buffered_count == 0);  // flush 后应该清零
        REQUIRE(stats.flushed_count >= 2);
    }
}

TEST_CASE("AsyncBufferedLogger - 多线程并发", "[async_logger]") {
    Rendu::log::AsyncLoggerConfig config;
    config.buffer_size = 100;
    config.flush_interval = std::chrono::milliseconds(50);

    auto file_sink = std::make_shared<Rendu::log::FileSink>(g_test_log_file);
    Rendu::log::AsyncBufferedLogger logger("test", g_io, config);
    logger.add_sink(file_sink);

    SECTION("多线程并发写入") {
        const int thread_count = 4;
        const int messages_per_thread = 10;
        std::vector<std::thread> threads;

        for (int i = 0; i < thread_count; ++i) {
            threads.emplace_back([&logger, i, messages_per_thread]() {
                for (int j = 0; j < messages_per_thread; ++j) {
                    logger.info("Thread " + std::to_string(i) + " message " + std::to_string(j));
                }
            });
        }

        // 等待所有线程完成
        for (auto& t : threads) {
            t.join();
        }

        logger.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // 验证文件中的日志数量
        std::ifstream file(g_test_log_file);
        REQUIRE(file.is_open());
        std::string line;
        int line_count = 0;
        while (std::getline(file, line)) {
            line_count++;
        }
        REQUIRE(line_count >= thread_count * messages_per_thread);
        file.close();
    }
}

TEST_CASE("AsyncBufferedLogger - 全局默认日志器", "[async_logger]") {
    SECTION("初始化全局默认日志器") {
        Rendu::log::AsyncLoggerConfig config;
        Rendu::log::init_default_async_logger(g_io, config);

        REQUIRE_NOTHROW(Rendu::log::default_async_logger().info("Test message"));
    }

    SECTION("重置全局默认日志器") {
        Rendu::log::reset_default_async_logger();
        // 再次初始化应该成功
        REQUIRE_NOTHROW(Rendu::log::init_default_async_logger(g_io));
    }
}

TEST_CASE("AsyncBufferedLogger - 配置灵活性", "[async_logger]") {
    SECTION("大缓冲区配置") {
        Rendu::log::AsyncLoggerConfig config;
        config.buffer_size = 1024 * 1024;  // 1MB
        config.message_pool_size = 10000;

        Rendu::log::AsyncBufferedLogger logger("test", g_io, config);
        REQUIRE(logger.config().buffer_size == 1024 * 1024);
        REQUIRE(logger.config().message_pool_size == 10000);
    }

    SECTION("小缓冲区快速刷新配置") {
        Rendu::log::AsyncLoggerConfig config;
        config.buffer_size = 10;
        config.flush_interval = std::chrono::milliseconds(10);
        config.auto_flush = true;

        Rendu::log::AsyncBufferedLogger logger("test", g_io, config);
        REQUIRE(logger.config().buffer_size == 10);
        REQUIRE(logger.config().flush_interval.count() == 10);
        REQUIRE(logger.config().auto_flush == true);
    }

    SECTION("禁用自动刷新") {
        Rendu::log::AsyncLoggerConfig config;
        config.auto_flush = false;

        Rendu::log::AsyncBufferedLogger logger("test", g_io, config);
        REQUIRE(logger.config().auto_flush == false);
    }
}
