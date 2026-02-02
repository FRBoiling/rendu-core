#pragma once

#include "common/define.h"
#include <string>
#include <map>
#include <unordered_map>
#include <mutex>
#include <functional>
#include <chrono>
#include <vector>
#include <variant>
#include <cmath>
#include <atomic>
#include <sstream>
#include <iomanip>

BEGIN_NAMESPACE_COMMON
namespace metrics {

/**
 * @brief 计数器类型
 *
 * 只能增加，适合记录请求总数、错误数等累计指标
 * 使用原子操作保证线程安全
 */
struct Counter {
    std::atomic<double> value{0.0};

    void increment(double delta = 1.0) {
        value.fetch_add(delta, std::memory_order_relaxed);
    }

    double get() const {
        return value.load(std::memory_order_relaxed);
    }

    void reset() {
        value.store(0.0, std::memory_order_relaxed);
    }
};

/**
 * @brief 测量值类型
 *
 * 可以增减，适合记录当前连接数、内存使用等瞬时值
 * 使用原子操作保证线程安全
 */
struct Gauge {
    std::atomic<double> value{0.0};

    void set(double new_value) {
        value.store(new_value, std::memory_order_relaxed);
    }

    void increment(double delta = 1.0) {
        value.fetch_add(delta, std::memory_order_relaxed);
    }

    void decrement(double delta = 1.0) {
        value.fetch_sub(delta, std::memory_order_relaxed);
    }

    double get() const {
        return value.load(std::memory_order_relaxed);
    }

    void reset() {
        value.store(0.0, std::memory_order_relaxed);
    }
};

/**
 * @brief 直方图类型
 *
 * 记录值分布，适合记录请求耗时、消息大小等
 * 线程安全实现
 */
struct Histogram {
    // 默认分桶边界 (毫秒)
    static inline const std::vector<double> default_buckets{
        0.001, 0.005, 0.01, 0.025, 0.05, 0.075, 0.1, 0.25, 0.5, 0.75, 1.0,
        2.5, 5.0, 7.5, 10.0, 25.0, 50.0, 75.0, 100.0, 250.0, 500.0, 750.0, 1000.0,
        std::numeric_limits<double>::infinity()
    };

    std::vector<double> buckets;
    std::atomic<double> sum{0.0};
    std::atomic<uint64_t> count{0};
    mutable std::mutex bucket_mutex_;  // 保护桶计数
    std::vector<uint64_t> bucket_counts_;

    explicit Histogram(const std::vector<double>& bucket_bounds = {})
        : buckets(bucket_bounds.empty() ? default_buckets : bucket_bounds)
        , bucket_counts_(buckets.size(), 0) {
        std::sort(this->buckets.begin(), this->buckets.end());
    }

    void observe(double value) {
        sum.fetch_add(value, std::memory_order_relaxed);
        count.fetch_add(1, std::memory_order_relaxed);

        // 找到合适的桶并增加计数
        {
            std::lock_guard<std::mutex> lock(bucket_mutex_);
            for (size_t i = 0; i < buckets.size(); ++i) {
                if (value <= buckets[i]) {
                    bucket_counts_[i]++;
                    break;
                }
            }
        }
    }

    void reset() {
        sum.store(0.0, std::memory_order_relaxed);
        count.store(0, std::memory_order_relaxed);
        std::lock_guard<std::mutex> lock(bucket_mutex_);
        std::fill(bucket_counts_.begin(), bucket_counts_.end(), 0);
    }

    // 获取桶计数（用于导出）
    std::vector<uint64_t> get_bucket_counts() const {
        std::lock_guard<std::mutex> lock(bucket_mutex_);
        return bucket_counts_;
    }
};

/**
 * @brief 摘要类型
 *
 * 计算分位数，适合记录 P95、P99 等
 * 线程安全实现
 */
struct Summary {
    mutable std::mutex values_mutex_;
    std::vector<double> values;

    void observe(double value) {
        std::lock_guard<std::mutex> lock(values_mutex_);
        values.push_back(value);
    }

    double quantile(double q) const {
        std::lock_guard<std::mutex> lock(values_mutex_);
        if (values.empty()) return 0.0;

        std::vector<double> sorted_values = values;
        std::sort(sorted_values.begin(), sorted_values.end());

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
        std::lock_guard<std::mutex> lock(values_mutex_);
        values.clear();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(values_mutex_);
        return values.size();
    }
};

/**
 * @brief 指标元数据
 */
struct MetricMetadata {
    std::string name;
    std::string help;
    std::string type;  // "counter", "gauge", "histogram", "summary"
    std::map<std::string, std::string> labels;
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
     * @brief 设置指标元数据
     * @param name 指标名称
     * @param help 帮助信息
     * @param type 指标类型
     */
    void set_metadata(const std::string& name,
                    const std::string& help,
                    const std::string& type);

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
     * @param custom_buckets 自定义桶边界
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
     * @param tags 标签
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

    /**
     * @brief 获取指标总数
     * @return size_t 指标总数
     */
    size_t get_total_metric_count() const;

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

    // 获取指标名称（去掉标签）
    std::string extract_metric_name(const std::string& key) const;

    mutable std::mutex mutex_;

    // 指标存储: key -> metric
    // key 格式: "name{tag1=value1,tag2=value2}"
    std::unordered_map<std::string, Counter> counters_;
    std::unordered_map<std::string, Gauge> gauges_;
    std::unordered_map<std::string, Histogram> histograms_;
    std::unordered_map<std::string, Summary> summaries_;

    // 指标元数据: name -> metadata
    std::unordered_map<std::string, MetricMetadata> metadata_;
};

} // namespace metrics
END_NAMESPACE_COMMON
