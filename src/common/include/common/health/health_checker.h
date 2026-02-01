/**
 * @file health_checker.h
 * @brief 健康检查器
 *
 * 提供系统健康检查功能，支持注册自定义检查项
 */

#pragma once

#include <string>
#include <map>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <chrono>
#include <optional>

namespace rendu {

/**
 * @brief 健康检查状态
 */
struct HealthStatus {
    bool healthy;                                    ///< 整体健康状态
    std::string status;                              ///< 状态描述
    std::map<std::string, bool> checks;            ///< 各检查项结果
    std::map<std::string, std::string> details;     ///< 各检查项详细信息

    /**
     * @brief 转换为 JSON 格式
     */
    std::string to_json() const;

    /**
     * @brief 转换为纯文本格式
     */
    std::string to_text() const;
};

/**
 * @brief 健康检查结果
 */
struct CheckResult {
    bool passed;                                     ///< 检查是否通过
    std::string message;                             ///< 检查消息
    std::optional<std::chrono::milliseconds> duration; ///< 检查耗时

    CheckResult() : passed(true) {}

    CheckResult(bool p, const std::string& msg)
        : passed(p), message(msg) {}

    CheckResult(bool p, const std::string& msg,
                std::chrono::milliseconds d)
        : passed(p), message(msg), duration(d) {}
};

/**
 * @brief 健康检查器
 *
 * 支持注册多个检查项，执行检查并返回综合健康状态
 */
class HealthChecker {
public:
    using CheckFunc = std::function<CheckResult()>;

    HealthChecker() = default;
    ~HealthChecker() = default;

    // 禁止拷贝和移动
    HealthChecker(const HealthChecker&) = delete;
    HealthChecker& operator=(const HealthChecker&) = delete;
    HealthChecker(HealthChecker&&) = delete;
    HealthChecker& operator=(HealthChecker&&) = delete;

    /**
     * @brief 执行所有健康检查
     * @return 健康状态
     */
    HealthStatus check();

    /**
     * @brief 执行指定的健康检查
     * @param name 检查项名称
     * @return 健康状态（仅包含指定检查项）
     */
    std::optional<HealthStatus> check(const std::string& name);

    /**
     * @brief 注册健康检查项
     * @param name 检查项名称
     * @param check 检查函数
     */
    void register_check(const std::string& name, CheckFunc check);

    /**
     * @brief 注销健康检查项
     * @param name 检查项名称
     * @return 是否成功注销
     */
    bool unregister_check(const std::string& name);

    /**
     * @brief 设置是否允许部分失败
     * @param allow 如果为 true，部分检查失败不会导致整体状态为不健康
     */
    void set_allow_partial_failure(bool allow) {
        std::lock_guard<std::mutex> lock(mutex_);
        allow_partial_failure_ = allow;
    }

    /**
     * @brief 获取注册的检查项数量
     */
    size_t check_count() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return checks_.size();
    }

    /**
     * @brief 获取所有检查项名称
     */
    std::vector<std::string> get_check_names() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> names;
        names.reserve(checks_.size());
        for (const auto& [name, _] : checks_) {
            names.push_back(name);
        }
        return names;
    }

    /**
     * @brief 清除所有检查项
     */
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        checks_.clear();
    }

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, CheckFunc> checks_;
    bool allow_partial_failure_ = false;
};

/**
 * @brief 内置检查函数
 */
namespace health_checks {

/**
 * @brief 内存使用检查
 * @param max_bytes 最大允许字节数
 */
CheckResult check_memory_usage(size_t max_bytes);

/**
 * @brief CPU 使用率检查
 * @param max_percent 最大允许百分比
 */
CheckResult check_cpu_usage(double max_percent);

/**
 * @brief 磁盘使用率检查
 * @param path 磁盘路径
 * @param max_percent 最大允许百分比
 */
CheckResult check_disk_usage(const std::string& path, double max_percent);

/**
 * @brief 简单的始终通过的检查
 */
CheckResult always_pass(const std::string& message = "OK");

/**
 * @brief 简单的始终失败的检查
 */
CheckResult always_fail(const std::string& message = "Failed");

/**
 * @brief 延迟检查
 * @param delay_ms 延迟毫秒数
 */
CheckResult check_latency(uint64_t delay_ms);

} // namespace health_checks

} // namespace rendu
