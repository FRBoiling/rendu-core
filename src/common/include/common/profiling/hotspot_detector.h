//
// Hotspot Detector - 热点检测
// 用于自动识别 ECS 系统中的性能瓶颈
//

#pragma once

#include "common/define.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <atomic>
#include <mutex>
#include <chrono>

BEGIN_NAMESPACE_ECS

// 前向声明
class PerformanceProfiler;

/// @brief 热点信息
struct Hotspot {
    std::string name;                ///< 操作名称
    std::string category;            ///< 类别
    uint64_t callCount;             ///< 调用次数
    float totalTime;                 ///< 总耗时（毫秒）
    float avgTime;                  ///< 平均耗时（毫秒）
    float minTime;                  ///< 最小耗时（毫秒）
    float maxTime;                  ///< 最大耗时（毫秒）
    float percentage;               ///< 占总时间的百分比
    std::vector<std::string> stackTrace;  ///< 调用栈（简化版）
    bool isCritical;                ///< 是否为关键热点
};

/// @brief 热点检测器 - 自动识别性能瓶颈
class HotspotDetector {
public:
    /// @brief 配置选项
    struct Config {
        float hotspotThreshold = 5.0f;   ///< 热点阈值（毫秒），超过此时间会被标记为热点
        float criticalThreshold = 50.0f;  ///< 关键热点阈值（毫秒）
        size_t minCallCount = 5;          ///< 最小调用次数，低于此值不考虑为热点
        bool enableStackTrace = false;    ///< 是否启用调用栈跟踪
        size_t maxStackTraceDepth = 10;   ///< 最大调用栈深度
    };

    /// @brief 报告格式
    enum class ReportFormat {
        Text,      ///< 文本格式
        JSON,      ///< JSON 格式
        Markdown   ///< Markdown 格式
    };

    /// @brief 构造函数
    explicit HotspotDetector();

    /// @brief 析构函数
    ~HotspotDetector();

    /// @brief 设置配置
    void setConfig(const Config& config);

    /// @brief 获取配置
    const Config& getConfig() const { return m_config; }

    /// @brief 开始检测
    void startDetection();

    /// @brief 停止检测
    void stopDetection();

    /// @brief 检查是否正在检测
    bool isDetecting() const { return m_isDetecting.load(); }

    /// @brief 记录操作开始
    /// @param name 操作名称
    /// @param category 类别（可选）
    /// @return 操作 ID
    uint64_t beginOperation(const std::string& name, const std::string& category = "");

    /// @brief 记录操作结束
    /// @param operationId 操作 ID
    void endOperation(uint64_t operationId);

    /// @brief 自动测量操作（RAII 风格）
    class ScopeTimer {
    public:
        ScopeTimer(HotspotDetector& detector, const std::string& name, const std::string& category = "");
        ~ScopeTimer();

        // 禁止拷贝
        ScopeTimer(const ScopeTimer&) = delete;
        ScopeTimer& operator=(const ScopeTimer&) = delete;

    private:
        HotspotDetector& m_detector;
        uint64_t m_operationId;
    };

    /// @brief 获取热点列表
    std::vector<Hotspot> getHotspots();

    /// @brief 获取关键热点列表（超过 criticalThreshold 的热点）
    std::vector<Hotspot> getCriticalHotspots();

    /// @brief 获取所有操作统计
    std::vector<Hotspot> getAllOperations();

    /// @brief 按类别获取热点
    /// @param category 类别
    /// @return 该类别的热点列表
    std::vector<Hotspot> getHotspotsByCategory(const std::string& category);

    /// @brief 清除数据
    void clear();

    /// @brief 打印热点报告
    void printReport() const;

    /// @brief 导出报告
    /// @param filename 文件名
    /// @param format 报告格式
    /// @return 是否成功
    bool exportReport(const std::string& filename, ReportFormat format = ReportFormat::Text) const;

    /// @brief 导出为 JSON 字符串
    std::string exportToJson() const;

    /// @brief 获取优化建议
    std::vector<std::string> getOptimizationSuggestions() const;

private:
    /// @brief 操作记录
    struct OperationRecord {
        std::string name;
        std::string category;
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point endTime;
    };

    /// @brief 操作统计
    struct OperationStats {
        std::string name;
        std::string category;
        std::atomic<uint64_t> callCount;
        std::atomic<float> totalTime;
        std::atomic<float> minTime;
        std::atomic<float> maxTime;

        OperationStats(const std::string& n, const std::string& c)
            : name(n)
            , category(c)
            , callCount(0)
            , totalTime(0.0f)
            , minTime(std::numeric_limits<float>::max())
            , maxTime(0.0f) {}
    };

    /// @brief 配置
    Config m_config;

    /// @brief 是否正在检测
    std::atomic<bool> m_isDetecting;

    /// @brief 操作 ID 计数器
    std::atomic<uint64_t> m_nextOperationId;

    /// @brief 操作记录映射（用于 begin/end 配对）
    std::unordered_map<uint64_t, OperationRecord> m_operationRecords;

    /// @brief 操作统计
    std::unordered_map<std::string, std::unique_ptr<OperationStats>> m_operationStats;

    /// @brief 互斥锁
    mutable std::mutex m_mutex;

    /// @brief 更新操作统计
    void updateStats(const std::string& name, const std::string& category, float elapsedMs);

    /// @brief 检测热点
    std::vector<Hotspot> detectHotspots(float threshold, bool onlyCritical = false) const;

    /// @brief 生成调用栈（简化版）
    std::vector<std::string> generateStackTrace() const;
};

/// @brief 便利宏：自动测量作用域
#define HOTSPOT_SCOPE(detector, name) \
    HotspotDetector::ScopeTimer _hotspot_timer(detector, name)

#define HOTSPOT_SCOPE_CATEGORY(detector, name, category) \
    HotspotDetector::ScopeTimer _hotspot_timer(detector, name, category)

END_NAMESPACE_ECS
