/**
 * @file health_checker_test.cpp
 * @brief 健康检查器测试
 */

#include <catch2/catch_test_macros.hpp>
#include "common/health/health_checker.h"
#include <thread>
#include <chrono>

using namespace rendu;

// ============================================================================
// 基础功能测试
// ============================================================================

TEST_CASE("HealthChecker 基础功能", "[health]") {
    HealthChecker checker;

    SECTION("初始状态：无检查项") {
        auto status = checker.check();
        REQUIRE(status.healthy);
        REQUIRE(status.checks.empty());
        REQUIRE(status.details.empty());
    }

    SECTION("注册并执行检查项") {
        checker.register_check("test", []() {
            return CheckResult(true, "Test passed");
        });

        auto status = checker.check();
        REQUIRE(status.healthy);
        REQUIRE(status.checks.size() == 1);
        REQUIRE(status.checks.at("test") == true);
        REQUIRE(status.details.at("test") == "Test passed");
    }

    SECTION("检查项失败时整体不健康") {
        checker.register_check("test", []() {
            return CheckResult(false, "Test failed");
        });

        auto status = checker.check();
        REQUIRE_FALSE(status.healthy);
        REQUIRE(status.checks.at("test") == false);
        REQUIRE(status.details.at("test") == "Test failed");
    }
}

// ============================================================================
// 多检查项测试
// ============================================================================

TEST_CASE("HealthChecker 多检查项", "[health]") {
    HealthChecker checker;

    SECTION("多个检查项全部通过") {
        checker.register_check("check1", []() {
            return CheckResult(true, "OK");
        });
        checker.register_check("check2", []() {
            return CheckResult(true, "OK");
        });
        checker.register_check("check3", []() {
            return CheckResult(true, "OK");
        });

        auto status = checker.check();
        REQUIRE(status.healthy);
        REQUIRE(status.checks.size() == 3);
        REQUIRE(status.checks.at("check1") == true);
        REQUIRE(status.checks.at("check2") == true);
        REQUIRE(status.checks.at("check3") == true);
    }

    SECTION("部分检查项失败") {
        checker.register_check("check1", []() {
            return CheckResult(true, "OK");
        });
        checker.register_check("check2", []() {
            return CheckResult(false, "Failed");
        });
        checker.register_check("check3", []() {
            return CheckResult(true, "OK");
        });

        auto status = checker.check();
        REQUIRE_FALSE(status.healthy);
        REQUIRE(status.checks.at("check1") == true);
        REQUIRE(status.checks.at("check2") == false);
        REQUIRE(status.checks.at("check3") == true);
    }
}

// ============================================================================
// 部分失败允许测试
// ============================================================================

TEST_CASE("HealthChecker 部分失败允许", "[health]") {
    HealthChecker checker;

    SECTION("默认不允许部分失败") {
        checker.register_check("check1", []() {
            return CheckResult(true, "OK");
        });
        checker.register_check("check2", []() {
            return CheckResult(false, "Failed");
        });

        auto status = checker.check();
        REQUIRE_FALSE(status.healthy);
    }

    SECTION("允许部分失败") {
        checker.set_allow_partial_failure(true);
        checker.register_check("check1", []() {
            return CheckResult(true, "OK");
        });
        checker.register_check("check2", []() {
            return CheckResult(false, "Failed");
        });

        auto status = checker.check();
        REQUIRE_FALSE(status.healthy);
        REQUIRE(status.status == "Partial failure: 1/2 checks failed");
    }

    SECTION("允许部分失败时全部通过") {
        checker.set_allow_partial_failure(true);
        checker.register_check("check1", []() {
            return CheckResult(true, "OK");
        });
        checker.register_check("check2", []() {
            return CheckResult(true, "OK");
        });

        auto status = checker.check();
        REQUIRE(status.healthy);
    }
}

// ============================================================================
// 异常处理测试
// ============================================================================

TEST_CASE("HealthChecker 异常处理", "[health]") {
    HealthChecker checker;

    SECTION("检查项抛出异常") {
        checker.register_check("exception", []() -> CheckResult {
            throw std::runtime_error("Test exception");
        });

        auto status = checker.check();
        REQUIRE_FALSE(status.healthy);
        REQUIRE(status.checks.at("exception") == false);
        REQUIRE(status.details.at("exception").find("Exception") != std::string::npos);
    }

    SECTION("混合正常和异常检查项") {
        checker.register_check("normal", []() {
            return CheckResult(true, "OK");
        });
        checker.register_check("exception", []() -> CheckResult {
            throw std::runtime_error("Test exception");
        });

        auto status = checker.check();
        REQUIRE_FALSE(status.healthy);
        REQUIRE(status.checks.at("normal") == true);
        REQUIRE(status.checks.at("exception") == false);
    }
}

// ============================================================================
// 单个检查项测试
// ============================================================================

TEST_CASE("HealthChecker 单个检查项检查", "[health]") {
    HealthChecker checker;

    SECTION("检查存在的检查项") {
        checker.register_check("test", []() {
            return CheckResult(true, "OK");
        });

        auto result = checker.check("test");
        REQUIRE(result.has_value());
        REQUIRE(result->healthy);
        REQUIRE(result->checks.size() == 1);
        REQUIRE(result->checks.at("test") == true);
    }

    SECTION("检查不存在的检查项") {
        auto result = checker.check("nonexistent");
        REQUIRE_FALSE(result.has_value());
    }

    SECTION("单个检查项失败") {
        checker.register_check("test", []() {
            return CheckResult(false, "Failed");
        });

        auto result = checker.check("test");
        REQUIRE(result.has_value());
        REQUIRE_FALSE(result->healthy);
        REQUIRE(result->checks.at("test") == false);
    }
}

// ============================================================================
// 注册/注销测试
// ============================================================================

TEST_CASE("HealthChecker 注册/注销", "[health]") {
    HealthChecker checker;

    SECTION("注册检查项") {
        checker.register_check("test", []() {
            return CheckResult(true, "OK");
        });

        REQUIRE(checker.check_count() == 1);

        auto names = checker.get_check_names();
        REQUIRE(names.size() == 1);
        REQUIRE(names[0] == "test");
    }

    SECTION("注销检查项") {
        checker.register_check("test", []() {
            return CheckResult(true, "OK");
        });

        bool removed = checker.unregister_check("test");
        REQUIRE(removed);
        REQUIRE(checker.check_count() == 0);

        removed = checker.unregister_check("nonexistent");
        REQUIRE_FALSE(removed);
    }

    SECTION("覆盖已存在的检查项") {
        checker.register_check("test", []() {
            return CheckResult(true, "First");
        });

        checker.register_check("test", []() {
            return CheckResult(false, "Second");
        });

        auto status = checker.check();
        REQUIRE_FALSE(status.healthy);
        REQUIRE(status.details.at("test") == "Second");
    }

    SECTION("清除所有检查项") {
        checker.register_check("test1", []() {
            return CheckResult(true, "OK");
        });
        checker.register_check("test2", []() {
            return CheckResult(true, "OK");
        });

        REQUIRE(checker.check_count() == 2);

        checker.clear();
        REQUIRE(checker.check_count() == 0);

        auto status = checker.check();
        REQUIRE(status.healthy);
        REQUIRE(status.checks.empty());
    }

    SECTION("注册空函数抛出异常") {
        REQUIRE_THROWS_AS(
            checker.register_check("null", nullptr),
            std::invalid_argument
        );
    }
}

// ============================================================================
// 输出格式测试
// ============================================================================

TEST_CASE("HealthStatus 输出格式", "[health]") {
    HealthChecker checker;

    SECTION("JSON 格式输出") {
        checker.register_check("test", []() {
            return CheckResult(true, "OK");
        });

        auto status = checker.check();
        std::string json = status.to_json();

        REQUIRE(json.find("\"healthy\": true") != std::string::npos);
        REQUIRE(json.find("\"test\": true") != std::string::npos);
        REQUIRE(json.find("\"OK\"") != std::string::npos);
    }

    SECTION("文本格式输出") {
        checker.register_check("test1", []() {
            return CheckResult(true, "OK");
        });
        checker.register_check("test2", []() {
            return CheckResult(false, "Failed");
        });

        auto status = checker.check();
        std::string text = status.to_text();

        REQUIRE(text.find("Health Status:") != std::string::npos);
        REQUIRE(text.find("[✓] test1") != std::string::npos);
        REQUIRE(text.find("[✗] test2") != std::string::npos);
        REQUIRE(text.find("- OK") != std::string::npos);
        REQUIRE(text.find("- Failed") != std::string::npos);
    }
}

// ============================================================================
// 内置检查函数测试
// ============================================================================

TEST_CASE("内置检查函数", "[health]") {
    SECTION("always_pass") {
        auto result = health_checks::always_pass("Test OK");
        REQUIRE(result.passed);
        REQUIRE(result.message == "Test OK");
    }

    SECTION("always_fail") {
        auto result = health_checks::always_fail("Test Failed");
        REQUIRE_FALSE(result.passed);
        REQUIRE(result.message == "Test Failed");
    }

    SECTION("check_latency") {
        auto start = std::chrono::steady_clock::now();
        auto result = health_checks::check_latency(10);
        auto end = std::chrono::steady_clock::now();

        REQUIRE(result.passed);
        REQUIRE(result.message.find("10 ms") != std::string::npos);
        REQUIRE(std::chrono::duration_cast<std::chrono::milliseconds>(
            end - start).count() >= 10);
    }

    SECTION("check_memory_usage") {
        // 检查 1GB 限制
        auto result = health_checks::check_memory_usage(1024 * 1024 * 1024);
        REQUIRE(result.passed);
        REQUIRE(result.message.find("MB") != std::string::npos);
    }

    SECTION("check_disk_usage - 有效路径") {
        auto result = health_checks::check_disk_usage("/", 99.0);
        REQUIRE(result.passed);
        REQUIRE(result.message.find("%") != std::string::npos);
    }

    SECTION("check_cpu_usage") {
        auto result = health_checks::check_cpu_usage(100.0);
        // 应该总是通过（因为未完全实现）
        REQUIRE(result.passed);
    }
}

// ============================================================================
// 线程安全测试
// ============================================================================

TEST_CASE("HealthChecker 线程安全", "[health]") {
    HealthChecker checker;

    SECTION("多线程注册检查项") {
        std::vector<std::thread> threads;
        for (int i = 0; i < 10; ++i) {
            threads.emplace_back([&checker, i]() {
                checker.register_check("check_" + std::to_string(i), []() {
                    return CheckResult(true, "OK");
                });
            });
        }

        for (auto& t : threads) {
            t.join();
        }

        REQUIRE(checker.check_count() == 10);
    }

    SECTION("多线程执行检查") {
        for (int i = 0; i < 5; ++i) {
            checker.register_check("check_" + std::to_string(i), []() {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                return CheckResult(true, "OK");
            });
        }

        std::vector<std::thread> threads;
        std::vector<HealthStatus> results(10);

        for (int i = 0; i < 10; ++i) {
            threads.emplace_back([&checker, &results, i]() {
                results[i] = checker.check();
            });
        }

        for (auto& t : threads) {
            t.join();
        }

        // 所有结果都应该健康
        for (const auto& status : results) {
            REQUIRE(status.healthy);
        }
    }
}

// ============================================================================
// 综合场景测试
// ============================================================================

TEST_CASE("综合场景测试", "[health]") {
    HealthChecker checker;

    SECTION("模拟真实监控系统") {
        // 注册多个检查项
        checker.register_check("memory", []() {
            return health_checks::check_memory_usage(1024 * 1024 * 1024); // 1GB
        });

        checker.register_check("disk", []() {
            return health_checks::check_disk_usage("/", 90.0);
        });

        checker.register_check("latency", []() {
            return health_checks::check_latency(5);
        });

        checker.register_check("cpu", []() {
            return health_checks::check_cpu_usage(80.0);
        });

        // 执行检查
        auto status = checker.check();

        REQUIRE(status.healthy);
        REQUIRE(status.checks.size() == 4);

        // 输出 JSON
        std::string json = status.to_json();
        REQUIRE(json.find("memory") != std::string::npos);
        REQUIRE(json.find("disk") != std::string::npos);
        REQUIRE(json.find("latency") != std::string::npos);
        REQUIRE(json.find("cpu") != std::string::npos);

        // 输出文本
        std::string text = status.to_text();
        REQUIRE(text.find("Health Status:") != std::string::npos);
    }

    SECTION("模拟系统降级场景") {
        checker.set_allow_partial_failure(true);

        checker.register_check("primary", []() {
            return CheckResult(false, "Primary service down");
        });

        checker.register_check("secondary", []() {
            return CheckResult(true, "Secondary service up");
        });

        auto status = checker.check();
        REQUIRE_FALSE(status.healthy);
        REQUIRE(status.status == "Partial failure: 1/2 checks failed");

        // 单独检查 secondary
        auto secondary = checker.check("secondary");
        REQUIRE(secondary.has_value());
        REQUIRE(secondary->healthy);
    }
}
