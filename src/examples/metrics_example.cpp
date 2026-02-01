//**********************************
//  Metrics 示例程序
//  Created by boil on 2026/02/01.
//**********************************

#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <random>
#include "common/metrics/metrics_collector.h"

using namespace rendu::metrics;

void counter_example() {
    std::cout << "=== Counter 示例 ===" << std::endl;

    MetricsCollector& collector = MetricsCollector::instance();
    collector.reset();

    // 基本计数
    collector.increment_counter("http_requests_total");
    collector.increment_counter("http_requests_total", 5.0);

    // 带标签的计数
    collector.increment_counter("http_requests_total", 1.0, {{"method", "GET"}});
    collector.increment_counter("http_requests_total", 1.0, {{"method", "POST"}});

    std::cout << "GET 请求: " << collector.get_counter("http_requests_total", {{"method", "GET"}}) << std::endl;
    std::cout << "POST 请求: " << collector.get_counter("http_requests_total", {{"method", "POST"}}) << std::endl;
    std::cout << "总请求数: " << collector.get_counter("http_requests_total") << std::endl;
    std::cout << std::endl;
}

void gauge_example() {
    std::cout << "=== Gauge 示例 ===" << std::endl;

    MetricsCollector& collector = MetricsCollector::instance();
    collector.reset();

    // 记录瞬时值
    collector.record_gauge("memory_usage_mb", 256.0);
    std::cout << "内存使用: " << collector.get_gauge("memory_usage_mb") << " MB" << std::endl;

    // 模拟内存变化
    for (int i = 0; i < 5; ++i) {
        collector.record_gauge("memory_usage_mb", 256.0 + i * 10);
        std::cout << "内存使用: " << collector.get_gauge("memory_usage_mb") << " MB" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << std::endl;
}

void histogram_example() {
    std::cout << "=== Histogram 示例 ===" << std::endl;

    MetricsCollector& collector = MetricsCollector::instance();
    collector.reset();

    // 模拟请求延迟
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> dist(50.0, 20.0);

    std::cout << "记录 100 个请求延迟..." << std::endl;
    for (int i = 0; i < 100; ++i) {
        double latency = std::max(1.0, dist(gen));
        collector.record_histogram("request_duration_ms", latency);
    }

    auto stats = collector.get_histogram_stats("request_duration_ms");
    std::cout << "总请求数: " << stats["count"] << std::endl;
    std::cout << "总耗时: " << stats["sum"] << " ms" << std::endl;
    std::cout << "平均延迟: " << stats["sum"] / stats["count"] << " ms" << std::endl;
    std::cout << std::endl;
}

void summary_example() {
    std::cout << "=== Summary 示例 ===" << std::endl;

    MetricsCollector& collector = MetricsCollector::instance();
    collector.reset();

    // 模拟响应大小
    std::vector<double> sizes = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};

    for (auto size : sizes) {
        collector.record_summary("response_size_bytes", size);
    }

    std::cout << "P50 (中位数): " << collector.get_summary_quantile("response_size_bytes", 0.5) << " bytes" << std::endl;
    std::cout << "P95: " << collector.get_summary_quantile("response_size_bytes", 0.95) << " bytes" << std::endl;
    std::cout << "P99: " << collector.get_summary_quantile("response_size_bytes", 0.99) << " bytes" << std::endl;
    std::cout << std::endl;
}

void timing_example() {
    std::cout << "=== 计时功能示例 ===" << std::endl;

    MetricsCollector& collector = MetricsCollector::instance();
    collector.reset();

    // 计时 void 函数
    collector.time("sleep_duration", []() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    });

    // 计时有返回值的函数
    int result = collector.time("compute_duration", []() {
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        return 42;
    });

    std::cout << "计算结果: " << result << std::endl;

    auto stats = collector.get_histogram_stats("sleep_duration");
    std::cout << "睡眠耗时: " << stats["sum"] << " ms" << std::endl;

    stats = collector.get_histogram_stats("compute_duration");
    std::cout << "计算耗时: " << stats["sum"] << " ms" << std::endl;
    std::cout << std::endl;
}

void export_example() {
    std::cout << "=== 导出示例 ===" << std::endl;

    MetricsCollector& collector = MetricsCollector::instance();

    // 添加一些测试数据
    collector.increment_counter("test_counter", 10.0);
    collector.record_gauge("test_gauge", 42.0);
    collector.record_histogram("test_histogram", 25.0);

    // 导出 Prometheus 格式
    std::cout << "--- Prometheus 格式 ---" << std::endl;
    std::string prometheus = collector.export_metrics("prometheus");
    std::cout << prometheus.substr(0, 500) << "..." << std::endl; // 只显示前 500 字符
    std::cout << std::endl;

    // 导出纯文本格式
    std::cout << "--- 纯文本格式 ---" << std::endl;
    std::string plain = collector.export_metrics("plain");
    std::cout << plain << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  RenduCore Metrics 模块示例" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;

    counter_example();
    gauge_example();
    histogram_example();
    summary_example();
    timing_example();
    export_example();

    std::cout << "========================================" << std::endl;
    std::cout << "  示例运行完成" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
