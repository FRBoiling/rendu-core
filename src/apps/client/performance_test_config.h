#pragma once

#include <string>
#include <cstdint>

BEGIN_NAMESPACE_COMMON
namespace client {

/**
 * @brief 性能测试配置
 */
struct TestConfig {
    int num_clients = 100;           // 客户端数量
    int test_duration_sec = 60;      // 测试持续时间（秒）
    int chat_interval_ms = 1000;     // 聊天发送间隔（毫秒）
    std::string server_host = "127.0.0.1";
    uint16_t server_port = 8080;
};

/**
 * @brief 性能测试结果
 */
struct TestResult {
    int total_clients;
    int successful_connections;
    int failed_connections;

    uint64_t total_messages_sent;
    uint64_t total_messages_received;
    uint64_t total_bytes_sent;
    uint64_t total_bytes_received;

    double avg_messages_per_second;
    double avg_bytes_per_second;
    double avg_latency_ms;

    int64_t total_test_time_ms;
};

} // namespace client
END_NAMESPACE_COMMON
