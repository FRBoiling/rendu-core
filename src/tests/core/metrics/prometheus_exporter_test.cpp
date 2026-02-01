#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <thread>
#include <chrono>
#include <vector>

#include "core/metrics/prometheus_exporter.h"
#include "common/metrics/metrics_collector.h"

using namespace rendu::metrics;

TEST_CASE("PrometheusExporter 基本构造", "[metrics][prometheus_exporter]") {
    MetricsCollector& collector = MetricsCollector::instance();

    SECTION("默认配置构造") {
        Rendu::PrometheusExporter::Config config;
        Rendu::PrometheusExporter exporter(collector, config);
        REQUIRE(exporter.get_address() == "0.0.0.0");
        REQUIRE(exporter.get_port() == 9090);
    }

    SECTION("自定义配置构造") {
        Rendu::PrometheusExporter::Config config;
        config.listen_address = "127.0.0.1";
        config.listen_port = 8080;
        Rendu::PrometheusExporter exporter(collector, config);
        REQUIRE(exporter.get_address() == "127.0.0.1");
        REQUIRE(exporter.get_port() == 8080);
    }
}

TEST_CASE("PrometheusExporter 启动停止", "[metrics][prometheus_exporter]") {
    MetricsCollector& collector = MetricsCollector::instance();
    Rendu::PrometheusExporter exporter(collector);

    SECTION("正常启动停止") {
        REQUIRE_NOTHROW(exporter.start());
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        REQUIRE_NOTHROW(exporter.stop());
    }

    SECTION("重复启动") {
        exporter.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        REQUIRE_NOTHROW(exporter.start());
        exporter.stop();
    }

    SECTION("重复停止") {
        exporter.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        exporter.stop();
        REQUIRE_NOTHROW(exporter.stop());
    }
}

TEST_CASE("PrometheusExporter 配合 MetricsCollector", "[metrics][prometheus_exporter]") {
    MetricsCollector& collector = MetricsCollector::instance();
    Rendu::PrometheusExporter exporter(collector);

    SECTION("导出 Counter 指标") {
        collector.increment_counter("test_counter", 1.0, {{"label", "value1"}});
        collector.increment_counter("test_counter", 2.0, {{"label", "value2"}});

        std::string metrics = collector.export_metrics("prometheus");
        REQUIRE_FALSE(metrics.empty());
        REQUIRE(metrics.find("test_counter") != std::string::npos);
    }

    SECTION("导出 Gauge 指标") {
        collector.record_gauge("test_gauge", 42.0, {{"node", "server1"}});
        collector.record_gauge("test_gauge", 56.0, {{"node", "server2"}});

        std::string metrics = collector.export_metrics("prometheus");
        REQUIRE_FALSE(metrics.empty());
        REQUIRE(metrics.find("test_gauge") != std::string::npos);
    }

    SECTION("导出 Histogram 指标") {
        collector.record_histogram("test_histogram", 23.5, {{"endpoint", "/api/users"}});
        collector.record_histogram("test_histogram", 45.2, {{"endpoint", "/api/users"}});

        std::string metrics = collector.export_metrics("prometheus");
        REQUIRE_FALSE(metrics.empty());
        REQUIRE(metrics.find("test_histogram") != std::string::npos);
    }
}

TEST_CASE("PrometheusExporter 多个指标类型混合导出", "[metrics][prometheus_exporter]") {
    MetricsCollector& collector = MetricsCollector::instance();
    Rendu::PrometheusExporter exporter(collector);

    SECTION("混合指标导出") {
        // 添加各种类型的指标
        collector.increment_counter("http_requests_total", 100, {{"method", "GET"}});
        collector.record_gauge("active_connections", 42, {{"server", "api"}});
        collector.record_histogram("request_duration_ms", 23.5, {{"endpoint", "/api"}});

        std::string metrics = collector.export_metrics("prometheus");

        // 验证所有指标都在导出结果中
        REQUIRE(metrics.find("http_requests_total") != std::string::npos);
        REQUIRE(metrics.find("active_connections") != std::string::npos);
        REQUIRE(metrics.find("request_duration_ms") != std::string::npos);
    }
}

TEST_CASE("PrometheusExporter 线程安全", "[metrics][prometheus_exporter]") {
    MetricsCollector& collector = MetricsCollector::instance();
    Rendu::PrometheusExporter exporter(collector);

    SECTION("并发导出指标") {
        const int thread_count = 5;
        const int updates_per_thread = 100;
        std::vector<std::thread> threads;

        for (int t = 0; t < thread_count; ++t) {
            threads.emplace_back([&collector, t, updates_per_thread]() {
                for (int i = 0; i < updates_per_thread; ++i) {
                    collector.increment_counter("concurrent_counter", 1.0,
                                            {{"thread", std::to_string(t)}});
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        std::string metrics = collector.export_metrics("prometheus");
        REQUIRE(metrics.find("concurrent_counter") != std::string::npos);
    }
}
