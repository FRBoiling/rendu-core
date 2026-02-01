#pragma once

#include <string>
#include <map>
#include <unordered_map>
#include <mutex>
#include <functional>
#include <chrono>
#include <vector>
#include <variant>
#include <cmath>

namespace rendu {
namespace metrics {

/**
 * @brief 计数器类型
 *
 * 只能增加，适合记录请求总数、错误数等累计指标
 */
struct Counter {
    double value{0.0};

    void increment(double delta = 1.0) {
        value += delta;
    }

    void reset() {
        value = 0.0;
    }
};

/**
 * @brief 测量值类型
 *
 * 可以增减，适合记录当前连接数、内存使用等瞬时值
 */
struct Gauge {
    double value{0.0};

    void set(double new_value) {
        value = new_value;
    }

    void increment(double delta = 1.0) {
        value += delta;
    }

    void decrement(double delta = 1.0) {
        value -= delta;
    }
};

/**
 * @brief 直方图类型
 *
 * 记录值分布，适合记录请求耗时、消息大小等
 */
struct Histogram {
    // 默认分桶边界 (毫秒)
    std::vector<double> default_buckets{
        0.001, 0.005, 0.01, 0.025, 0.05, 0.075, 0.1, 0.25, 0.5, 0.75, 1.0,
        2.5, 5.0, 7.5, 10.0, 25.0, 50.0, 75.0, 100.0, 250.0, 500.0, 750.0, 1000.0, std::numeric_limits<double>::infinity()
    };

    std::vector<double> buckets;
    double sum{0.0};
    uint64_t count{0};

    explicit Histogram(const std::vector<double>& bucket_bounds = {})
        : buckets(bucket_bounds.empty() ? default_buckets : bucket_bounds) {
        // 确保桶是排好序的
        std::sort(this->buckets.begin(), this->buckets.end());
    }

    void observe(double value) {
        sum += value;
        count++;

        // 找到合适的桶
        for (size_t i = 0; i < buckets.size(); ++i) {
            if (value <= buckets[i]) {
                // bucket_counts[i]++;
                break;
            }
        }
    }

    void reset() {
        sum = 0.0;
        count = 0;
        // bucket_counts.clear();
        // bucket_counts.resize(buckets.size(), 0);
    }
};

/**
 * @brief 摘要类型
 *
 * 计算分位数，适合记录 P95、P99 等
 */
struct Summary {
    std::vector<double> values;

    void observe(double value) {
        values.push_back(value);
    }

    // 计算分位数
    double quantile(double q) const {
        if (values.empty()) return 0.0;

        std::vector<double> sorted_values = values;
        std::sort(sorted_values.begin(), sorted_values.end());

        // 线性插值计算分位数
        double pos = q * (sorted_values.size() - 1);
        size_t lower = static_cast<size_t>(std::floor(pos));
        size_t upper = static_cast<size_t>(std::ceil(pos));

        if (lower == upper) {
            return sorted_values[lower];
        }

        double lower_val = sorted_values[lower];
        double upper_val = sorted_values[upper];
        return lower_val + (upper_val - lower_val) * (pos - lower);
    }

    void reset() {
        values.clear();
    }
};

/**
 * @brief 性能指标收集器
 *
 * 提供统一的指标收集接口，支持计数器、测量值、直方图等指标类型
 * 线程安全，可在多线程环境使用
 *
 * @example
 * MetricsCollector& collector = MetricsCollector::instance();
 * collector.increment_counter("http_requests_total", 1.0, {{"method", "GET"}});
 * collector.record_gauge("active_connections", 42, {{"server", "api"}});
 * collector.record_histogram("request_duration_ms", 23.5, {{"endpoint", "/api/users"}});
 */
class MetricsCollector {
public:
    /**
     * @brief 获取单例实例
     * @return MetricsCollector& 单例引用
     */
    static MetricsCollector& instance();

    // 禁止拷贝和移动
    MetricsCollector(const MetricsCollector&) = delete;
    MetricsCollector& operator=(const MetricsCollector&) = delete;
    MetricsCollector(MetricsCollector&&) = delete;
    MetricsCollector& operator=(MetricsCollector&&) = delete;

    /**
     * @brief 记录计数器
     * @param name 指标名称
     * @param delta 增量值
     * @param tags 标签（用于分类）
     */
    void increment_counter(const std::string& name, double delta = 1.0,
                          const std::map<std::string, std::string>& tags = {});

    /**
     * @brief 获取计数器值
     * @param name 指标名称
     * @param tags 标签
     * @return double 计数器值
     */
    double get_counter(const std::string& name,
                      const std::map<std::string, std::string>& tags = {}) const;

    /**
     * @brief 记录测量值
     * @param name 指标名称
     * @param value 测量值
     * @param tags 标签
     */
    void record_gauge(const std::string& name, double value,
                      const std::map<std::string, std::string>& tags = {});

    /**
     * @brief 获取测量值
     * @param name 指标名称
     * @param tags 标签
     * @return double 测量值
     */
    double get_gauge(const std::string& name,
                     const std::map<std::string, std::string>& tags = {}) const;

    /**
     * @brief 记录直方图
     * @param name 指标名称
     * @param value 观测值
     * @param tags 标签
     */
    void record_histogram(const std::string& name, double value,
                          const std::map<std::string, std::string>& tags = {},
                          const std::vector<double>& custom_buckets = {});

    /**
     * @brief 获取直方图统计
     * @param name 指标名称
     * @param tags 标签
     * @return std::map<std::string, double> 统计信息 (sum, count)
     */
    std::map<std::string, double> get_histogram_stats(
        const std::string& name,
        const std::map<std::string, std::string>& tags = {}) const;

    /**
     * @brief 记录摘要
     * @param name 指标名称
     * @param value 观测值
     * @param tags 标签
     */
    void record_summary(const std::string& name, double value,
                       const std::map<std::string, std::string>& tags = {});

    /**
     * @brief 获取摘要分位数
     * @param name 指标名称
     * @param quantile 分位数 (0.0-1.0)
     * @param tags 标签
     * @return double 分位数值
     */
    double get_summary_quantile(const std::string& name, double quantile,
                               const std::map<std::string, std::string>& tags = {}) const;

    /**
     * @brief 计时函数执行时间
     * @param name 指标名称
     * @param func 要计时的函数
     * @tparam F 函数类型
     * @return auto 函数返回值
     */
    template<typename F>
    auto time(const std::string& name, F&& func,
              const std::map<std::string, std::string>& tags = {}) {
        auto start = std::chrono::high_resolution_clock::now();

        if constexpr (std::is_void_v<decltype(func())>) {
            func();
            auto end = std::chrono::high_resolution_clock::now();
            auto duration_ms = std::chrono::duration<double, std::milli>(end - start).count();
            record_histogram(name, duration_ms, tags);
        } else {
            auto result = func();
            auto end = std::chrono::high_resolution_clock::now();
            auto duration_ms = std::chrono::duration<double, std::milli>(end - start).count();
            record_histogram(name, duration_ms, tags);
            return result;
        }
    }

    /**
     * @brief 导出所有指标为文本格式
     * @param format 格式 ("prometheus" 或 "plain")
     * @return std::string 格式化的指标文本
     */
    std::string export_metrics(const std::string& format = "prometheus") const;

    /**
     * @brief 清空所有指标
     */
    void reset();

    /**
     * @brief 获取所有指标名称
     * @return std::vector<std::string> 指标名称列表
     */
    std::vector<std::string> get_all_metric_names() const;

private:
    MetricsCollector() = default;
    ~MetricsCollector() = default;

    // 生成指标的唯一键
    std::string make_key(const std::string& name,
                         const std::map<std::string, std::string>& tags) const;

    // 格式化标签
    std::string format_tags(const std::map<std::string, std::string>& tags) const;

    // 导出 Prometheus 格式
    std::string export_prometheus() const;

    // 导出纯文本格式
    std::string export_plain() const;

    mutable std::mutex mutex_;

    // 指标存储: key -> metric
    // key 格式: "name{tag1=value1,tag2=value2}"
    std::unordered_map<std::string, Counter> counters_;
    std::unordered_map<std::string, Gauge> gauges_;
    std::unordered_map<std::string, Histogram> histograms_;
    std::unordered_map<std::string, Summary> summaries_;
};

} // namespace metrics
} // namespace rendu
