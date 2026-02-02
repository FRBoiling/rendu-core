#include "common/metrics/metrics_collector.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

BEGIN_NAMESPACE_COMMON
namespace metrics {

MetricsCollector& MetricsCollector::instance() {
    static MetricsCollector instance;
    return instance;
}

void MetricsCollector::set_metadata(const std::string& name,
                                     const std::string& help,
                                     const std::string& type) {
    std::lock_guard<std::mutex> lock(mutex_);
    metadata_[name] = MetricMetadata{ name, help, type, {} };
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

std::string MetricsCollector::extract_metric_name(const std::string& key) const {
    size_t brace_pos = key.find('{');
    return (brace_pos != std::string::npos) ? key.substr(0, brace_pos) : key;
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
        return it->second.get();
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
        return it->second.get();
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
        // 如果有自定义桶，使用 try_emplace 避免移动 mutex
        auto [it, inserted] = histograms_.try_emplace(key, custom_buckets);
        it->second.observe(value);
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
            {"sum", it->second.sum.load(std::memory_order_relaxed)},
            {"count", static_cast<double>(it->second.count.load(std::memory_order_relaxed))}
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
        std::string name = extract_metric_name(key);
        std::string labels = (key.find('{') != std::string::npos) ? key.substr(key.find('{')) : "";

        // 使用元数据（如果存在）
        auto meta_it = metadata_.find(name);
        if (meta_it != metadata_.end() && !meta_it->second.help.empty()) {
            oss << "# HELP " << name << " " << meta_it->second.help << "\n";
        }
        oss << "# TYPE " << name << " counter\n";
        oss << name << labels << " " << counter.get() << "\n\n";
    }

    // 导出测量值
    for (const auto& [key, gauge] : gauges_) {
        std::string name = extract_metric_name(key);
        std::string labels = (key.find('{') != std::string::npos) ? key.substr(key.find('{')) : "";

        auto meta_it = metadata_.find(name);
        if (meta_it != metadata_.end() && !meta_it->second.help.empty()) {
            oss << "# HELP " << name << " " << meta_it->second.help << "\n";
        }
        oss << "# TYPE " << name << " gauge\n";
        oss << name << labels << " " << gauge.get() << "\n\n";
    }

    // 导出直方图
    for (const auto& [key, hist] : histograms_) {
        std::string name = extract_metric_name(key);
        std::string labels = (key.find('{') != std::string::npos) ? key.substr(key.find('{')) : "";
        std::string labels_suffix = (key.find('{') != std::string::npos) ? ",le" : "{le";

        auto meta_it = metadata_.find(name);
        if (meta_it != metadata_.end() && !meta_it->second.help.empty()) {
            oss << "# HELP " << name << " " << meta_it->second.help << "\n";
        }
        oss << "# TYPE " << name << " histogram\n";

        // 导出分桶
        auto bucket_counts = hist.get_bucket_counts();
        uint64_t cumulative = 0;
        for (size_t i = 0; i < hist.buckets.size(); ++i) {
            double bucket = hist.buckets[i];
            std::string bucket_str = (std::isinf(bucket)) ? "+Inf" : std::to_string(bucket);
            cumulative += bucket_counts[i];
            oss << name << labels_suffix << "=\"" << bucket_str << "\"} " << cumulative << "\n";
        }

        // 导出 sum 和 count
        uint64_t count = hist.count.load(std::memory_order_relaxed);
        double sum = hist.sum.load(std::memory_order_relaxed);
        oss << name << labels_suffix << "=\"+Inf\"} " << count << "\n";
        oss << name << "_sum" << labels << " " << sum << "\n";
        oss << name << "_count" << labels << " " << count << "\n\n";
    }

    // 导出摘要
    for (const auto& [key, summary] : summaries_) {
        std::string name = extract_metric_name(key);
        std::string labels = (key.find('{') != std::string::npos) ? key.substr(key.find('{')) : "";
        std::string labels_suffix = (key.find('{') != std::string::npos) ? ",quantile" : "{quantile";

        auto meta_it = metadata_.find(name);
        if (meta_it != metadata_.end() && !meta_it->second.help.empty()) {
            oss << "# HELP " << name << " " << meta_it->second.help << "\n";
        }
        oss << "# TYPE " << name << " summary\n";

        // 导出常用分位数
        std::vector<double> quantiles = {0.5, 0.9, 0.95, 0.99};
        for (double q : quantiles) {
            oss << name << labels_suffix << "=\"" << q << "\"} "
                << summary.quantile(q) << "\n";
        }

        oss << name << "_sum" << labels << " " << 0.0 << "\n";
        oss << name << "_count" << labels << " " << summary.size() << "\n\n";
    }

    return oss.str();
}

std::string MetricsCollector::export_plain() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream oss;

    oss << "=== Counters ===\n";
    for (const auto& [key, counter] : counters_) {
        oss << key << ": " << counter.get() << "\n";
    }

    oss << "\n=== Gauges ===\n";
    for (const auto& [key, gauge] : gauges_) {
        oss << key << ": " << gauge.get() << "\n";
    }

    oss << "\n=== Histograms ===\n";
    for (const auto& [key, hist] : histograms_) {
        oss << key << ": count=" << hist.count.load(std::memory_order_relaxed)
            << ", sum=" << hist.sum.load(std::memory_order_relaxed) << "\n";
    }

    oss << "\n=== Summaries ===\n";
    for (const auto& [key, summary] : summaries_) {
        oss << key << ": count=" << summary.size()
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
    // 保留元数据
    // metadata_.clear();
}

std::vector<std::string> MetricsCollector::get_all_metric_names() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;

    // 收集所有指标名称（去标签）
    for (const auto& [key, _] : counters_) {
        names.push_back(extract_metric_name(key));
    }
    for (const auto& [key, _] : gauges_) {
        names.push_back(extract_metric_name(key));
    }
    for (const auto& [key, _] : histograms_) {
        names.push_back(extract_metric_name(key));
    }
    for (const auto& [key, _] : summaries_) {
        names.push_back(extract_metric_name(key));
    }

    // 去重并排序
    std::sort(names.begin(), names.end());
    names.erase(std::unique(names.begin(), names.end()), names.end());

    return names;
}

size_t MetricsCollector::get_total_metric_count() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return counters_.size() + gauges_.size() + histograms_.size() + summaries_.size();
}

} // namespace metrics
END_NAMESPACE_COMMON
