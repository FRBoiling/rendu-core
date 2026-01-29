#include "performance_test.h"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <numeric>

using namespace Rendu;
using namespace Rendu::net;
using namespace Rendu::client;

PerformanceTest::PerformanceTest(const TestConfig& config) : config_(config) {}

void PerformanceTest::run() {
    std::cout << "========================================" << std::endl;
    std::cout << "      服务器性能测试" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "配置:" << std::endl;
    std::cout << "  客户端数量: " << config_.num_clients << std::endl;
    std::cout << "  测试时长: " << config_.test_duration_sec << " 秒" << std::endl;
    std::cout << "  聊天间隔: " << config_.chat_interval_ms << " 毫秒" << std::endl;
    std::cout << "  服务器: " << config_.server_host << ":" << config_.server_port << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    // 初始化 IO Context
    io_ = std::make_unique<io::IoContext>(4);
    log::init_default_io_context(*io_);

    // 启动 IO 线程
    std::vector<std::thread> io_threads;
    for (int i = 0; i < 4; ++i) {
        io_threads.emplace_back([this]() { io_->run(); });
    }

    // 连接所有客户端
    std::cout << "[步骤 1] 连接客户端..." << std::endl;
    connect_clients();

    // 等待连接完成
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // 开始性能测试
    std::cout << std::endl;
    std::cout << "[步骤 2] 开始性能测试 (" << config_.test_duration_sec << " 秒)..." << std::endl;
    auto start_time = std::chrono::steady_clock::now();

    start_chat_loop();

    // 等待测试完成
    std::this_thread::sleep_for(std::chrono::seconds(config_.test_duration_sec));

    // 停止聊天循环
    stop_chat_loop();

    auto end_time = std::chrono::steady_clock::now();

    // 收集结果 (在断开连接之前收集统计信息)
    std::cout << std::endl;
    std::cout << "[步骤 3] 收集测试结果..." << std::endl;
    print_results();

    // 断开客户端连接
    disconnect_clients();

    // 等待断开完成
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 停止 IO
    io_->stop();
    for (auto& t : io_threads) {
        if (t.joinable()) {
            t.join();
        }
    }
}

void PerformanceTest::connect_clients() {
    for (int i = 0; i < config_.num_clients; ++i) {
        auto client = std::make_shared<TestClient>(i + 1, *io_);

        client->set_message_callback([i](const protocol::ServerMessage& msg) {
            if (msg.has_chat()) {
                std::cout << "\r  收到消息: " << msg.chat().content() << std::flush;
            }
        });

        client->set_error_callback([i](const std::string& error) {
            std::cerr << "\r  Client " << i << " 错误: " << error << std::flush;
        });

        clients_.push_back(client);

        client->connect(config_.server_host, config_.server_port,
            [this, client, i]() {
                // 连接成功后登录
                client->login("user_" + std::to_string(i));
                std::cout << "  Client " << i << " 已连接" << std::endl;
            }
        );

        // 避免连接过快,根据客户端数量调整连接间隔
        if (config_.num_clients > 500) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

void PerformanceTest::disconnect_clients() {
    for (auto& client : clients_) {
        client->disconnect();
    }
    clients_.clear();
}

void PerformanceTest::start_chat_loop() {
    for (auto& client : clients_) {
        client->start_chat_loop(config_.chat_interval_ms,
            "Performance test message from client " +
            std::to_string(client->client_id()));
    }
}

void PerformanceTest::stop_chat_loop() {
    for (auto& client : clients_) {
        client->stop_chat_loop();
    }
}

void PerformanceTest::print_results() {
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "      测试结果" << std::endl;
    std::cout << "========================================" << std::endl;

    TestResult result;
    result.total_clients = static_cast<int>(clients_.size());
    result.successful_connections = 0;
    result.failed_connections = 0;
    result.total_messages_sent = 0;
    result.total_messages_received = 0;
    result.total_bytes_sent = 0;
    result.total_bytes_received = 0;

    std::cout << "DEBUG: clients_.size() = " << clients_.size() << std::endl;

    // 在断开连接前收集统计信息
    for (const auto& client : clients_) {
        // 如果发送过消息,说明连接成功过
        if (client->stats().messages_sent.load() > 0) {
            result.successful_connections++;
        } else {
            result.failed_connections++;
        }

        result.total_messages_sent += client->stats().messages_sent.load();
        result.total_messages_received += client->stats().messages_received.load();
        result.total_bytes_sent += client->stats().bytes_sent.load();
        result.total_bytes_received += client->stats().bytes_received.load();
    }

    std::cout << "连接状态:" << std::endl;
    std::cout << "  成功连接: " << result.successful_connections << "/" << result.total_clients << std::endl;
    std::cout << "  失败连接: " << result.failed_connections << std::endl;
    std::cout << std::endl;

    std::cout << "消息统计:" << std::endl;
    std::cout << "  发送消息数: " << result.total_messages_sent << std::endl;
    std::cout << "  接收消息数: " << result.total_messages_received << std::endl;
    std::cout << "  发送字节数: " << format_bytes(result.total_bytes_sent) << std::endl;
    std::cout << "  接收字节数: " << format_bytes(result.total_bytes_received) << std::endl;
    std::cout << std::endl;

    std::cout << "性能指标:" << std::endl;
    if (config_.test_duration_sec > 0) {
        std::cout << "  吞吐量: " << std::fixed << std::setprecision(2)
            << (result.total_messages_sent / static_cast<double>(config_.test_duration_sec))
            << " 消息/秒" << std::endl;
        std::cout << "  带宽: " << format_bytes(
            static_cast<uint64_t>(result.total_bytes_sent / config_.test_duration_sec))
            << "/秒" << std::endl;
    }
    std::cout << "========================================" << std::endl;
}

std::string PerformanceTest::format_bytes(uint64_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB"};
    int unit_index = 0;
    double size = static_cast<double>(bytes);

    while (size >= 1024.0 && unit_index < 3) {
        size /= 1024.0;
        unit_index++;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << size << " " << units[unit_index];
    return oss.str();
}
