//
// Created by boil on 2026/1/15.
//

#include "common/ecs/profiler.h"
#include "common/ecs/registry_optimized.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstddef>
#include <fstream>

BEGIN_NAMESPACE_ECS

    // ============================================================================
    // ScopeTimer 实现
    // ============================================================================

    ScopeTimer::ScopeTimer(std::function<void(float)> callback)
        : m_start(high_resolution_clock::now())
        , m_callback(std::move(callback))
    {}

    ScopeTimer::~ScopeTimer()
    {
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - m_start);
        m_callback(duration.count() / 1000.0f); // 转换为毫秒
    }

    // ============================================================================
    // PerformanceProfiler 实现
    // ============================================================================

    void PerformanceProfiler::recordOperation(const std::string& name, float durationMs)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto& stat = m_stats[name];
        stat.count++;
        stat.totalTime += durationMs;
        stat.minTime = std::min(stat.minTime, durationMs);
        stat.maxTime = std::max(stat.maxTime, durationMs);
    }

    void PerformanceProfiler::incrementCounter(const std::string& name, uint64_t delta)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stats[name].count += delta;
    }

    const PerformanceStat* PerformanceProfiler::getStat(const std::string& name) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = m_stats.find(name);
        if (it == m_stats.end())
        {
            return nullptr;
        }

        return reinterpret_cast<const PerformanceStat*>(&it->second);
    }

    std::vector<PerformanceStat> PerformanceProfiler::getAllStats() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        std::vector<PerformanceStat> stats;
        stats.reserve(m_stats.size());

        for (const auto& [name, data] : m_stats)
        {
            PerformanceStat stat;
            stat.name = name;
            stat.count = data.count;
            stat.totalTime = data.totalTime;
            stat.avgTime = data.count > 0 ? data.totalTime / data.count : 0.0f;
            stat.minTime = data.minTime;
            stat.maxTime = data.maxTime;
            stats.push_back(stat);
        }

        // 按总耗时排序
        std::sort(stats.begin(), stats.end(),
            [](const PerformanceStat& a, const PerformanceStat& b) {
                return a.totalTime > b.totalTime;
            });

        return stats;
    }

    void PerformanceProfiler::printReport() const
    {
        auto stats = getAllStats();

        std::cout << "\n========================================" << std::endl;
        std::cout << "  性能分析报告" << std::endl;
        std::cout << "========================================" << std::endl;

        std::cout << std::left << std::setw(30) << "操作"
                  << std::right << std::setw(12) << "次数"
                  << std::setw(12) << "总耗时(ms)"
                  << std::setw(12) << "平均(ms)"
                  << std::setw(12) << "最小(ms)"
                  << std::setw(12) << "最大(ms)" << std::endl;
        std::cout << std::string(90, '-') << std::endl;

        for (const auto& stat : stats)
        {
            std::cout << std::left << std::setw(30) << stat.name
                      << std::right << std::setw(12) << stat.count
                      << std::setw(12) << std::fixed << std::setprecision(2) << stat.totalTime
                      << std::setw(12) << std::fixed << std::setprecision(4) << stat.avgTime
                      << std::setw(12) << std::fixed << std::setprecision(4) << stat.minTime
                      << std::setw(12) << std::fixed << std::setprecision(4) << stat.maxTime
                      << std::endl;
        }

        std::cout << "========================================" << std::endl;
    }

    void PerformanceProfiler::reset()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stats.clear();
    }

    ScopeTimer PerformanceProfiler::timeScope(const std::string& name)
    {
        return ScopeTimer([this, name](float duration) {
            recordOperation(name, duration);
        });
    }

    // ============================================================================
    // MemoryAnalyzer 实现
    // ============================================================================

    MemoryAnalyzer::MemoryAnalyzer()
        : m_config{}
    {
    }

    void MemoryAnalyzer::setConfig(const MemoryAnalysisConfig& config)
    {
        m_config = config;
    }

    RegistryMemoryInfo MemoryAnalyzer::analyze(const RegistryOptimized& registry) const
    {
        // 直接使用RegistryOptimized提供的接口
        return registry.getMemoryInfo();
    }

    FragmentInfo MemoryAnalyzer::analyzeFragmentation(const RegistryOptimized& registry) const
    {
        FragmentInfo info;
        auto baseInfo = analyze(registry);
        
        info.totalSize = baseInfo.totalMemory;
        info.totalArchetypes = baseInfo.archetypeCount;
        info.archetypeInfo = baseInfo.archetypes;
        
        // 计算使用内存
        info.usedSize = 0;
        for (const auto& archInfo : baseInfo.archetypes)
        {
            info.usedSize += archInfo.entityCount * archInfo.componentSize;
        }
        
        info.freeSize = info.totalSize - info.usedSize;
        info.allocatedMemory = info.totalSize;
        
        // 计算碎片率
        info.fragmentationRate = estimateFragmentation(info.usedSize, info.totalSize);
        
        // 简化碎片分析（详细分析需要更复杂的实现）
        info.fragmentCount = baseInfo.archetypeCount;
        info.averageFragmentSize = info.totalSize / std::max(1u, static_cast<uint32_t>(baseInfo.archetypeCount));
        
        return info;
    }

    void MemoryAnalyzer::printReport(const RegistryMemoryInfo& info) const
    {
        std::cout << "\n========================================" << std::endl;
        std::cout << "  内存分析报告" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "总内存: " << (info.totalMemory / 1024.0) << " KB" << std::endl;
        std::cout << "实体数量: " << info.entityCount << std::endl;
        std::cout << "Archetype 数量: " << info.archetypeCount << std::endl;
        std::cout << "========================================" << std::endl;

        for (const auto& archInfo : info.archetypes)
        {
            std::cout << "Archetype [" << archInfo.componentTypes << "]" << std::endl;
            std::cout << "  实体数: " << archInfo.entityCount << std::endl;
            std::cout << "  容量: " << archInfo.capacity << std::endl;
            std::cout << "  内存: " << (archInfo.totalMemory / 1024.0) << " KB" << std::endl;
            std::cout << std::endl;
        }
    }

    void MemoryAnalyzer::printFragmentationReport(const FragmentInfo& info) const
    {
        std::cout << "\n========================================" << std::endl;
        std::cout << "  内存碎片分析报告" << std::endl;
        std::cout << "========================================" << std::endl;
        
        std::cout << "总内存: " << formatMemorySize(info.totalSize) << std::endl;
        std::cout << "已使用: " << formatMemorySize(info.usedSize) << std::endl;
        std::cout << "空闲内存: " << formatMemorySize(info.freeSize) << std::endl;
        std::cout << "碎片率: " << (info.fragmentationRate * 100) << "%" << std::endl;
        std::cout << "碎片数量: " << info.fragmentCount << std::endl;
        std::cout << "平均碎片大小: " << formatMemorySize(info.averageFragmentSize) << std::endl;
        std::cout << "========================================" << std::endl;
    }

    float MemoryAnalyzer::calculateFragmentation(const RegistryMemoryInfo& info) const
    {
        if (info.totalMemory == 0) return 0.0f;

        std::size_t usedMemory = 0;
        for (const auto& archInfo : info.archetypes)
        {
            usedMemory += archInfo.entityCount * archInfo.componentSize;
        }

        return 1.0f - (static_cast<float>(usedMemory) / info.totalMemory);
    }

    std::vector<std::string> MemoryAnalyzer::suggestDefragmentation(const FragmentInfo& info) const
    {
        std::vector<std::string> suggestions;
        
        if (info.fragmentationRate > m_config.highFragmentationThreshold)
        {
            suggestions.push_back("检测到高碎片率，建议进行内存整理");
            suggestions.push_back("考虑合并相似的Archetype以减少碎片");
        }
        
        if (info.averageFragmentSize < m_config.minFragmentSize)
        {
            suggestions.push_back("平均碎片大小较小，建议优化内存分配策略");
        }
        
        if (suggestions.empty())
        {
            suggestions.push_back("内存状态良好，无需特殊优化");
        }
        
        return suggestions;
    }

    std::string MemoryAnalyzer::generateFragmentationReport(const FragmentInfo& info) const
    {
        std::ostringstream oss;
        oss << "内存碎片分析报告\n";
        oss << "总内存: " << formatMemorySize(info.totalSize) << "\n";
        oss << "已使用: " << formatMemorySize(info.usedSize) << "\n";
        oss << "碎片率: " << (info.fragmentationRate * 100) << "%\n";
        
        auto suggestions = suggestDefragmentation(info);
        if (!suggestions.empty())
        {
            oss << "优化建议:\n";
            for (const auto& suggestion : suggestions)
            {
                oss << "- " << suggestion << "\n";
            }
        }
        
        return oss.str();
    }

    bool MemoryAnalyzer::exportReport(const FragmentInfo& info, const std::string& filename) const
    {
        std::ofstream file(filename);
        if (!file.is_open())
        {
            return false;
        }
        
        file << generateFragmentationReport(info);
        file.close();
        return true;
    }

    float MemoryAnalyzer::estimateFragmentation(size_t usedSize, size_t allocatedSize) const
    {
        if (allocatedSize == 0) return 0.0f;
        return 1.0f - (static_cast<float>(usedSize) / allocatedSize);
    }

    std::string MemoryAnalyzer::formatMemorySize(size_t bytes) const
    {
        const char* units[] = {"B", "KB", "MB", "GB"};
        size_t unitIndex = 0;
        double size = static_cast<double>(bytes);
        
        while (size >= 1024.0 && unitIndex < 3)
        {
            size /= 1024.0;
            unitIndex++;
        }
        
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << size << " " << units[unitIndex];
        return oss.str();
    }

END_NAMESPACE_ECS


