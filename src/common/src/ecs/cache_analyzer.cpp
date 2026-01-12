//
// Cache Analyzer 实现
//

#include "common/ecs/cache_analyzer.h"
#include "common/ecs/registry_optimized.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

BEGIN_NAMESPACE_ECS

CacheAnalyzer::CacheAnalyzer()
    : m_config()
    , m_isProfiling(false)
    , m_totalAccesses(0)
    , m_cacheHits(0)
    , m_cacheMisses(0)
    , m_avgLatency(0.0f)
    , m_minLatency(std::numeric_limits<float>::max())
    , m_maxLatency(0.0f)
    , m_latencySampleCount(0)
{
}

void CacheAnalyzer::setConfig(const Config& config) {
    m_config = config;
}

void CacheAnalyzer::startProfiling() {
    m_isProfiling.store(true);
}

void CacheAnalyzer::stopProfiling() {
    m_isProfiling.store(false);
}

void CacheAnalyzer::recordAccess(const std::string& componentType, bool hit, bool isLinear) {
    if (!m_isProfiling.load()) {
        return;
    }

    m_totalAccesses.fetch_add(1);
    if (hit) {
        m_cacheHits.fetch_add(1);
    } else {
        m_cacheMisses.fetch_add(1);
    }

    updateAccessPattern(componentType, isLinear);
}

void CacheAnalyzer::recordLatency(float latencyNs) {
    if (!m_config.trackLatency) {
        return;
    }

    m_latencySampleCount.fetch_add(1);

    // 更新平均延迟
    uint64_t count = m_latencySampleCount.load();
    float currentAvg = m_avgLatency.load();
    float newAvg = currentAvg + (latencyNs - currentAvg) / count;
    while (!m_avgLatency.compare_exchange_weak(currentAvg, newAvg)) {
        currentAvg = m_avgLatency.load();
        newAvg = currentAvg + (latencyNs - currentAvg) / count;
    }

    // 更新最小延迟
    float currentMin = m_minLatency.load();
    while (latencyNs < currentMin && !m_minLatency.compare_exchange_weak(currentMin, latencyNs)) {}

    // 更新最大延迟
    float currentMax = m_maxLatency.load();
    while (latencyNs > currentMax && !m_maxLatency.compare_exchange_weak(currentMax, latencyNs)) {}
}

void CacheAnalyzer::clear() {
    m_totalAccesses.store(0);
    m_cacheHits.store(0);
    m_cacheMisses.store(0);
    m_avgLatency.store(0.0f);
    m_minLatency.store(std::numeric_limits<float>::max());
    m_maxLatency.store(0.0f);
    m_latencySampleCount.store(0);

    std::lock_guard<std::mutex> lock(m_mutex);
    m_accessPatterns.clear();
}

CacheStats CacheAnalyzer::getCacheStats() {
    CacheStats stats;

    stats.totalAccesses = m_totalAccesses.load();
    stats.cacheHits = m_cacheHits.load();
    stats.cacheMisses = m_cacheMisses.load();
    stats.hitRate = stats.totalAccesses > 0 ?
                   (static_cast<float>(stats.cacheHits) / static_cast<float>(stats.totalAccesses)) : 0.0f;

    // 估算未命中类型（简化版）
    stats.coldMisses = static_cast<uint64_t>(stats.cacheMisses * 0.3f);
    stats.capacityMisses = static_cast<uint64_t>(stats.cacheMisses * 0.5f);
    stats.conflictMisses = static_cast<uint64_t>(stats.cacheMisses * 0.2f);

    // 获取访问模式
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        stats.patterns.reserve(m_accessPatterns.size());
        for (const auto& pair : m_accessPatterns) {
            stats.patterns.push_back(pair.second);
        }
    }

    // 延迟统计
    if (m_config.trackLatency) {
        stats.avgAccessLatency = m_avgLatency.load();
        stats.minAccessLatency = m_minLatency.load() == std::numeric_limits<float>::max() ?
                                 0.0f : m_minLatency.load();
        stats.maxAccessLatency = m_maxLatency.load();
    }

    return stats;
}

std::string CacheAnalyzer::generateReport(const CacheStats& stats) const {
    std::ostringstream oss;

    oss << "=== Cache Analysis Report ===\n\n";
    oss << "Total Accesses: " << stats.totalAccesses << "\n";
    oss << "Cache Hits: " << stats.cacheHits << "\n";
    oss << "Cache Misses: " << stats.cacheMisses << "\n";
    oss << "Hit Rate: " << std::fixed << std::setprecision(2)
        << (stats.hitRate * 100.0f) << "%\n\n";

    oss << "Miss Types:\n";
    oss << "  Cold Misses: " << stats.coldMisses << "\n";
    oss << "  Capacity Misses: " << stats.capacityMisses << "\n";
    oss << "  Conflict Misses: " << stats.conflictMisses << "\n\n";

    if (m_config.trackLatency && stats.totalAccesses > 0) {
        oss << "Access Latency:\n";
        oss << "  Average: " << std::fixed << std::setprecision(2)
            << stats.avgAccessLatency << " ns\n";
        oss << "  Minimum: " << stats.minAccessLatency << " ns\n";
        oss << "  Maximum: " << stats.maxAccessLatency << " ns\n\n";
    }

    if (!stats.patterns.empty()) {
        oss << "--- Access Patterns ---\n";
        oss << std::left << std::setw(25) << "Component Type"
                  << std::right << std::setw(15) << "Linear"
                  << std::setw(15) << "Random"
                  << std::setw(15) << "Sequential"
                  << std::setw(15) << "Locality"
                  << "\n";

        oss << std::string(85, '-') << "\n";

        for (const auto& pattern : stats.patterns) {
            oss << std::left << std::setw(25) << pattern.componentType
                  << std::right << std::setw(15) << pattern.linearAccesses
                  << std::setw(15) << pattern.randomAccesses
                  << std::setw(15) << pattern.sequentialAccesses
                  << std::fixed << std::setprecision(3)
                  << std::setw(15) << pattern.localityScore
                  << "\n";
        }
        oss << "\n";
    }

    oss << "=============================\n";

    return oss.str();
}

void CacheAnalyzer::printReport(const CacheStats& stats) const {
    std::cout << generateReport(stats);
}

std::vector<CacheOptimizationSuggestion> CacheAnalyzer::suggestOptimizations(const CacheStats& stats) const {
    std::vector<CacheOptimizationSuggestion> suggestions;

    // 整体命中率分析
    if (stats.hitRate < 0.7f) {
        CacheOptimizationSuggestion suggestion;
        suggestion.componentType = "Overall";
        suggestion.suggestion = "Low cache hit rate (< 70%). Consider improving data locality or increasing cache size.";
        suggestion.priority = 8;
        suggestion.expectedImprovement = 0.3f;
        suggestions.push_back(suggestion);
    } else if (stats.hitRate < 0.85f) {
        CacheOptimizationSuggestion suggestion;
        suggestion.componentType = "Overall";
        suggestion.suggestion = "Moderate cache hit rate (70-85%). Monitor for patterns and consider optimization.";
        suggestion.priority = 5;
        suggestion.expectedImprovement = 0.15f;
        suggestions.push_back(suggestion);
    }

    // 访问模式分析
    for (const auto& pattern : stats.patterns) {
        if (pattern.linearAccesses > pattern.randomAccesses * 2) {
            CacheOptimizationSuggestion suggestion;
            suggestion.componentType = pattern.componentType;
            suggestion.suggestion = "Highly linear access pattern. Consider prefetching or loop unrolling.";
            suggestion.priority = 6;
            suggestion.expectedImprovement = 0.2f;
            suggestions.push_back(suggestion);
        }

        if (pattern.randomAccesses > pattern.linearAccesses * 2) {
            CacheOptimizationSuggestion suggestion;
            suggestion.componentType = pattern.componentType;
            suggestion.suggestion = "Highly random access pattern. Consider using cache-friendly data structures or hashing.";
            suggestion.priority = 7;
            suggestion.expectedImprovement = 0.25f;
            suggestions.push_back(suggestion);
        }

        if (pattern.localityScore < 0.5f) {
            CacheOptimizationSuggestion suggestion;
            suggestion.componentType = pattern.componentType;
            suggestion.suggestion = "Poor locality detected. Reorganize data layout for better cache utilization.";
            suggestion.priority = 8;
            suggestion.expectedImprovement = 0.3f;
            suggestions.push_back(suggestion);
        }
    }

    // 延迟分析
    if (m_config.trackLatency && stats.totalAccesses > 0) {
        float variance = stats.maxAccessLatency - stats.minAccessLatency;
        if (variance > stats.avgAccessLatency * 2.0f) {
            CacheOptimizationSuggestion suggestion;
            suggestion.componentType = "Overall";
            suggestion.suggestion = "High latency variance detected. Investigate access patterns and potential locks.";
            suggestion.priority = 7;
            suggestion.expectedImprovement = 0.2f;
            suggestions.push_back(suggestion);
        }
    }

    // 按优先级排序
    std::sort(suggestions.begin(), suggestions.end(),
        [](const CacheOptimizationSuggestion& a, const CacheOptimizationSuggestion& b) {
            return a.priority > b.priority;
        });

    return suggestions;
}

bool CacheAnalyzer::exportReport(const CacheStats& stats, const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << generateReport(stats);

    // 添加优化建议
    auto suggestions = suggestOptimizations(stats);
    if (!suggestions.empty()) {
        file << "\n=== Optimization Suggestions ===\n\n";

        for (const auto& suggestion : suggestions) {
            file << "Component: " << suggestion.componentType << "\n";
            file << "Priority: " << suggestion.priority << "/10\n";
            file << "Expected Improvement: " << std::fixed << std::setprecision(0)
                << (suggestion.expectedImprovement * 100.0f) << "%\n";
            file << "Suggestion: " << suggestion.suggestion << "\n\n";
        }
    }

    file.close();
    return true;
}

std::string CacheAnalyzer::exportToJson(const CacheStats& stats) const {
    std::ostringstream oss;

    oss << "{\n";
    oss << "  \"totalAccesses\": " << stats.totalAccesses << ",\n";
    oss << "  \"cacheHits\": " << stats.cacheHits << ",\n";
    oss << "  \"cacheMisses\": " << stats.cacheMisses << ",\n";
    oss << "  \"hitRate\": " << stats.hitRate << ",\n";
    oss << "  \"coldMisses\": " << stats.coldMisses << ",\n";
    oss << "  \"capacityMisses\": " << stats.capacityMisses << ",\n";
    oss << "  \"conflictMisses\": " << stats.conflictMisses << ",\n";

    if (m_config.trackLatency) {
        oss << "  \"avgAccessLatency\": " << stats.avgAccessLatency << ",\n";
        oss << "  \"minAccessLatency\": " << stats.minAccessLatency << ",\n";
        oss << "  \"maxAccessLatency\": " << stats.maxAccessLatency << ",\n";
    }

    oss << "  \"accessPatterns\": [\n";
    for (size_t i = 0; i < stats.patterns.size(); ++i) {
        const auto& pattern = stats.patterns[i];
        oss << "    {\n";
        oss << "      \"componentType\": \"" << pattern.componentType << "\",\n";
        oss << "      \"linearAccesses\": " << pattern.linearAccesses << ",\n";
        oss << "      \"randomAccesses\": " << pattern.randomAccesses << ",\n";
        oss << "      \"sequentialAccesses\": " << pattern.sequentialAccesses << ",\n";
        oss << "      \"localityScore\": " << pattern.localityScore << "\n";
        oss << "    }";
        if (i < stats.patterns.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    oss << "  ]\n";
    oss << "}";

    return oss.str();
}

void CacheAnalyzer::updateAccessPattern(const std::string& componentType, bool isLinear) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_accessPatterns.find(componentType);
    if (it == m_accessPatterns.end()) {
        AccessPattern pattern;
        pattern.componentType = componentType;
        pattern.linearAccesses = 0;
        pattern.randomAccesses = 0;
        pattern.sequentialAccesses = 0;
        pattern.localityScore = 0.0f;
        it = m_accessPatterns.insert({componentType, pattern}).first;
    }

    auto& pattern = it->second;
    if (isLinear) {
        pattern.linearAccesses++;
    } else {
        pattern.randomAccesses++;
    }

    // 更新访问序列
    uint64_t timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    pattern.accessSequence.push_back(timestamp);

    // 限制序列长度
    if (pattern.accessSequence.size() > m_config.accessSequenceLength) {
        pattern.accessSequence.erase(pattern.accessSequence.begin());
    }

    // 更新局部性分数
    pattern.localityScore = calculateLocalityScore(pattern.accessSequence);

    // 分类访问类型
    AccessType type = classifyAccess(pattern.accessSequence);
    if (type == AccessType::Sequential) {
        pattern.sequentialAccesses++;
    }
}

float CacheAnalyzer::calculateLocalityScore(const std::vector<uint64_t>& sequence) const {
    if (sequence.size() < 2) {
        return 1.0f;
    }

    // 计算相邻时间差
    std::vector<uint64_t> diffs;
    for (size_t i = 1; i < sequence.size(); ++i) {
        diffs.push_back(sequence[i] - sequence[i - 1]);
    }

    if (diffs.empty()) {
        return 1.0f;
    }

    // 计算方差（作为局部性的度量）
    uint64_t sum = 0;
    for (auto diff : diffs) {
        sum += diff;
    }
    uint64_t avg = sum / diffs.size();

    uint64_t variance = 0;
    for (auto diff : diffs) {
        variance += (diff - avg) * (diff - avg);
    }
    variance /= diffs.size();

    // 方差越小，局部性越好
    float normalizedVariance = std::min(variance / (1000000.0f * 1000000.0f), 1.0f);
    return 1.0f - normalizedVariance;
}

CacheAnalyzer::AccessType CacheAnalyzer::classifyAccess(const std::vector<uint64_t>& sequence) const {
    if (sequence.size() < 3) {
        return AccessType::Linear;
    }

    // 计算时间差
    std::vector<uint64_t> diffs;
    for (size_t i = 1; i < sequence.size(); ++i) {
        diffs.push_back(sequence[i] - sequence[i - 1]);
    }

    // 检查是否为顺序访问（时间差相对恒定）
    uint64_t avg = 0;
    for (auto diff : diffs) {
        avg += diff;
    }
    avg /= diffs.size();

    size_t consistentCount = 0;
    for (auto diff : diffs) {
        if (diff >= avg * 0.8 && diff <= avg * 1.2) {
            consistentCount++;
        }
    }

    float consistencyRatio = static_cast<float>(consistentCount) / diffs.size();

    if (consistencyRatio > 0.8f) {
        return AccessType::Sequential;
    }

    return AccessType::Linear;
}

END_NAMESPACE_ECS
