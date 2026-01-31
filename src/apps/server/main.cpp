#include "core/engine/engine.h"
#include "core/actor/actor.h"
#include "core/actor/actor_ref.h"
#include "common/net/socket.h"
#include "server_actor.h"
#include "session_actor.h"
#include "server_messages.h"
#include "server_stats.h"
#include "common/log/logger.h"
#include "common/log/file_sink.h"
#include "common/io/io_context.h"
#include <iostream>
#include <thread>
#include <memory>
#include <atomic>
#include <csignal>
#include <mutex>
#include <condition_variable>
#include <filesystem>

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
        RENDU_LOG_INFO("收到信号 {}, 正在关闭服务器...", signal);
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
    std::string log_level = "info";
    std::string log_file = "logs/server.log";
    bool enable_file_log = true;
    int stats_report_interval_sec = 60;  // 统计报告间隔（秒）
};

int main() {
    ServerConfig config;

    // 初始化 IO Context
    auto io = std::make_unique<io::IoContext>(config.io_threads);
    log::init_default_io_context(*io);
    RENDU_LOG_INFO("========== RenduCore 服务器启动 ==========");
    RENDU_LOG_INFO("[IO] IO Context 已初始化, 线程数: {}", config.io_threads);

    // 配置日志
    auto& logger = log::default_logger();
    logger.add_sink(std::make_shared<log::ConsoleSink>());

    // 配置文件日志
    if (config.enable_file_log) {
        try {
            // 创建日志目录
            std::filesystem::create_directories("logs");

            // 添加文件 sink，支持每日轮转
            auto file_sink = std::make_shared<log::FileSink>(config.log_file);
            file_sink->set_rotation(log::FileSink::Rotation::Daily);
            // logger.add_sink(file_sink);
            RENDU_LOG_INFO("[日志] 文件日志已启用, 路径: {}", config.log_file);
        } catch (const std::exception& e) {
            RENDU_LOG_ERROR("[日志] 初始化文件日志失败: {}", e.what());
        }
    }

    // 设置日志级别
    if (config.log_level == "trace") {
        logger.set_level(log::Level::Trace);
    } else if (config.log_level == "debug") {
        logger.set_level(log::Level::Debug);
    } else if (config.log_level == "info") {
        logger.set_level(log::Level::Info);
    } else if (config.log_level == "warn") {
        logger.set_level(log::Level::Warn);
    } else if (config.log_level == "error") {
        logger.set_level(log::Level::Error);
    }
    RENDU_LOG_INFO("[日志] 日志级别设置为: {}", config.log_level);

    // 启动 IO Context 线程
    std::vector<std::thread> io_threads;
    for (int i = 0; i < config.io_threads; ++i) {
        io_threads.emplace_back([&io]() {
            io->run();
        });
    }
    RENDU_LOG_INFO("[IO] IO Context 线程已启动 ({} 线程)", config.io_threads);

    // 初始化引擎
    Engine engine;
    RENDU_LOG_INFO("[引擎] 正在初始化...");
    engine.initialize();
    RENDU_LOG_INFO("[引擎] 初始化成功");

    // 初始化统计模块
    auto stats = std::make_shared<server::ServerStats>();
    stats->mark_start_time();
    RENDU_LOG_INFO("[统计] 性能统计模块已初始化");

    // 创建 Actor 系统
    auto actor_system = std::make_unique<ActorSystem>(config.actor_threads);
    actor_system->start();
    RENDU_LOG_INFO("[Actor] Actor 系统已启动 ({} 线程)", config.actor_threads);

    // 创建 Server Actor
    auto server_actor = actor_system->create_actor(
        "server_actor",
        [&actor_system, stats]() -> std::unique_ptr<Actor> {
            auto actor = std::make_unique<server::ServerActor>("server_actor", actor_system.get());
            actor->set_stats(stats);
            return actor;
        }
    );
    RENDU_LOG_INFO("[Actor] Server Actor 已创建");

    // 创建 TCP 接收器
    auto acceptor = std::make_unique<net::TcpAcceptor>(*io, config.port);
    RENDU_LOG_INFO("[网络] 正在监听端口 {}...", config.port);

    // 用户 ID 生成器
    std::atomic<int32_t> next_user_id{1};

    // 启动统计报告定时器
    std::thread stats_report_thread([config = config, stats]() mutable {
        while (ServerSignalHandler::instance().is_running()) {
            std::this_thread::sleep_for(std::chrono::seconds(config.stats_report_interval_sec));
            if (ServerSignalHandler::instance().is_running()) {
                std::string report = stats->get_report();
                log::default_logger().info(report);
            }
        }
    });
    stats_report_thread.detach();
    RENDU_LOG_INFO("[统计] 性能报告定时器已启动 (间隔: {} 秒)", config.stats_report_interval_sec);

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

            // 生成用户 ID
            int32_t user_id = next_user_id++;
            RENDU_LOG_INFO("[网络] 新客户端连接, 分配 user_id: {}", user_id);

            // 更新统计
            stats->increment_total_connections();
            stats->increment_active_connections();

            // 创建 Session Actor
            auto session = actor_system->create_actor(
                "session_" + std::to_string(user_id),
                [&socket, user_id, &server_actor, &actor_system, stats]() -> std::unique_ptr<Rendu::Actor> {
                    auto actor = std::make_unique<server::SessionActor>(
                        "session_" + std::to_string(user_id),
                        socket,
                        user_id,
                        server_actor
                    );
                    actor->set_system(actor_system.get());
                    actor->set_stats(stats);
                    return actor;
                }
            );

            // 注册会话
            auto login_msg = std::make_shared<server::SessionLoginMessage>(user_id, "");
            actor_system->tell(server_actor, login_msg);

            RENDU_LOG_DEBUG("[网络] Session Actor 已创建: user_id={}", user_id);

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
    RENDU_LOG_INFO("[引擎] 引擎已启动");
    RENDU_LOG_INFO("========== RenduCore 服务器运行中 ==========");
    RENDU_LOG_INFO("配置: 端口={}, IO线程={}, Actor线程={}",
                  config.port, config.io_threads, config.actor_threads);

    // 等待关闭信号
    ServerSignalHandler::instance().wait_for_shutdown();

    RENDU_LOG_INFO("========== RenduCore 服务器关闭 ==========");

    // 停止接收器
    acceptor->close();
    RENDU_LOG_INFO("[网络] 接收器已停止");

    // 停止引擎
    engine.stop();
    RENDU_LOG_INFO("[引擎] 引擎已停止");

    // 停止 Actor 系统
    actor_system->stop();
    RENDU_LOG_INFO("[Actor] Actor 系统已停止");

    // 停止 IO Context
    io->stop();
    for (auto& thread : io_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    RENDU_LOG_INFO("[IO] IO Context 线程已停止");

    RENDU_LOG_INFO("========== 服务器已关闭 ==========");

    return 0;
}
