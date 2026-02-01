#include "common/metrics/metrics_collector.h"
#include <sstream>
#include <iomanip>

namespace rendu {
namespace metrics {

MetricsCollector& MetricsCollector::instance() {
    static MetricsCollector instance;
    return instance;
}

std::string MetricsCollector::make_key(const std::string& name,
                                       const std::map<std::string, std::string>& tags) const {
    if (tags.empty()) {
        return name;
    }
    return name + format_tags(tags);
}

std::string MetricsCollector::format_tags(const std::map<std::string, std::string>& tags) const {
    if (tags.empty()) {
        return "";
    }

    std::ostringstream oss;
    oss << "{";
    bool first = true;
    for (const auto& [key, value] : tags) {
        if (!first) {
            oss << ",";
        }
        oss << key << "=\"" << value << "\"";
        first = false;
    }
    oss << "}";
    return oss.str();
}

void MetricsCollector::increment_counter(const std::string& name, double delta,
                                        const std::map<std::string, std::string>& tags) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto key = make_key(name, tags);
    counters_[key].increment(delta);
}

double MetricsCollector::get_counter(const std::string& name,
                                    const std::map<std::string, std::string>& tags) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto key = make_key(name, tags);
    auto it = counters_.find(key);
    if (it != counters_.end()) {
        return it->second.value;
    }
    return 0.0;
}

void MetricsCollector::record_gauge(const std::string& name, double value,
                                   const std::map<std::string, std::string>& tags) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto key = make_key(name, tags);
    gauges_[key].set(value);
}

double MetricsCollector::get_gauge(const std::string& name,
                                  const std::map<std::string, std::string>& tags) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto key = make_key(name, tags);
    auto it = gauges_.find(key);
    if (it != gauges_.end()) {
        return it->second.value;
    }
    return 0.0;
}

void MetricsCollector::record_histogram(const std::string& name, double value,
                                       const std::map<std::string, std::string>& tags,
                                       const std::vector<double>& custom_buckets) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto key = make_key(name, tags);

    if (custom_buckets.empty()) {
        histograms_[key].observe(value);
    } else {
        // 如果有自定义桶，检查是否已存在，如果不存在则创建
        auto it = histograms_.find(key);
        if (it == histograms_.end()) {
            Histogram hist(custom_buckets);
            hist.observe(value);
            histograms_[key] = hist;
        } else {
            it->second.observe(value);
        }
    }
}

std::map<std::string, double> MetricsCollector::get_histogram_stats(
    const std::string& name,
    const std::map<std::string, std::string>& tags) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto key = make_key(name, tags);
    auto it = histograms_.find(key);
    if (it != histograms_.end()) {
        return {
            {"sum", it->second.sum},
            {"count", static_cast<double>(it->second.count)}
        };
    }
    return {};
}

void MetricsCollector::record_summary(const std::string& name, double value,
                                      const std::map<std::string, std::string>& tags) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto key = make_key(name, tags);
    summaries_[key].observe(value);
}

double MetricsCollector::get_summary_quantile(const std::string& name, double quantile,
                                             const std::map<std::string, std::string>& tags) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto key = make_key(name, tags);
    auto it = summaries_.find(key);
    if (it != summaries_.end()) {
        return it->second.quantile(quantile);
    }
    return 0.0;
}

std::string MetricsCollector::export_metrics(const std::string& format) const {
    if (format == "prometheus") {
        return export_prometheus();
    } else {
        return export_plain();
    }
}

std::string MetricsCollector::export_prometheus() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream oss;

    // 导出计数器
    for (const auto& [key, counter] : counters_) {
        // 提取指标名称（去掉标签部分）
        size_t brace_pos = key.find('{');
        std::string name = (brace_pos != std::string::npos) ? key.substr(0, brace_pos) : key;
        std::string labels = (brace_pos != std::string::npos) ? key.substr(brace_pos) : "";

        oss << "# HELP " << name << " Counter metric\n";
        oss << "# TYPE " << name << " counter\n";
        oss << name << labels << " " << counter.value << "\n\n";
    }

    // 导出测量值
    for (const auto& [key, gauge] : gauges_) {
        size_t brace_pos = key.find('{');
        std::string name = (brace_pos != std::string::npos) ? key.substr(0, brace_pos) : key;
        std::string labels = (brace_pos != std::string::npos) ? key.substr(brace_pos) : "";

        oss << "# HELP " << name << " Gauge metric\n";
        oss << "# TYPE " << name << " gauge\n";
        oss << name << labels << " " << gauge.value << "\n\n";
    }

    // 导出直方图
    for (const auto& [key, hist] : histograms_) {
        size_t brace_pos = key.find('{');
        std::string name = (brace_pos != std::string::npos) ? key.substr(0, brace_pos) : key;
        std::string labels = (brace_pos != std::string::npos) ? key.substr(brace_pos) : "";
        std::string labels_suffix = (brace_pos != std::string::npos) ? ",le" : "{le";

        oss << "# HELP " << name << " Histogram metric\n";
        oss << "# TYPE " << name << " histogram\n";

        // 导出分桶
        for (size_t i = 0; i < hist.buckets.size(); ++i) {
            double bucket = hist.buckets[i];
            std::string bucket_str = (std::isinf(bucket)) ? "+Inf" : std::to_string(bucket);
            oss << name << labels_suffix << "=\"" << bucket_str << "\"} " << i + 1 << "\n";
        }

        // 导出 sum 和 count
        oss << name << labels_suffix << "=\"+Inf\"} " << hist.count << "\n";
        oss << name << "_sum" << labels << " " << hist.sum << "\n";
        oss << name << "_count" << labels << " " << hist.count << "\n\n";
    }

    // 导出摘要
    for (const auto& [key, summary] : summaries_) {
        size_t brace_pos = key.find('{');
        std::string name = (brace_pos != std::string::npos) ? key.substr(0, brace_pos) : key;
        std::string labels = (brace_pos != std::string::npos) ? key.substr(brace_pos) : "";
        std::string labels_suffix = (brace_pos != std::string::npos) ? ",quantile" : "{quantile";

        oss << "# HELP " << name << " Summary metric\n";
        oss << "# TYPE " << name << " summary\n";

        // 导出常用分位数
        std::vector<double> quantiles = {0.5, 0.9, 0.95, 0.99};
        for (double q : quantiles) {
            oss << name << labels_suffix << "=\"" << q << "\"} "
                << summary.quantile(q) << "\n";
        }

        oss << name << "_sum" << labels << " " << 0.0 << "\n";
        oss << name << "_count" << labels << " " << summary.values.size() << "\n\n";
    }

    return oss.str();
}

std::string MetricsCollector::export_plain() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream oss;

    oss << "=== Counters ===\n";
    for (const auto& [key, counter] : counters_) {
        oss << key << ": " << counter.value << "\n";
    }

    oss << "\n=== Gauges ===\n";
    for (const auto& [key, gauge] : gauges_) {
        oss << key << ": " << gauge.value << "\n";
    }

    oss << "\n=== Histograms ===\n";
    for (const auto& [key, hist] : histograms_) {
        oss << key << ": count=" << hist.count << ", sum=" << hist.sum << "\n";
    }

    oss << "\n=== Summaries ===\n";
    for (const auto& [key, summary] : summaries_) {
        oss << key << ": count=" << summary.values.size()
            << ", p50=" << summary.quantile(0.5)
            << ", p95=" << summary.quantile(0.95)
            << ", p99=" << summary.quantile(0.99) << "\n";
    }

    return oss.str();
}

void MetricsCollector::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    counters_.clear();
    gauges_.clear();
    histograms_.clear();
    summaries_.clear();
}

std::vector<std::string> MetricsCollector::get_all_metric_names() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;

    // 收集所有指标名称（去标签）
    for (const auto& [key, _] : counters_) {
        size_t brace_pos = key.find('{');
        names.push_back((brace_pos != std::string::npos) ? key.substr(0, brace_pos) : key);
    }
    for (const auto& [key, _] : gauges_) {
        size_t brace_pos = key.find('{');
        names.push_back((brace_pos != std::string::npos) ? key.substr(0, brace_pos) : key);
    }
    for (const auto& [key, _] : histograms_) {
        size_t brace_pos = key.find('{');
        names.push_back((brace_pos != std::string::npos) ? key.substr(0, brace_pos) : key);
    }
    for (const auto& [key, _] : summaries_) {
        size_t brace_pos = key.find('{');
        names.push_back((brace_pos != std::string::npos) ? key.substr(0, brace_pos) : key);
    }

    // 去重
    std::sort(names.begin(), names.end());
    names.erase(std::unique(names.begin(), names.end()), names.end());

    return names;
}

} // namespace metrics
} // namespace rendu
