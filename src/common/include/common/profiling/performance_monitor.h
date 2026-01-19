//
// Performance Monitor - 性能监控面板
// 用于实时监控 ECS 系统的性能指标
//

#pragma once

#include "common/define.h"
#include <chrono>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

BEGIN_NAMESPACE_ECS

// 前向声明
class RegistryOptimized;
class MemoryAnalyzer;

/// @brief 性能监控面板 - 实时监控性能指标
class RC_COMMON_API PerformanceMonitor {
public:
    /// @brief 性能快照
    struct Snapshot {
        uint64_t timestamp;                                    ///< 时间戳（毫秒）
        float fps;                                              ///< 每秒帧数
        float tps;                                              ///< 每秒更新次数
        size_t entityCount;                                     ///< 实体数量
        size_t memoryUsage;                                     ///< 内存使用量（字节）
        std::unordered_map<std::string, float> operationTimes; ///< 操作耗时（毫秒）
        std::unordered_map<std::string, float> operationCounts; ///< 操作计数
    };

    /// @brief 历史统计
    struct HistoryStats {
        float avgFps;              ///< 平均 FPS
        float minFps;              ///< 最小 FPS
        float maxFps;              ///< 最大 FPS
        float avgEntityCount;      ///< 平均实体数
        size_t peakEntityCount;    ///< 峰值实体数
        float avgMemoryUsage;      ///< 平均内存使用
        size_t peakMemoryUsage;    ///< 峰值内存使用
    };

    /// @brief 配置选项
    struct Config {
        size_t maxHistorySize = 1000;  ///< 最大历史记录数
        float updateInterval = 0.016f;  ///< 更新间隔（秒，约 60 FPS）
        bool autoUpdate = false;       ///< 是否自动更新
    };

    /// @brief 构造函数
    explicit PerformanceMonitor();

    /// @brief 析构函数
    ~PerformanceMonitor();

    /// @brief 设置配置
    /// @param config 配置选项
    void setConfig(const Config& config);

    /// @brief 获取配置
    /// @return 当前配置
    const Config& getConfig() const { return m_config; }

    /// @brief 更新监控数据
    /// @param registry 要监控的注册表
    /// @param deltaTime 自上次更新以来的时间（秒）
    void update(const RegistryOptimized& registry, float deltaTime = 0.0f);

/// @brief 手动更新监控数据（带内存分析）
/// @param registry 要监控的注册表
/// @param memoryAnalyzer 内存分析器
/// @param deltaTime 自上次更新以来的时间（秒）
void update(const RegistryOptimized& registry, const MemoryAnalyzer& memoryAnalyzer, float deltaTime = 0.0f);

    /// @brief 获取当前快照
    /// @return 当前快照
    Snapshot getCurrentSnapshot() const;

    /// @brief 获取历史数据
    /// @return 历史快照列表
    const std::vector<Snapshot>& getHistory() const;

    /// @brief 清除历史数据
    void clearHistory();

    /// @brief 获取历史统计
    /// @return 历史统计信息
    HistoryStats getHistoryStats() const;

    /// @brief 记录操作时间
    /// @param operationName 操作名称
    /// @param timeMs 耗时（毫秒）
    void recordOperation(const std::string& operationName, float timeMs);

    /// @brief 记录操作计数
    /// @param operationName 操作名称
    /// @param count 操作次数
    void recordOperationCount(const std::string& operationName, float count = 1.0f);

    /// @brief 重置操作统计
    void resetOperationStats();

    /// @brief 计算平均 FPS
    /// @return 平均 FPS
    float calculateAverageFps() const;

    /// @brief 计算平均 TPS
    /// @return 平均 TPS
    float calculateAverageTps() const;

    /// @brief 打印当前快照
    void printCurrentSnapshot() const;

    /// @brief 打印历史统计
    void printHistoryStats() const;

    /// @brief 导出报告为文本格式
    /// @param filename 文件名
    /// @return 是否成功
    bool exportReport(const std::string& filename) const;

    /// @brief 导出报告为 JSON 格式
    /// @param filename 文件名
    /// @return 是否成功
    bool exportJsonReport(const std::string& filename) const;

    /// @brief 导出当前快照为 JSON 字符串
    /// @return JSON 字符串
    std::string exportSnapshotToJson() const;

    /// @brief 导出历史统计为 JSON 字符串
    /// @return JSON 字符串
    std::string exportHistoryToJson() const;

private:
    /// @brief 配置
    Config m_config;

    /// @brief 当前快照
    Snapshot m_currentSnapshot;

    /// @brief 历史快照列表
    std::vector<Snapshot> m_history;

    /// @brief 帧计数器（用于计算 FPS）
    uint64_t m_frameCount;

    /// @frame 累计时间（用于计算 FPS）
    float m_accumulatedTime;

    /// @brief 上一次更新时间
    std::chrono::steady_clock::time_point m_lastUpdateTime;

    /// @brief 添加历史记录
    void addHistory();

    /// @brief 获取当前时间戳
    /// @return 时间戳（毫秒）
    uint64_t getCurrentTimestamp() const;
};

END_NAMESPACE_ECS
