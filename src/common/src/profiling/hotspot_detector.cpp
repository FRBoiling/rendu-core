//
// Hotspot Detector 实现
//

#include "common/profiling/hotspot_detector.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <cmath>

BEGIN_NAMESPACE_ECS

HotspotDetector::HotspotDetector()
    : m_config()
    , m_isDetecting(false)
    , m_nextOperationId(0)
{
}

HotspotDetector::~HotspotDetector() {
    stopDetection();
}

void HotspotDetector::setConfig(const Config& config) {
    m_config = config;
}

void HotspotDetector::startDetection() {
    m_isDetecting.store(true);
}

void HotspotDetector::stopDetection() {
    m_isDetecting.store(false);
}

uint64_t HotspotDetector::beginOperation(const std::string& name, const std::string& category) {
    if (!m_isDetecting.load()) {
        return 0;
    }

    uint64_t operationId = m_nextOperationId.fetch_add(1) + 1;

    OperationRecord record;
    record.name = name;
    record.category = category.empty() ? "General" : category;
    record.startTime = std::chrono::steady_clock::now();

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_operationRecords[operationId] = std::move(record);
    }

    return operationId;
}

void HotspotDetector::endOperation(uint64_t operationId) {
    if (operationId == 0) {
        return;
    }

    std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();

    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_operationRecords.find(operationId);
    if (it != m_operationRecords.end()) {
        auto elapsed = std::chrono::duration<float, std::milli>(endTime - it->second.startTime).count();
        updateStats(it->second.name, it->second.category, elapsed);
        m_operationRecords.erase(it);
    }
}

HotspotDetector::ScopeTimer::ScopeTimer(HotspotDetector& detector, const std::string& name, const std::string& category)
    : m_detector(detector)
    , m_operationId(0)
{
    m_operationId = m_detector.beginOperation(name, category);
}

HotspotDetector::ScopeTimer::~ScopeTimer() {
    m_detector.endOperation(m_operationId);
}

std::vector<Hotspot> HotspotDetector::getHotspots() {
    return detectHotspots(m_config.hotspotThreshold, false);
}

std::vector<Hotspot> HotspotDetector::getCriticalHotspots() {
    return detectHotspots(m_config.criticalThreshold, true);
}

std::vector<Hotspot> HotspotDetector::getAllOperations() {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::vector<Hotspot> allOps;
    float totalTime = 0.0f;

    // 先计算总时间
    for (const auto& pair : m_operationStats) {
        totalTime += pair.second->totalTime.load();
    }

    // 生成所有操作统计
    for (const auto& pair : m_operationStats) {
        const auto& stats = *pair.second;
        Hotspot hotspot;
        hotspot.name = stats.name;
        hotspot.category = stats.category;
        hotspot.callCount = stats.callCount.load();
        hotspot.totalTime = stats.totalTime.load();
        hotspot.avgTime = hotspot.callCount > 0 ? hotspot.totalTime / hotspot.callCount : 0.0f;
        hotspot.minTime = stats.minTime.load() == std::numeric_limits<float>::max() ? 0.0f : stats.minTime.load();
        hotspot.maxTime = stats.maxTime.load();
        hotspot.percentage = totalTime > 0 ? (hotspot.totalTime / totalTime * 100.0f) : 0.0f;
        hotspot.isCritical = hotspot.totalTime >= m_config.criticalThreshold;

        if (m_config.enableStackTrace && hotspot.isCritical) {
            hotspot.stackTrace = generateStackTrace();
        }

        allOps.push_back(std::move(hotspot));
    }

    // 按总时间排序
    std::sort(allOps.begin(), allOps.end(), [](const Hotspot& a, const Hotspot& b) {
        return a.totalTime > b.totalTime;
    });

    return allOps;
}

std::vector<Hotspot> HotspotDetector::getHotspotsByCategory(const std::string& category) {
    auto all = getAllOperations();
    std::vector<Hotspot> filtered;

    for (const auto& hotspot : all) {
        if (hotspot.category == category) {
            filtered.push_back(hotspot);
        }
    }

    return filtered;
}

void HotspotDetector::clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_operationRecords.clear();
    m_operationStats.clear();
    m_nextOperationId.store(0);
}

void HotspotDetector::printReport() const {
    std::cout << "\n=== Hotspot Detection Report ===\n";

    auto hotspots = const_cast<HotspotDetector*>(this)->detectHotspots(m_config.hotspotThreshold, false);

    if (hotspots.empty()) {
        std::cout << "No hotspots detected.\n";
    } else {
        std::cout << "Detected " << hotspots.size() << " hotspots:\n\n";

        // 打印表头
        std::cout << std::left << std::setw(30) << "Operation"
                  << std::setw(20) << "Category"
                  << std::right << std::setw(10) << "Calls"
                  << std::setw(12) << "Total(ms)"
                  << std::setw(12) << "Avg(ms)"
                  << std::setw(12) << "Min(ms)"
                  << std::setw(12) << "Max(ms)"
                  << std::setw(10) << "Percent"
                  << "\n";

        std::cout << std::string(118, '-') << "\n";

        // 打印每个热点
        for (const auto& hotspot : hotspots) {
            std::cout << std::left << std::setw(30) << hotspot.name
                      << std::setw(20) << hotspot.category
                      << std::right << std::setw(10) << hotspot.callCount
                      << std::fixed << std::setprecision(3)
                      << std::setw(12) << hotspot.totalTime
                      << std::setw(12) << hotspot.avgTime
                      << std::setw(12) << hotspot.minTime
                      << std::setw(12) << hotspot.maxTime
                      << std::setprecision(1)
                      << std::setw(9) << hotspot.percentage << "%";

            if (hotspot.isCritical) {
                std::cout << " [CRITICAL]";
            }
            std::cout << "\n";
        }
    }

    std::cout << "\n================================\n";
}

bool HotspotDetector::exportReport(const std::string& filename, ReportFormat format) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    auto hotspots = const_cast<HotspotDetector*>(this)->detectHotspots(m_config.hotspotThreshold, false);

    switch (format) {
        case ReportFormat::Text: {
            file << "=== Hotspot Detection Report ===\n";
            file << "Detected " << hotspots.size() << " hotspots:\n\n";

            file << std::left << std::setw(30) << "Operation"
                  << std::setw(20) << "Category"
                  << std::right << std::setw(10) << "Calls"
                  << std::setw(12) << "Total(ms)"
                  << std::setw(12) << "Avg(ms)"
                  << std::setw(12) << "Min(ms)"
                  << std::setw(12) << "Max(ms)"
                  << std::setw(10) << "Percent"
                  << "\n";

            file << std::string(118, '-') << "\n";

            for (const auto& hotspot : hotspots) {
                file << std::left << std::setw(30) << hotspot.name
                     << std::setw(20) << hotspot.category
                     << std::right << std::setw(10) << hotspot.callCount
                     << std::fixed << std::setprecision(3)
                     << std::setw(12) << hotspot.totalTime
                     << std::setw(12) << hotspot.avgTime
                     << std::setw(12) << hotspot.minTime
                     << std::setw(12) << hotspot.maxTime
                     << std::setprecision(1)
                     << std::setw(9) << hotspot.percentage << "%";
                if (hotspot.isCritical) {
                    file << " [CRITICAL]";
                }
                file << "\n";
            }
            break;
        }

        case ReportFormat::JSON:
            file << exportToJson();
            break;

        case ReportFormat::Markdown: {
            file << "# Hotspot Detection Report\n\n";
            file << "Detected " << hotspots.size() << " hotspots.\n\n";

            file << "| Operation | Category | Calls | Total(ms) | Avg(ms) | Min(ms) | Max(ms) | Percent |\n";
            file << "|-----------|----------|-------|-----------|---------|---------|---------|---------|\n";

            for (const auto& hotspot : hotspots) {
                file << "| " << hotspot.name
                     << " | " << hotspot.category
                     << " | " << hotspot.callCount
                     << " | " << std::fixed << std::setprecision(3) << hotspot.totalTime
                     << " | " << hotspot.avgTime
                     << " | " << hotspot.minTime
                     << " | " << hotspot.maxTime
                     << " | " << std::setprecision(1) << hotspot.percentage << "%";
                if (hotspot.isCritical) {
                    file << " **[CRITICAL]**";
                }
                file << " |\n";
            }
            break;
        }
    }

    file.close();
    return true;
}

std::string HotspotDetector::exportToJson() const {
    std::ostringstream oss;
    auto hotspots = const_cast<HotspotDetector*>(this)->detectHotspots(m_config.hotspotThreshold, false);

    oss << "{\n";
    oss << "  \"hotspots\": [\n";

    for (size_t i = 0; i < hotspots.size(); ++i) {
        const auto& hotspot = hotspots[i];
        oss << "    {\n";
        oss << "      \"name\": \"" << hotspot.name << "\",\n";
        oss << "      \"category\": \"" << hotspot.category << "\",\n";
        oss << "      \"callCount\": " << hotspot.callCount << ",\n";
        oss << "      \"totalTime\": " << hotspot.totalTime << ",\n";
        oss << "      \"avgTime\": " << hotspot.avgTime << ",\n";
        oss << "      \"minTime\": " << hotspot.minTime << ",\n";
        oss << "      \"maxTime\": " << hotspot.maxTime << ",\n";
        oss << "      \"percentage\": " << hotspot.percentage << ",\n";
        oss << "      \"isCritical\": " << (hotspot.isCritical ? "true" : "false") << "\n";
        oss << "    }";
        if (i < hotspots.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }

    oss << "  ]\n";
    oss << "}";

    return oss.str();
}

std::vector<std::string> HotspotDetector::getOptimizationSuggestions() const {
    std::vector<std::string> suggestions;
    auto hotspots = const_cast<HotspotDetector*>(this)->detectHotspots(m_config.hotspotThreshold, false);

    if (hotspots.empty()) {
        suggestions.push_back("No hotspots detected. System performance is good.");
        return suggestions;
    }

    // 分析热点并生成建议
    std::unordered_map<std::string, std::vector<Hotspot>> byCategory;
    for (const auto& hotspot : hotspots) {
        byCategory[hotspot.category].push_back(hotspot);
    }

    for (const auto& pair : byCategory) {
        const auto& categoryHotspots = pair.second;
        float totalTime = 0.0f;
        for (const auto& h : categoryHotspots) {
            totalTime += h.totalTime;
        }

        if (totalTime > m_config.criticalThreshold * categoryHotspots.size()) {
            suggestions.push_back("[" + pair.first + "] Category has multiple critical hotspots. Consider refactoring or caching.");
        }

        // 单个热点优化建议
        for (const auto& hotspot : categoryHotspots) {
            if (hotspot.isCritical) {
                if (hotspot.avgTime > 1.0f) {
                    suggestions.push_back("[" + hotspot.name + "] High average time. Consider algorithm optimization.");
                }
                if (hotspot.callCount < m_config.minCallCount && hotspot.totalTime > m_config.criticalThreshold) {
                    suggestions.push_back("[" + hotspot.name + "] High total time with few calls. Single operation is very slow. Focus on optimizing this specific operation.");
                }
                if (hotspot.callCount > 1000 && hotspot.avgTime > 0.1f) {
                    suggestions.push_back("[" + hotspot.name + "] Frequently called with moderate cost. Consider batching or parallelizing.");
                }
            }
        }
    }

    if (suggestions.empty()) {
        suggestions.push_back("Hotspots detected but no specific optimization strategies available.");
        suggestions.push_back("Consider profiling with more detailed tools to identify root causes.");
    }

    return suggestions;
}

void HotspotDetector::updateStats(const std::string& name, const std::string& category, float elapsedMs) {
    std::string key = category + "::" + name;

    auto it = m_operationStats.find(key);
    if (it == m_operationStats.end()) {
        it = m_operationStats.insert({key, std::make_unique<OperationStats>(name, category)}).first;
    }

    auto& stats = *it->second;
    stats.callCount.fetch_add(1);
    stats.totalTime.fetch_add(elapsedMs);

    // 更新最小时间
    float currentMin = stats.minTime.load();
    while (elapsedMs < currentMin && !stats.minTime.compare_exchange_weak(currentMin, elapsedMs)) {}

    // 更新最大时间
    float currentMax = stats.maxTime.load();
    while (elapsedMs > currentMax && !stats.maxTime.compare_exchange_weak(currentMax, elapsedMs)) {}
}

std::vector<Hotspot> HotspotDetector::detectHotspots(float threshold, bool onlyCritical) const {
    std::vector<Hotspot> results;
    float totalTime = 0.0f;

    // 计算总时间
    for (const auto& pair : m_operationStats) {
        totalTime += pair.second->totalTime.load();
    }

    // 筛选热点
    for (const auto& pair : m_operationStats) {
        const auto& stats = *pair.second;

        uint64_t callCount = stats.callCount.load();
        if (callCount < m_config.minCallCount) {
            continue;  // 跳过调用次数过少的操作
        }

        float total = stats.totalTime.load();
        float avg = callCount > 0 ? total / callCount : 0.0f;

        bool isHotspot = (total >= threshold || avg >= threshold);
        if (onlyCritical && !isHotspot) {
            continue;
        }

        if (isHotspot || !onlyCritical) {
            Hotspot hotspot;
            hotspot.name = stats.name;
            hotspot.category = stats.category;
            hotspot.callCount = callCount;
            hotspot.totalTime = total;
            hotspot.avgTime = avg;
            hotspot.minTime = stats.minTime.load() == std::numeric_limits<float>::max() ? 0.0f : stats.minTime.load();
            hotspot.maxTime = stats.maxTime.load();
            hotspot.percentage = totalTime > 0 ? (total / totalTime * 100.0f) : 0.0f;
            hotspot.isCritical = total >= m_config.criticalThreshold;

            if (m_config.enableStackTrace && hotspot.isCritical) {
                hotspot.stackTrace = generateStackTrace();
            }

            results.push_back(std::move(hotspot));
        }
    }

    // 按总时间排序
    std::sort(results.begin(), results.end(), [](const Hotspot& a, const Hotspot& b) {
        return a.totalTime > b.totalTime;
    });

    return results;
}

std::vector<std::string> HotspotDetector::generateStackTrace() const {
    // 简化版调用栈 - 实际实现可以使用平台特定的 API
    std::vector<std::string> stack;
    stack.push_back("[Stack trace not available in this build]");
    return stack;
}

END_NAMESPACE_ECS
