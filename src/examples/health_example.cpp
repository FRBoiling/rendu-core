/**
 * @file health_example.cpp
 * @brief 健康检查器使用示例
 */

#include "common/health/health_checker.h"
#include "common/log/logger.h"
#include "common/log/sink.h"
#include "common/io/io_context.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace rendu;

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "      RenduCore 健康检查器示例" << std::endl;
    std::cout << "========================================" << std::endl;

    // 示例1：基础健康检查
    std::cout << "\n=== 示例1：基础健康检查 ===" << std::endl;

    HealthChecker checker;
    checker.register_check("database", []() {
        return CheckResult(true, "Database connection OK");
    });
    checker.register_check("cache", []() {
        return CheckResult(true, "Cache server responsive");
    });
    checker.register_check("api", []() {
        return CheckResult(true, "API endpoint available");
    });

    auto status = checker.check();
    std::cout << "整体状态: " << (status.healthy ? "健康" : "不健康") << std::endl;
    std::cout << "状态描述: " << status.status << std::endl;
    std::cout << "详细结果:" << std::endl;
    for (const auto& [name, passed] : status.checks) {
        std::cout << "  " << (passed ? "[✓]" : "[✗]") << " " << name << ": " << status.details.at(name) << std::endl;
    }

    // 示例2：使用内置检查函数
    std::cout << "\n=== 示例2：使用内置检查函数 ===" << std::endl;
    HealthChecker checker2;
    checker2.register_check("memory", []() {
        return health_checks::check_memory_usage(1024 * 1024 * 1024);
    });
    checker2.register_check("disk", []() {
        return health_checks::check_disk_usage("/", 90.0);
    });
    checker2.register_check("cpu", []() {
        return health_checks::check_cpu_usage(80.0);
    });

    auto status2 = checker2.check();
    std::cout << "系统资源检查:" << std::endl;
    for (const auto& [name, passed] : status2.checks) {
        std::cout << "  " << (passed ? "[✓]" : "[✗]") << " " << name << ": " << status2.details.at(name) << std::endl;
    }

    // 示例3：异常处理
    std::cout << "\n=== 示例3：异常处理 ===" << std::endl;
    HealthChecker checker3;
    checker3.register_check("normal", []() {
        return CheckResult(true, "This check is normal");
    });
    checker3.register_check("exception", []() -> CheckResult {
        throw std::runtime_error("Something went wrong!");
    });

    auto status3 = checker3.check();
    std::cout << "健康检查结果:" << std::endl;
    for (const auto& [name, passed] : status3.checks) {
        std::cout << "  " << (passed ? "[✓]" : "[✗]") << " " << name << ": " << status3.details.at(name) << std::endl;
    }

    // 示例4：部分失败允许
    std::cout << "\n=== 示例4：部分失败允许 ===" << std::endl;
    HealthChecker checker4;
    checker4.set_allow_partial_failure(true);
    checker4.register_check("primary_db", []() {
        return CheckResult(false, "Primary database is down");
    });
    checker4.register_check("secondary_db", []() {
        return CheckResult(true, "Secondary database is up");
    });
    checker4.register_check("cache", []() {
        return CheckResult(true, "Cache is operational");
    });

    auto status4 = checker4.check();
    std::cout << "系统状态: " << status4.status << std::endl;
    std::cout << "服务详情:" << std::endl;
    for (const auto& [name, passed] : status4.checks) {
        std::cout << "  " << (passed ? "[✓]" : "[✗]") << " " << name << ": " << status4.details.at(name) << std::endl;
    }

    // 示例5：输出格式
    std::cout << "\n=== 示例5：输出格式 ===" << std::endl;
    HealthChecker checker5;
    checker5.register_check("service1", []() {
        return CheckResult(true, "Running");
    });
    checker5.register_check("service2", []() {
        return CheckResult(false, "Stopped");
    });

    auto status5 = checker5.check();
    std::cout << "JSON 格式:" << std::endl;
    std::cout << status5.to_json() << std::endl;

    std::cout << "\n文本格式:" << std::endl;
    std::cout << status5.to_text() << std::endl;

    // 示例6：动态管理检查项
    std::cout << "\n=== 示例6：动态管理检查项 ===" << std::endl;
    HealthChecker checker6;
    checker6.register_check("check1", []() {
        return CheckResult(true, "Check 1");
    });

    std::cout << "初始检查项数量: " << checker6.check_count() << std::endl;

    checker6.register_check("check2", []() {
        return CheckResult(true, "Check 2");
    });

    std::cout << "添加后检查项数量: " << checker6.check_count() << std::endl;

    auto names = checker6.get_check_names();
    std::cout << "所有检查项名称: ";
    for (const auto& name : names) {
        std::cout << name << " ";
    }
    std::cout << std::endl;

    bool removed = checker6.unregister_check("check1");
    std::cout << "移除 check1: " << (removed ? "成功" : "失败") << std::endl;
    std::cout << "移除后检查项数量: " << checker6.check_count() << std::endl;

    checker6.clear();
    std::cout << "清除后检查项数量: " << checker6.check_count() << std::endl;

    // 示例7：模拟监控场景
    std::cout << "\n=== 示例7：模拟监控场景 ===" << std::endl;
    HealthChecker checker7;
    checker7.register_check("memory", []() {
        return health_checks::check_memory_usage(1024 * 1024 * 1024);
    });
    checker7.register_check("disk", []() {
        return health_checks::check_disk_usage("/", 90.0);
    });
    checker7.register_check("database", []() {
        static int counter = 0;
        counter++;
        if (counter % 5 == 0) {
            return CheckResult(false, "Database timeout");
        }
        return CheckResult(true, "Database OK");
    });
    checker7.register_check("api", []() {
        return health_checks::check_latency(10);
    });

    std::cout << "持续监控 3 秒..." << std::endl;
    for (int i = 0; i < 3; ++i) {
        auto status = checker7.check();
        std::cout << "\n第 " << (i + 1) << " 次检查:" << std::endl;
        std::cout << "  状态: " << (status.healthy ? "[✓] 健康" : "[✗] 不健康") << std::endl;
        std::cout << "  详情: " << status.status << std::endl;

        for (const auto& [name, passed] : status.checks) {
            if (!passed) {
                std::cout << "  ⚠️  " << name << ": " << status.details.at(name) << std::endl;
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // 示例8：自定义延迟检查
    std::cout << "\n=== 示例8：自定义延迟检查 ===" << std::endl;
    HealthChecker checker8;
    checker8.register_check("slow_service", []() {
        auto start = std::chrono::steady_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        if (duration.count() < 100) {
            return CheckResult(true, "Response time: " + std::to_string(duration.count()) + "ms");
        } else {
            return CheckResult(false, "Response time too slow: " + std::to_string(duration.count()) + "ms");
        }
    });

    auto status8 = checker8.check();
    std::cout << "延迟检查结果:" << std::endl;
    for (const auto& [name, passed] : status8.checks) {
        std::cout << "  " << (passed ? "[✓]" : "[✗]") << " " << name << ": " << status8.details.at(name) << std::endl;
    }

    std::cout << "\n========================================" << std::endl;
    std::cout << "         所有示例执行完成" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
