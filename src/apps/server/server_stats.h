#pragma once

#include <atomic>
#include <chrono>
#include <string>
#include <mutex>
#include <sstream>
#include <iomanip>

namespace server {

/// 服务器性能统计类
class ServerStats {
public:
    ServerStats() = default;

    // 连接统计
    void increment_total_connections() { ++total_connections_; }
    void increment_active_connections() { ++active_connections_; }
    void decrement_active_connections() { --active_connections_; }

    int64_t total_connections() const { return total_connections_.load(); }
    int64_t active_connections() const { return active_connections_.load(); }

    // 消息统计
    void increment_messages_received() { ++messages_received_; }
    void increment_messages_sent() { ++messages_sent_; }
    void increment_bytes_received(uint64_t bytes) { bytes_received_ += bytes; }
    void increment_bytes_sent(uint64_t bytes) { bytes_sent_ += bytes; }

    int64_t messages_received() const { return messages_received_.load(); }
    int64_t messages_sent() const { return messages_sent_.load(); }
    int64_t bytes_received() const { return bytes_received_.load(); }
    int64_t bytes_sent() const { return bytes_sent_.load(); }

    // 错误统计
    void increment_connection_errors() { ++connection_errors_; }
    void increment_parse_errors() { ++parse_errors_; }
    void increment_send_errors() { ++send_errors_; }

    int64_t connection_errors() const { return connection_errors_.load(); }
    int64_t parse_errors() const { return parse_errors_.load(); }
    int64_t send_errors() const { return send_errors_.load(); }

    // 性能指标
    void update_avg_message_latency(uint64_t latency_ms) {
        if (latency_ms > 0) {
            // 简单的移动平均
            uint64_t current = avg_message_latency_.load();
            uint64_t new_avg = (current * 9 + latency_ms) / 10;  // 90% 历史值 + 10% 新值
            avg_message_latency_.store(new_avg);
        }
    }

    uint64_t avg_message_latency() const { return avg_message_latency_.load(); }

    // 服务器启动时间
    void mark_start_time() { start_time_ = std::chrono::steady_clock::now(); }

    std::chrono::seconds uptime() const {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(now - start_time_);
    }

    // 获取格式化的统计报告
    std::string get_report() const {
        std::lock_guard<std::mutex> lock(mutex_);

        auto uptime_secs = uptime().count();
        double msg_rate = uptime_secs > 0 ? messages_received() / static_cast<double>(uptime_secs) : 0.0;
        double throughput = uptime_secs > 0 ? bytes_received() / static_cast<double>(uptime_secs) : 0.0;

        std::ostringstream oss;
        oss << "========== 服务器性能统计 ==========\n";
        oss << "运行时间: " << uptime_secs << " 秒\n";
        oss << "\n";
        oss << "连接统计:\n";
        oss << "  总连接数: " << total_connections() << "\n";
        oss << "  活跃连接: " << active_connections() << "\n";
        oss << "\n";
        oss << "消息统计:\n";
        oss << "  接收消息: " << messages_received() << "\n";
        oss << "  发送消息: " << messages_sent() << "\n";
        oss << "  接收字节: " << format_bytes(bytes_received()) << "\n";
        oss << "  发送字节: " << format_bytes(bytes_sent()) << "\n";
        oss << "\n";
        oss << "性能指标:\n";
        oss << "  消息速率: " << std::fixed << std::setprecision(2) << msg_rate << " 消息/秒\n";
        oss << "  吞吐量: " << format_bytes(static_cast<uint64_t>(throughput)) << "/秒\n";
        oss << "  平均延迟: " << avg_message_latency() << " ms\n";
        oss << "\n";
        oss << "错误统计:\n";
        oss << "  连接错误: " << connection_errors() << "\n";
        oss << "  解析错误: " << parse_errors() << "\n";
        oss << "  发送错误: " << send_errors() << "\n";
        oss << "===================================";

        return oss.str();
    }

private:
    static std::string format_bytes(uint64_t bytes) {
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

private:
    // 连接统计
    std::atomic<int64_t> total_connections_{0};
    std::atomic<int64_t> active_connections_{0};

    // 消息统计
    std::atomic<int64_t> messages_received_{0};
    std::atomic<int64_t> messages_sent_{0};
    std::atomic<int64_t> bytes_received_{0};
    std::atomic<int64_t> bytes_sent_{0};

    // 错误统计
    std::atomic<int64_t> connection_errors_{0};
    std::atomic<int64_t> parse_errors_{0};
    std::atomic<int64_t> send_errors_{0};

    // 性能指标
    std::atomic<uint64_t> avg_message_latency_{0};

    // 时间戳
    std::chrono::steady_clock::time_point start_time_{std::chrono::steady_clock::now()};

    mutable std::mutex mutex_;
};

} // namespace server
