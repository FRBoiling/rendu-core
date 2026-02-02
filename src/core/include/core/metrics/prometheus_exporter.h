#pragma once

#include <string>
#include <memory>
#include <atomic>

#include "common/metrics/metrics_collector.h"
#include "core/define.h"

BEGIN_NAMESPACE_CORE

/// Prometheus 指标导出器
class PrometheusExporter {
public:
    /// 配置
    struct Config {
        std::string listen_address{"0.0.0.0"};  // 监听地址
        uint16_t listen_port{9090};                // 监听端口
        std::string metrics_endpoint{"/metrics"};    // 指标端点路径
        size_t thread_pool_size{4};                  // 线程池大小
    };

    explicit PrometheusExporter(metrics::MetricsCollector& collector);
    PrometheusExporter(metrics::MetricsCollector& collector, const Config& config);
    ~PrometheusExporter();

    /// 启动 HTTP 服务器
    void start();

    /// 停止 HTTP 服务器
    void stop();

    /// 获取监听端口
    uint16_t get_port() const;

    /// 获取监听地址
    const std::string& get_address() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

END_NAMESPACE_CORE
