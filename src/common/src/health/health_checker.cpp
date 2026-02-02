/**
 * @file health_checker.cpp
 * @brief 健康检查器实现
 */

#include "common/health/health_checker.h"

#include <sstream>
#include <iomanip>
#include <thread>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <sys/statvfs.h>
#include <unistd.h>
#include <fstream>
#endif

using namespace COMMON_NAMESPACE;

// ============================================================================
// HealthChecker 实现
// ============================================================================

HealthStatus HealthChecker::check() {
    std::lock_guard<std::mutex> lock(mutex_);

    HealthStatus status;
    status.healthy = true;
    status.status = "All checks passed";

    if (checks_.empty()) {
        status.status = "No checks registered";
        return status;
    }

    int passed = 0;
    int failed = 0;

    for (const auto& [name, check_func] : checks_) {
        try {
            auto start = std::chrono::steady_clock::now();
            CheckResult result = check_func();
            auto end = std::chrono::steady_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

            status.checks[name] = result.passed;
            status.details[name] = result.message;

            if (result.passed) {
                passed++;
            } else {
                failed++;
                if (!allow_partial_failure_) {
                    status.healthy = false;
                    status.status = "Check failed: " + name;
                }
            }
        } catch (const std::exception& e) {
            failed++;
            status.checks[name] = false;
            status.details[name] = "Exception: " + std::string(e.what());

            if (!allow_partial_failure_) {
                status.healthy = false;
                status.status = "Check threw exception: " + name;
            }
        }
    }

    if (allow_partial_failure_ && failed > 0) {
        status.healthy = false;
        status.status = "Partial failure: " + std::to_string(failed) + "/" +
                       std::to_string(checks_.size()) + " checks failed";
    } else if (!allow_partial_failure_ && status.healthy) {
        status.status = "All checks passed";
    }

    return status;
}

std::optional<HealthStatus> HealthChecker::check(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = checks_.find(name);
    if (it == checks_.end()) {
        return std::nullopt;
    }

    HealthStatus status;
    status.healthy = true;
    status.status = "Check passed";

    try {
        auto result = it->second();
        status.checks[name] = result.passed;
        status.details[name] = result.message;
        status.healthy = result.passed;
        status.status = result.passed ? "Check passed" : "Check failed";
    } catch (const std::exception& e) {
        status.checks[name] = false;
        status.details[name] = "Exception: " + std::string(e.what());
        status.healthy = false;
        status.status = "Check threw exception";
    }

    return status;
}

void HealthChecker::register_check(const std::string& name, CheckFunc check) {
    if (!check) {
        throw std::invalid_argument("Check function cannot be null");
    }
    std::lock_guard<std::mutex> lock(mutex_);
    checks_[name] = std::move(check);
}

bool HealthChecker::unregister_check(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    return checks_.erase(name) > 0;
}

// ============================================================================
// 内置检查函数实现
// ============================================================================

namespace health_checks {

#ifdef _WIN32
size_t get_memory_usage() {
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize;
    }
    return 0;
}
#else
size_t get_memory_usage() {
    std::ifstream statm("/proc/self/statm");
    if (!statm) {
        return 0;
    }

    size_t size, resident;
    statm >> size >> resident;
    statm.close();

    return resident * sysconf(_SC_PAGESIZE);
}
#endif

CheckResult check_memory_usage(size_t max_bytes) {
    try {
        size_t usage = get_memory_usage();
        double usage_mb = usage / (1024.0 * 1024.0);
        double max_mb = max_bytes / (1024.0 * 1024.0);

        if (usage <= max_bytes) {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2);
            oss << usage_mb << " MB / " << max_mb << " MB";
            return CheckResult(true, oss.str());
        } else {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2);
            oss << "Memory usage too high: " << usage_mb << " MB / " << max_mb << " MB";
            return CheckResult(false, oss.str());
        }
    } catch (const std::exception& e) {
        return CheckResult(false, "Failed to check memory: " + std::string(e.what()));
    }
}

CheckResult check_cpu_usage(double max_percent) {
    // 跨平台 CPU 使用率检测比较复杂，这里返回一个简化版本
    // 在实际生产环境中，应该使用专门的性能计数器库

#ifdef _WIN32
    FILETIME idle_time, kernel_time, user_time;
    if (GetSystemTimes(&idle_time, &kernel_time, &user_time)) {
        // 简化的计算，实际应该维护历史数据
        return CheckResult(true, "CPU usage check not fully implemented");
    }
#else
    // Linux 下的 CPU 使用率检查
    std::ifstream stat("/proc/stat");
    if (stat) {
        std::string line;
        if (std::getline(stat, line)) {
            // 解析 CPU 时间（简化版）
            return CheckResult(true, "CPU usage check not fully implemented");
        }
    }
#endif

    // 如果无法获取，返回通过
    return CheckResult(true, "CPU usage check not fully implemented");
}

CheckResult check_disk_usage(const std::string& path, double max_percent) {
#ifdef _WIN32
    ULARGE_INTEGER free_bytes, total_bytes;
    if (GetDiskFreeSpaceExA(path.c_str(), &free_bytes, &total_bytes, nullptr)) {
        double used_percent = 100.0 * (1.0 - static_cast<double>(free_bytes.QuadPart) /
                                       static_cast<double>(total_bytes.QuadPart));

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2);
        oss << used_percent << "%";

        if (used_percent <= max_percent) {
            return CheckResult(true, oss.str());
        } else {
            oss << " (exceeds " << max_percent << "%)";
            return CheckResult(false, oss.str());
        }
    }
#else
    struct statvfs stat;
    if (statvfs(path.c_str(), &stat) == 0) {
        unsigned long total = stat.f_blocks * stat.f_frsize;
        unsigned long available = stat.f_bavail * stat.f_frsize;
        unsigned long used = total - available;

        double used_percent = 100.0 * (static_cast<double>(used) / static_cast<double>(total));

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2);
        oss << used_percent << "%";

        if (used_percent <= max_percent) {
            return CheckResult(true, oss.str());
        } else {
            oss << " (exceeds " << max_percent << "%)";
            return CheckResult(false, oss.str());
        }
    }
#endif

    return CheckResult(false, "Failed to check disk usage");
}

CheckResult always_pass(const std::string& message) {
    return CheckResult(true, message);
}

CheckResult always_fail(const std::string& message) {
    return CheckResult(false, message);
}

CheckResult check_latency(uint64_t delay_ms) {
    // 模拟一个延迟检查，用于测试
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));

    std::ostringstream oss;
    oss << "Latency check completed in " << delay_ms << " ms";
    return CheckResult(true, oss.str());
}

} // namespace health_checks

