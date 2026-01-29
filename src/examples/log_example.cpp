#include <iostream>
#include <thread>
#include <chrono>

#include "common/log/logger.h"
#include "common/log/console_sink.h"
#include "common/log/file_sink.h"
#include "common/log/formatter.h"

using namespace Rendu;

int main() {
    std::cout << "========== 日志 示例程序 ==========" << std::endl;
    std::cout << std::endl;

    // 初始化 IO Context
    auto io = std::make_unique<io::IoContext>(1);
    log::init_default_io_context(*io);

    // 启动 IO Context 线程
    std::thread io_thread([&io]() {
        io->run();
    });

    // 获取默认 Logger
    auto& logger = log::default_logger();
    std::cout << "[日志] 已获取默认 Logger: " << logger.name() << std::endl;

    // 添加控制台 Sink
    auto console_sink = std::make_shared<log::ConsoleSink>();
    console_sink->set_formatter(std::make_shared<log::PatternFormatter>(
        "[%t] [%l] [%n] %m"
    ));
    logger.add_sink(console_sink);
    std::cout << "[日志] 已添加控制台 Sink" << std::endl;

    // 添加文件 Sink（可选）
    try {
        auto file_sink = std::make_shared<log::FileSink>("example.log", log::Level::Debug);
        file_sink->set_formatter(std::make_shared<log::PatternFormatter>(
            "[%t] [%l] [%n] %m"
        ));
        logger.add_sink(file_sink);
        std::cout << "[日志] 已添加文件 Sink: example.log" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "[日志] 无法创建文件 Sink: " << e.what() << std::endl;
    }

    // 设置日志级别
    logger.set_level(log::Level::Debug);
    std::cout << "[日志] 日志级别已设置为: Debug" << std::endl;

    std::cout << std::endl;
    std::cout << "=== 演示不同级别的日志 ===" << std::endl;

    // 记录不同级别的日志
    logger.trace("这是一条 TRACE 级别的日志");
    logger.debug("这是一条 DEBUG 级别的日志");
    logger.info("这是一条 INFO 级别的日志");
    logger.warn("这是一条 WARN 级别的日志");
    logger.error("这是一条 ERROR 级别的日志");
    logger.critical("这是一条 CRITICAL 级别的日志");

    std::cout << std::endl;
    std::cout << "=== 演示结构化日志（带字段）===" << std::endl;

    // 使用结构化日志（带键值字段）
    logger.info_fields("用户登录", "user_id", "12345", "username", "test_user", "ip", "192.168.1.1");
    logger.warn_fields("系统资源使用率高", "cpu_usage", "85%", "memory_usage", "70%");
    logger.error_fields("请求处理失败", "request_id", "req-001", "error_code", "500", "error_msg", "Internal Server Error");

    std::cout << std::endl;
    std::cout << "=== 演示多线程日志 ===" << std::endl;

    // 创建多个线程同时记录日志
    std::vector<std::thread> threads;
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back([&logger, i]() {
            for (int j = 0; j < 3; ++j) {
                logger.info_fields("线程日志", "thread_id", std::to_string(i), "message", "这是第 " + std::to_string(j+1) + " 条消息");
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
    }

    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }

    std::cout << std::endl;
    std::cout << "=== 演示动态日志级别调整 ===" << std::endl;

    // 临时调整日志级别
    logger.info("当前日志级别: Debug");
    logger.set_level(log::Level::Warn);
    logger.info("这条 INFO 日志不会被输出（级别已改为 Warn）");
    logger.warn("这条 WARN 日志会被输出");
    logger.error("这条 ERROR 日志会被输出");

    // 恢复日志级别
    logger.set_level(log::Level::Debug);
    logger.info("日志级别已恢复为 Debug");

    std::cout << std::endl;
    std::cout << "=== 清理 ===" << std::endl;

    // 停止 IO Context
    io->stop();
    io_thread.join();

    std::cout << "[日志] IO Context 已停止" << std::endl;

    std::cout << std::endl;
    std::cout << "========== 示例程序结束 ==========" << std::endl;

    return 0;
}
