#include "simple_test.h"
#include "test_client.h"
#include "common/log/logger.h"
#include <thread>
#include <chrono>

using namespace Rendu;
using namespace Rendu::net;

namespace Rendu {
namespace client {

void run_simple_test(const std::string& host, uint16_t port) {
    // 初始化日志
    io::IoContext log_io;
    log::init_default_io_context(log_io);
    
    // 创建并设置默认 logger
    auto logger = std::make_shared<log::Logger>("test_client", log_io);
    logger->set_level(log::Level::Info);
    log::set_default_logger(logger);

    // 创建 IO 上下文
    io::IoContext io;

    // 创建测试客户端
    TestClient client(1, io);

    // 设置回调
    client.set_message_callback([](const protocol::ServerMessage& msg) {
        if (msg.has_login_response()) {
            RENDU_LOG_INFO("Login response: success={}", msg.login_response().success());
        } else if (msg.has_chat()) {
            RENDU_LOG_INFO("Received chat: {}", msg.chat().content());
        }
    });

    client.set_error_callback([](const std::string& error) {
        RENDU_LOG_ERROR("Error: {}", error);
    });

    RENDU_LOG_INFO("Connecting to {}:{}...", host, port);
    client.connect(host, port, [&]() {
        RENDU_LOG_INFO("Connected to server");
        // 连接成功后登录
        client.login("test_user");

        // 发送一些测试消息
        for (int i = 0; i < 5; ++i) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            client.send_chat("Hello from client " + std::to_string(i));
        }

        // 断开连接
        std::this_thread::sleep_for(std::chrono::seconds(1));
        client.disconnect();
    });

    // 启动 IO 循环
    std::thread io_thread([&io]() {
        io.run();
    });

    // 等待一段时间
    std::this_thread::sleep_for(std::chrono::seconds(10));

    // 停止 IO 上下文
    io.stop();
    if (io_thread.joinable()) {
        io_thread.join();
    }

    RENDU_LOG_INFO("Client shutdown complete");
}

} // namespace client
} // namespace Rendu
