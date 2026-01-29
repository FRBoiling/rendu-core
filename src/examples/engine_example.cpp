#include "core/engine/engine.h"
#include "core/engine/context.h"
#include "common/log/logger.h"
#include "common/io/io_context.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <memory>

using namespace Rendu;

/// 自定义 Engine 子类，演示扩展
class MyEngine : public Engine {
public:
    MyEngine() = default;

    void run_custom_logic() {
        std::cout << "[MyEngine] 执行自定义逻辑..." << std::endl;
        // 可以在这里添加自定义的引擎逻辑
    }
};

int main() {
    std::cout << "========== Engine 示例程序 ==========" << std::endl;
    std::cout << std::endl;

    // 初始化 IO Context
    auto io = std::make_unique<io::IoContext>(1);
    log::init_default_io_context(*io);
    std::cout << "[IO] IO Context 已初始化" << std::endl;

    // 启动 IO Context 线程
    std::thread io_thread([&io]() {
        io->run();
    });
    std::cout << "[IO] IO Context 线程已启动" << std::endl;
    std::cout << std::endl;

    // 创建引擎实例
    MyEngine engine;
    std::cout << "[引擎] 引擎实例已创建" << std::endl;

    // 初始化引擎
    std::cout << "[引擎] 正在初始化..." << std::endl;
    engine.initialize();
    std::cout << "[引擎] 初始化成功" << std::endl;

    // 访问引擎上下文
    auto& context = engine.context();
    std::cout << "[上下文] 引擎上下文已获取" << std::endl;

    // 访问 Logger
    auto& logger = context.logger();
    logger.add_sink(std::make_shared<log::ConsoleSink>());
    std::cout << "[日志] 控制台 Sink 已添加" << std::endl;

    // 启动引擎
    std::cout << "[引擎] 正在启动..." << std::endl;
    engine.start();
    std::cout << "[引擎] 引擎已启动" << std::endl;

    std::cout << std::endl;
    std::cout << "=== 引擎运行中 ===" << std::endl;

    // 使用 Logger 记录日志
    RENDU_LOG_INFO("引擎正在运行");
    RENDU_LOG_WARN("这是一个警告消息示例");
    RENDU_LOG_ERROR("这是一个错误消息示例");

    // 执行自定义逻辑
    engine.run_custom_logic();

    // 模拟引擎运行
    for (int i = 1; i <= 3; ++i) {
        std::cout << "[引擎] 运行循环 " << i << "/3" << std::endl;
        RENDU_LOG_INFO("运行循环: {}", i);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << std::endl;
    std::cout << "=== 停止引擎 ===" << std::endl;

    // 停止引擎
    engine.stop();
    std::cout << "[引擎] 引擎已停止" << std::endl;

    std::cout << std::endl;
    std::cout << "========== 示例程序结束 ==========" << std::endl;

    // 停止 IO Context
    io->stop();
    io_thread.join();

    return 0;
}
