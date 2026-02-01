#include <catch2/catch_test_macros.hpp>
#include "common/metrics/metrics_collector.h"
#include <thread>
#include <vector>
#include <chrono>

using namespace rendu::metrics;

TEST_CASE("MetricsCollector - 基本计数器功能", "[metrics][counter]") {
    MetricsCollector& collector = MetricsCollector::instance();
    collector.reset();

    SECTION("增加计数器") {
        collector.increment_counter("test_counter");
        REQUIRE(collector.get_counter("test_counter") == 1.0);

        collector.increment_counter("test_counter", 5.0);
        REQUIRE(collector.get_counter("test_counter") == 6.0);
    }

    SECTION("带标签的计数器") {
        collector.increment_counter("http_requests", 1.0, {{"method", "GET"}});
        collector.increment_counter("http_requests", 1.0, {{"method", "POST"}});

        REQUIRE(collector.get_counter("http_requests", {{"method", "GET"}}) == 1.0);
        REQUIRE(collector.get_counter("http_requests", {{"method", "POST"}}) == 1.0);

        collector.increment_counter("http_requests", 2.0, {{"method", "GET"}});
        REQUIRE(collector.get_counter("http_requests", {{"method", "GET"}}) == 3.0);
    }

    SECTION("获取不存在的计数器返回0") {
        REQUIRE(collector.get_counter("non_existent") == 0.0);
    }
}

TEST_CASE("MetricsCollector - 基本测量值功能", "[metrics][gauge]") {
    MetricsCollector& collector = MetricsCollector::instance();
    collector.reset();

    SECTION("设置测量值") {
        collector.record_gauge("temperature", 23.5);
        REQUIRE(collector.get_gauge("temperature") == 23.5);

        collector.record_gauge("temperature", 24.1);
        REQUIRE(collector.get_gauge("temperature") == 24.1);
    }

    SECTION("带标签的测量值") {
        collector.record_gauge("memory_usage", 1024.0, {{"host", "server1"}});
        collector.record_gauge("memory_usage", 512.0, {{"host", "server2"}});

        REQUIRE(collector.get_gauge("memory_usage", {{"host", "server1"}}) == 1024.0);
        REQUIRE(collector.get_gauge("memory_usage", {{"host", "server2"}}) == 512.0);
    }

    SECTION("获取不存在的测量值返回0") {
        REQUIRE(collector.get_gauge("non_existent") == 0.0);
    }
}

TEST_CASE("MetricsCollector - 基本直方图功能", "[metrics][histogram]") {
    MetricsCollector& collector = MetricsCollector::instance();
    collector.reset();

    SECTION("记录观测值") {
        collector.record_histogram("request_duration_ms", 10.0);
        collector.record_histogram("request_duration_ms", 20.0);
        collector.record_histogram("request_duration_ms", 30.0);

        auto stats = collector.get_histogram_stats("request_duration_ms");
        REQUIRE(stats["count"] == 3.0);
        REQUIRE(stats["sum"] == 60.0);
    }

    SECTION("带标签的直方图") {
        collector.record_histogram("request_duration_ms", 10.0, {{"endpoint", "/api/users"}});
        collector.record_histogram("request_duration_ms", 20.0, {{"endpoint", "/api/users"}});

        auto stats = collector.get_histogram_stats("request_duration_ms", {{"endpoint", "/api/users"}});
        REQUIRE(stats["count"] == 2.0);
        REQUIRE(stats["sum"] == 30.0);
    }

    SECTION("自定义桶边界") {
        std::vector<double> custom_buckets = {1.0, 5.0, 10.0, std::numeric_limits<double>::infinity()};
        collector.record_histogram("custom_histogram", 2.5, {}, custom_buckets);
        collector.record_histogram("custom_histogram", 7.5, {}, custom_buckets);

        auto stats = collector.get_histogram_stats("custom_histogram");
        REQUIRE(stats["count"] == 2.0);
        REQUIRE(stats["sum"] == 10.0);
    }
}

TEST_CASE("MetricsCollector - 基本摘要功能", "[metrics][summary]") {
    MetricsCollector& collector = MetricsCollector::instance();
    collector.reset();

    SECTION("记录观测值并计算分位数") {
        collector.record_summary("response_size", 100.0);
        collector.record_summary("response_size", 200.0);
        collector.record_summary("response_size", 300.0);
        collector.record_summary("response_size", 400.0);
        collector.record_summary("response_size", 500.0);

        REQUIRE(collector.get_summary_quantile("response_size", 0.5) == 300.0);
        REQUIRE(collector.get_summary_quantile("response_size", 0.95) == 480.0);
        REQUIRE(collector.get_summary_quantile("response_size", 0.99) == 496.0);
    }

    SECTION("带标签的摘要") {
        collector.record_summary("response_size", 100.0, {{"service", "api"}});
        collector.record_summary("response_size", 200.0, {{"service", "api"}});

        REQUIRE(collector.get_summary_quantile("response_size", 0.5, {{"service", "api"}}) == 150.0);
    }

    SECTION("获取不存在的摘要分位数返回0") {
        REQUIRE(collector.get_summary_quantile("non_existent", 0.5) == 0.0);
    }
}

TEST_CASE("MetricsCollector - 计时功能", "[metrics][timing]") {
    MetricsCollector& collector = MetricsCollector::instance();
    collector.reset();

    SECTION("计时 void 函数") {
        collector.time("test_duration", []() {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        });

        auto stats = collector.get_histogram_stats("test_duration");
        REQUIRE(stats["count"] == 1.0);
        REQUIRE(stats["sum"] >= 10.0); // 至少 10ms
    }

    SECTION("计时有返回值的函数") {
        int result = collector.time("test_duration", []() {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            return 42;
        });

        REQUIRE(result == 42);

        auto stats = collector.get_histogram_stats("test_duration");
        REQUIRE(stats["count"] == 1.0);
        REQUIRE(stats["sum"] >= 10.0);
    }
}

TEST_CASE("MetricsCollector - 导出 Prometheus 格式", "[metrics][export]") {
    MetricsCollector& collector = MetricsCollector::instance();
    collector.reset();

    SECTION("导出计数器") {
        collector.increment_counter("test_counter", 5.0);

        std::string exported = collector.export_metrics("prometheus");
        REQUIRE(exported.find("# TYPE test_counter counter") != std::string::npos);
        REQUIRE(exported.find("test_counter 5") != std::string::npos);
    }

    SECTION("导出带标签的指标") {
        collector.record_gauge("temperature", 23.5, {{"location", "room1"}});

        std::string exported = collector.export_metrics("prometheus");
        REQUIRE(exported.find("# TYPE temperature gauge") != std::string::npos);
        REQUIRE(exported.find("temperature{location=\"room1\"} 23.5") != std::string::npos);
    }

    SECTION("导出直方图") {
        collector.record_histogram("request_duration_ms", 10.0);
        collector.record_histogram("request_duration_ms", 20.0);

        std::string exported = collector.export_metrics("prometheus");
        REQUIRE(exported.find("# TYPE request_duration_ms histogram") != std::string::npos);
        REQUIRE(exported.find("request_duration_ms_sum") != std::string::npos);
        REQUIRE(exported.find("request_duration_ms_count") != std::string::npos);
    }
}

TEST_CASE("MetricsCollector - 导出纯文本格式", "[metrics][export]") {
    MetricsCollector& collector = MetricsCollector::instance();
    collector.reset();

    SECTION("导出所有指标") {
        collector.increment_counter("test_counter", 5.0);
        collector.record_gauge("temperature", 23.5);

        std::string exported = collector.export_metrics("plain");
        REQUIRE(exported.find("=== Counters ===") != std::string::npos);
        REQUIRE(exported.find("test_counter: 5") != std::string::npos);
        REQUIRE(exported.find("=== Gauges ===") != std::string::npos);
        REQUIRE(exported.find("temperature: 23.5") != std::string::npos);
    }
}

TEST_CASE("MetricsCollector - 重置功能", "[metrics][reset]") {
    MetricsCollector& collector = MetricsCollector::instance();

    SECTION("重置后所有指标清空") {
        collector.increment_counter("test_counter", 10.0);
        collector.record_gauge("temperature", 23.5);
        collector.record_histogram("request_duration_ms", 10.0);

        REQUIRE(collector.get_counter("test_counter") == 10.0);
        REQUIRE(collector.get_gauge("temperature") == 23.5);

        collector.reset();

        REQUIRE(collector.get_counter("test_counter") == 0.0);
        REQUIRE(collector.get_gauge("temperature") == 0.0);

        auto stats = collector.get_histogram_stats("request_duration_ms");
        REQUIRE(stats.empty());
    }
}

TEST_CASE("MetricsCollector - 获取所有指标名称", "[metrics][names]") {
    MetricsCollector& collector = MetricsCollector::instance();
    collector.reset();

    SECTION("获取所有指标名称") {
        collector.increment_counter("counter1", 1.0);
        collector.increment_counter("counter2", 1.0);
        collector.record_gauge("gauge1", 10.0);
        collector.record_histogram("hist1", 5.0);

        auto names = collector.get_all_metric_names();
        REQUIRE(names.size() == 4);
        REQUIRE(std::find(names.begin(), names.end(), "counter1") != names.end());
        REQUIRE(std::find(names.begin(), names.end(), "counter2") != names.end());
        REQUIRE(std::find(names.begin(), names.end(), "gauge1") != names.end());
        REQUIRE(std::find(names.begin(), names.end(), "hist1") != names.end());
    }

    SECTION("去重标签的指标名称") {
        collector.increment_counter("requests", 1.0, {{"method", "GET"}});
        collector.increment_counter("requests", 1.0, {{"method", "POST"}});

        auto names = collector.get_all_metric_names();
        REQUIRE(std::count(names.begin(), names.end(), "requests") == 1);
    }
}

TEST_CASE("MetricsCollector - 多线程安全性", "[metrics][threading]") {
    MetricsCollector& collector = MetricsCollector::instance();
    collector.reset();

    SECTION("多线程增加计数器") {
        const int num_threads = 10;
        const int increments_per_thread = 100;
        std::vector<std::thread> threads;

        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back([&collector, increments_per_thread]() {
                for (int j = 0; j < increments_per_thread; ++j) {
                    collector.increment_counter("thread_test_counter");
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        REQUIRE(collector.get_counter("thread_test_counter") == num_threads * increments_per_thread);
    }

    SECTION("多线程记录测量值") {
        const int num_threads = 10;
        std::vector<std::thread> threads;

        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back([&collector, i]() {
                collector.record_gauge("thread_test_gauge", i * 10.0);
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        // 最终值应该是最后一次设置的
        double final_value = collector.get_gauge("thread_test_gauge");
        REQUIRE(final_value >= 0.0);
        REQUIRE(final_value <= 90.0);
    }
}

TEST_CASE("MetricsCollector - 边界情况测试", "[metrics][edge]") {
    MetricsCollector& collector = MetricsCollector::instance();
    collector.reset();

    SECTION("零值和负值") {
        collector.increment_counter("zero_counter", 0.0);
        REQUIRE(collector.get_counter("zero_counter") == 0.0);

        collector.record_gauge("negative_gauge", -10.0);
        REQUIRE(collector.get_gauge("negative_gauge") == -10.0);
    }

    SECTION("空标签") {
        collector.increment_counter("no_tags", 1.0, {});
        REQUIRE(collector.get_counter("no_tags", {}) == 1.0);
    }

    SECTION("多个标签") {
        collector.record_gauge("multi_tags", 100.0,
            {{"host", "server1"}, {"region", "us-east"}, {"env", "prod"}});

        REQUIRE(collector.get_gauge("multi_tags",
            {{"host", "server1"}, {"region", "us-east"}, {"env", "prod"}}) == 100.0);
    }

    SECTION("非常大的值") {
        collector.increment_counter("large_counter", 1e10);
        REQUIRE(collector.get_counter("large_counter") == 1e10);

        collector.record_gauge("large_gauge", 1e20);
        REQUIRE(collector.get_gauge("large_gauge") == 1e20);
    }
}
