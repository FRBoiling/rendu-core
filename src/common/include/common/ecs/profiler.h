//
// Created by boil on 2026/1/15.
// ECS 性能分析器
//

#ifndef RENDU_ECS_PROFILER_H
#define RENDU_ECS_PROFILER_H

#include "common/define.h"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <limits>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

BEGIN_NAMESPACE_ECS

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::milliseconds;

    // ============================================================================
    // 性能计数器
    // ============================================================================

    /**
     * @brief 操作计时器
     */
    class RC_COMMON_API ScopeTimer
    {
    public:
        explicit ScopeTimer(std::function<void(float)> callback);
        ~ScopeTimer();

        ScopeTimer(const ScopeTimer&) = delete;
        ScopeTimer& operator=(const ScopeTimer&) = delete;

    private:
        high_resolution_clock::time_point m_start;
        std::function<void(float)> m_callback;
    };

    /**
     * @brief 性能统计项
     */
    struct RC_COMMON_API PerformanceStat
    {
        std::string name;
        uint64_t count;
        float totalTime;        // 总耗时（毫秒）
        float avgTime;         // 平均耗时（毫秒）
        float minTime;         // 最小耗时（毫秒）
        float maxTime;         // 最大耗时（毫秒）
    };

    // ============================================================================
    // 性能分析器
    // ============================================================================

    /**
     * @brief ECS 系统性能分析器
     *
     * 记录各种操作的耗时和频率，用于性能优化分析。
     */
    class RC_COMMON_API PerformanceProfiler
    {
    public:
        /**
         * @brief 记录操作
         */
        void recordOperation(const std::string& name, float durationMs);

        /**
         * @brief 增加计数
         */
        void incrementCounter(const std::string& name, uint64_t delta = 1);

        /**
         * @brief 获取统计数据
         */
        [[nodiscard]] const PerformanceStat* getStat(const std::string& name) const;

        /**
         * @brief 获取所有统计数据
         */
        [[nodiscard]] std::vector<PerformanceStat> getAllStats() const;

        /**
         * @brief 打印性能报告
         */
        void printReport() const;

        /**
         * @brief 重置统计
         */
        void reset();

        /**
         * @brief 创建范围计时器
         */
        [[nodiscard]] ScopeTimer timeScope(const std::string& name);

        // ========================================================================
        // 便捷宏
        // ========================================================================

#define PROFILE_SCOPE(profiler, name) \
    auto CONCAT(_timer_, __LINE__) = (profiler).timeScope(name)

    private:
        struct StatData
        {
            uint64_t count = 0;
            float totalTime = 0.0f;
            float minTime = std::numeric_limits<float>::max();
            float maxTime = 0.0f;
        };

        std::unordered_map<std::string, StatData> m_stats;
        mutable std::mutex m_mutex;
    };

    // ============================================================================
    // 内存分析器
    // ============================================================================

    // 前向声明
    class RegistryOptimized;

    /**
     * @brief Archetype 内存信息
     */
    struct RC_COMMON_API ArchetypeMemoryInfo
    {
        std::string componentTypes;
        size_t entityCount;
        size_t capacity;
        size_t componentSize;
        size_t totalMemory;
    };

    /**
     * @brief 内存块信息
     */
    struct RC_COMMON_API MemoryBlock
    {
        void* address;        ///< 内存地址
        size_t size;          ///< 块大小（字节）
        bool isUsed;          ///< 是否已使用
        size_t alignment;     ///< 对齐方式
        std::string label;    ///< 标签（可选）

        MemoryBlock() : address(nullptr), size(0), isUsed(false), alignment(0)
        {
        }

        MemoryBlock(void* addr, size_t sz, bool used, size_t align = 0, const std::string& lbl = "")
            : address(addr), size(sz), isUsed(used), alignment(align), label(lbl)
        {
        }
    };

    /**
     * @brief 注册表内存信息
     */
    struct RC_COMMON_API RegistryMemoryInfo
    {
        size_t totalMemory;
        size_t entityCount;
        size_t archetypeCount;
        std::vector<ArchetypeMemoryInfo> archetypes;
    };

    /**
     * @brief 内存碎片信息
     */
    struct RC_COMMON_API FragmentInfo
    {
        size_t totalSize;                    ///< 总内存（字节）
        size_t usedSize;                     ///< 已使用内存（字节）
        size_t freeSize;                     ///< 空闲内存（字节）
        size_t allocatedMemory;              ///< 已分配内存（字节）
        float fragmentationRate;             ///< 碎片率 (0-1)

        std::vector<MemoryBlock> usedBlocks; ///< 已使用块列表
        std::vector<MemoryBlock> freeBlocks; ///< 空闲块列表

        std::vector<ArchetypeMemoryInfo> archetypeInfo; ///< Archetype 内存信息

        size_t totalArchetypes;              ///< 总 Archetype 数
        size_t fragmentCount;                ///< 总碎片数
        size_t averageFragmentSize;          ///< 平均碎片大小

        FragmentInfo() : totalSize(0), usedSize(0), freeSize(0), allocatedMemory(0), 
                        fragmentationRate(0.0f), totalArchetypes(0), fragmentCount(0), 
                        averageFragmentSize(0)
        {
        }
    };

    /**
     * @brief 内存分析配置选项
     */
    struct RC_COMMON_API MemoryAnalysisConfig
    {
        bool detailedAnalysis = true;                ///< 是否进行详细分析
        bool suggestOptimizations = true;            ///< 是否提供优化建议
        bool trackFragmentation = true;              ///< 是否跟踪内存碎片
        size_t minFragmentSize = 16;                 ///< 最小碎片大小（字节）
        float highFragmentationThreshold = 0.3f;     ///< 高碎片率阈值
    };

    /**
     * @brief ECS 系统内存分析器
     *
     * 统一的内存分析工具，整合基础内存统计和碎片分析功能。
     */
    class RC_COMMON_API MemoryAnalyzer
    {
    public:
        /**
         * @brief 构造函数
         */
        explicit MemoryAnalyzer();

        /**
         * @brief 设置配置
         */
        void setConfig(const MemoryAnalysisConfig& config);

        /**
         * @brief 获取配置
         */
        const MemoryAnalysisConfig& getConfig() const { return m_config; }

        /**
         * @brief 分析注册表内存（基础版本）
         */
        [[nodiscard]] RegistryMemoryInfo analyze(const class RegistryOptimized& registry) const;

        /**
         * @brief 分析内存碎片（详细版本）
         */
        [[nodiscard]] FragmentInfo analyzeFragmentation(const class RegistryOptimized& registry) const;

        /**
         * @brief 打印内存报告
         */
        void printReport(const RegistryMemoryInfo& info) const;

        /**
         * @brief 打印碎片报告
         */
        void printFragmentationReport(const FragmentInfo& info) const;

        /**
         * @brief 估算内存碎片率
         */
        [[nodiscard]] float calculateFragmentation(const RegistryMemoryInfo& info) const;

        /**
         * @brief 建议碎片整理策略
         */
        [[nodiscard]] std::vector<std::string> suggestDefragmentation(const FragmentInfo& info) const;

        /**
         * @brief 生成碎片报告
         */
        [[nodiscard]] std::string generateFragmentationReport(const FragmentInfo& info) const;

        /**
         * @brief 导出报告
         */
        bool exportReport(const FragmentInfo& info, const std::string& filename) const;

    private:
        MemoryAnalysisConfig m_config;

        /**
         * @brief 估算碎片率（简化版）
         */
        float estimateFragmentation(size_t usedSize, size_t allocatedSize) const;

        /**
         * @brief 格式化内存大小
         */
        std::string formatMemorySize(size_t bytes) const;
    };

    // ============================================================================
    // 辅助宏
    // ============================================================================

#ifndef CONCAT
#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)
#endif

END_NAMESPACE_ECS

#endif // RENDU_ECS_PROFILER_H
