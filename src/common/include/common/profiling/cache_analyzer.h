//
// Cache Analyzer - 缓存分析
// 用于分析 ECS 系统的缓存命中率和访问模式
//

#pragma once

#include "common/define.h"
#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

// 前向声明
BEGIN_NAMESPACE_ECS
    class RegistryOptimized;

    /**
     * @brief 访问模式
     */
    struct AccessPattern
    {
        std::string componentType;        ///< 组件类型
        uint64_t linearAccesses;          ///< 线性访问次数
        uint64_t randomAccesses;          ///< 随机访问次数
        uint64_t sequentialAccesses;      ///< 顺序访问次数
        float localityScore;              ///< 局部性分数 (0-1)
        std::vector<uint64_t> accessSequence; ///< 访问序列（最近 N 次）
    };

    /**
     * @brief 缓存统计信息
     */
    struct CacheStats
    {
        uint64_t totalAccesses;           ///< 总访问次数
        uint64_t cacheHits;               ///< 缓存命中次数
        uint64_t cacheMisses;             ///< 缓存未命中次数
        float hitRate;                    ///< 命中率 (0-1)

        std::vector<AccessPattern> patterns; ///< 各组件类型的访问模式

        uint64_t coldMisses;              ///< 冷启动未命中
        uint64_t capacityMisses;          ///< 容量未命中
        uint64_t conflictMisses;          ///< 冲突未命中

        float avgAccessLatency;           ///< 平均访问延迟（纳秒）
        float minAccessLatency;           ///< 最小访问延迟
        float maxAccessLatency;           ///< 最大访问延迟
    };

    /**
     * @brief 缓存优化建议
     */
    struct CacheOptimizationSuggestion
    {
        std::string componentType;        ///< 组件类型
        std::string suggestion;           ///< 建议内容
        int priority;                     ///< 优先级 (1-10, 10 最高)
        float expectedImprovement;        ///< 预期提升 (0-1)
    };

    /// @brief 缓存分析器 - 分析 ECS 系统的缓存友好性
    class RC_COMMON_API CacheAnalyzer
    {
    public:
        /// @brief 配置选项
        struct Config
        {
            size_t accessSequenceLength = 1000; ///< 记录的访问序列长度
            bool trackLatency = true; ///< 是否跟踪访问延迟
            bool enableHotspotDetection = true; ///< 是否启用热点检测
            float hotspotThreshold = 0.8f; ///< 热点阈值（访问频率）
        };

        /// @brief 构造函数
        explicit CacheAnalyzer();

        /// @brief 析构函数
        ~CacheAnalyzer() = default;

        /// @brief 设置配置
        void setConfig(const Config& config);

        /// @brief 获取配置
        const Config& getConfig() const { return m_config; }

        /// @brief 开始统计
        void startProfiling();

        /// @brief 停止统计
        void stopProfiling();

        /// @brief 检查是否正在统计
        bool isProfiling() const { return m_isProfiling.load(); }

        /// @brief 记录缓存访问
        /// @param componentType 组件类型名称
        /// @param hit 是否命中缓存
        /// @param isLinear 是否为线性访问
        void recordAccess(const std::string& componentType, bool hit, bool isLinear = true);

        /// @brief 记录访问延迟
        /// @param latencyNs 延迟（纳秒）
        void recordLatency(float latencyNs);

        /// @brief 清除统计数据
        void clear();

        /// @brief 获取缓存统计
        CacheStats getCacheStats();

        /// @brief 生成缓存报告
        /// @param stats 缓存统计信息
        /// @return 报告字符串
        std::string generateReport(const CacheStats& stats) const;

        /// @brief 打印缓存报告
        /// @param stats 缓存统计信息
        void printReport(const CacheStats& stats) const;

        /// @brief 建议优化
        /// @param stats 缓存统计信息
        /// @return 优化建议列表
        std::vector<CacheOptimizationSuggestion> suggestOptimizations(const CacheStats& stats) const;

        /// @brief 导出报告
        /// @param stats 缓存统计信息
        /// @param filename 文件名
        /// @return 是否成功
        bool exportReport(const CacheStats& stats, const std::string& filename) const;

        /// @brief 导出为 JSON
        /// @param stats 缓存统计信息
        /// @return JSON 字符串
        std::string exportToJson(const CacheStats& stats) const;

        /// @brief 自动分析 Registry 的缓存友好性（辅助函数）
        /// @param registry 要分析的注册表
        /// @param sampleFunction 采样函数，用于遍历实体
        template <typename Func>
        void analyzeRegistry(const RegistryOptimized& registry, Func&& sampleFunction);

    private:
        /// @brief 配置
        Config m_config;

        /// @brief 是否正在统计
        std::atomic<bool> m_isProfiling;

        /// @brief 总访问次数
        std::atomic<uint64_t> m_totalAccesses;

        /// @brief 缓存命中次数
        std::atomic<uint64_t> m_cacheHits;

        /// @brief 缓存未命中次数
        std::atomic<uint64_t> m_cacheMisses;

        /// @brief 访问模式映射
        std::unordered_map<std::string, AccessPattern> m_accessPatterns;

        /// @brief 延迟统计
        std::atomic<float> m_avgLatency;
        std::atomic<float> m_minLatency;
        std::atomic<float> m_maxLatency;
        std::atomic<uint64_t> m_latencySampleCount;

        /// @brief 互斥锁
        mutable std::mutex m_mutex;

        /// @brief 更新访问模式
        void updateAccessPattern(const std::string& componentType, bool isLinear);

        /// @brief 计算局部性分数
        float calculateLocalityScore(const std::vector<uint64_t>& sequence) const;

        /// @brief 识别访问类型
        enum class AccessType { Linear, Random, Sequential };

        AccessType classifyAccess(const std::vector<uint64_t>& sequence) const;
    };

END_NAMESPACE_ECS

/// @brief 便利宏：自动缓存访问记录
#define CACHE_ACCESS(analyzer, type, hit) \
    analyzer.recordAccess(type, hit)

#define CACHE_ACCESS_LINEAR(analyzer, type, hit) \
    analyzer.recordAccess(type, hit, true)

#define CACHE_ACCESS_RANDOM(analyzer, type, hit) \
    analyzer.recordAccess(type, hit, false)
