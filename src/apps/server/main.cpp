#include "core/engine/engine.h"
#include "core/actor/actor.h"
#include "core/actor/actor_ref.h"
#include "common/net/socket.h"
#include "server_actor.h"
#include "session_actor.h"
#include "server_messages.h"
#include "common/log/logger.h"
#include "common/io/io_context.h"
#include <iostream>
#include <thread>
#include <memory>
#include <atomic>
#include <csignal>
#include <mutex>
#include <condition_variable>

using namespace Rendu;

/// 信号处理器单例
class ServerSignalHandler {
public:
    static ServerSignalHandler& instance() {
        static ServerSignalHandler handler;
        return handler;
    }

    void wait_for_shutdown() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !running_; });
    }

    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            running_ = false;
        }
        cv_.notify_all();
    }

    bool is_running() const { return running_; }

private:
    ServerSignalHandler() : running_(true) {
        std::signal(SIGINT, &ServerSignalHandler::signal_handler);
        std::signal(SIGTERM, &ServerSignalHandler::signal_handler);
    }

    static void signal_handler(int signal) {
        std::cout << "\n收到信号 " << signal << ", 正在关闭服务器..." << std::endl;
        instance().shutdown();
    }

    std::atomic<bool> running_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

/// 服务器配置
struct ServerConfig {
    uint16_t port = 8080;
    int io_threads = 4;
    int actor_threads = 4;
};

int main() {
    std::cout << "========== RenduCore 服务器示例 ==========" << std::endl;
    std::cout << std::endl;

    ServerConfig config;

    // 初始化 IO Context
    auto io = std::make_unique<io::IoContext>(config.io_threads);
    log::init_default_io_context(*io);
    std::cout << "[IO] IO Context 已初始化" << std::endl;

    // 启动 IO Context 线程
    std::vector<std::thread> io_threads;
    for (int i = 0; i < config.io_threads; ++i) {
        io_threads.emplace_back([&io]() {
            io->run();
        });
    }
    std::cout << "[IO] IO Context 线程已启动 (" << config.io_threads << " 线程)" << std::endl;
    std::cout << std::endl;

    // 初始化引擎
    Engine engine;
    std::cout << "[引擎] 正在初始化..." << std::endl;
    engine.initialize();
    std::cout << "[引擎] 初始化成功" << std::endl;
    std::cout << std::endl;

    // 访问引擎上下文
    auto& context = engine.context();

    // 配置日志
    auto& logger = context.logger();
    logger.add_sink(std::make_shared<log::ConsoleSink>());
    RENDU_LOG_INFO("服务器正在启动...");

    // 创建 Actor 系统
    auto actor_system = std::make_unique<ActorSystem>(config.actor_threads);
    actor_system->start();
    std::cout << "[Actor] Actor 系统已启动 (" << config.actor_threads << " 线程)" << std::endl;

    // 创建 Server Actor
    auto server_actor = actor_system->create_actor(
        "server_actor",
        [&actor_system]() -> std::unique_ptr<Actor> {
            return std::make_unique<server::ServerActor>("server_actor", actor_system.get());
        }
    );
    std::cout << "[Actor] Server Actor 已创建" << std::endl;
    std::cout << std::endl;

    // 创建 TCP 接收器
    auto acceptor = std::make_unique<net::TcpAcceptor>(*io, config.port);
    std::cout << "[网络] 正在监听端口 " << config.port << "..." << std::endl;

    // 用户 ID 生成器
    std::atomic<int32_t> next_user_id{1};

    // 接受新连接的回调函数（递归调用以持续接受连接）
    std::function<void()> accept_handler;
    accept_handler = [&]() {
        acceptor->async_accept([&](std::shared_ptr<net::TcpSocket> socket, const boost::system::error_code& ec) {
            if (ec) {
                if (ec != boost::asio::error::operation_aborted) {
                    RENDU_LOG_ERROR("Accept failed: {}", ec.message());
                }
                return;
            }

            RENDU_LOG_INFO("新客户端连接");

            // 生成用户 ID
            int32_t user_id = next_user_id++;

            // 创建 Session Actor
            auto session = actor_system->create_actor(
                "session_" + std::to_string(user_id),
                [&socket, user_id, &server_actor, &actor_system]() -> std::unique_ptr<Rendu::Actor> {
                    auto actor = std::make_unique<server::SessionActor>(
                        "session_" + std::to_string(user_id),
                        socket,
                        user_id,
                        server_actor
                    );
                    actor->set_system(actor_system.get());
                    return actor;
                }
            );

            // 注册会话
            auto login_msg = std::make_shared<server::SessionLoginMessage>(user_id, "");
            actor_system->tell(server_actor, login_msg);

            RENDU_LOG_INFO("Session Actor 已创建: user_id={}", user_id);

            // 继续接受下一个连接
            if (ServerSignalHandler::instance().is_running()) {
                accept_handler();
            }
        });
    };

    // 开始接受连接
    accept_handler();

    // 启动引擎
    engine.start();
    std::cout << "[引擎] 引擎已启动" << std::endl;
    std::cout << std::endl;
    std::cout << "=== 服务器运行中 ===" << std::endl;
    std::cout << "按 Ctrl+C 停止服务器" << std::endl;
    std::cout << std::endl;

    // 等待关闭信号
    ServerSignalHandler::instance().wait_for_shutdown();

    std::cout << std::endl;
    std::cout << "=== 停止服务器 ===" << std::endl;

    // 停止接收器
    acceptor->close();
    std::cout << "[网络] 接收器已停止" << std::endl;

    // 停止引擎
    engine.stop();
    std::cout << "[引擎] 引擎已停止" << std::endl;

    // 停止 Actor 系统
    actor_system->stop();
    std::cout << "[Actor] Actor 系统已停止" << std::endl;

    // 停止 IO Context
    io->stop();
    for (auto& thread : io_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    std::cout << "[IO] IO Context 线程已停止" << std::endl;

    std::cout << std::endl;
    std::cout << "========== 服务器已关闭 ==========" << std::endl;

    return 0;
}
