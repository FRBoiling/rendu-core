//
// Performance Monitor 实现
//

#include "common/ecs/performance_monitor.h"
#include "common/ecs/registry_optimized.h"
#include "common/ecs/archive.h"
#include "common/ecs/profiler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

BEGIN_NAMESPACE_ECS

PerformanceMonitor::PerformanceMonitor()
    : m_config()
    , m_frameCount(0)
    , m_accumulatedTime(0.0f)
    , m_lastUpdateTime(std::chrono::steady_clock::now())
{
    m_currentSnapshot = {};
    m_currentSnapshot.timestamp = getCurrentTimestamp();
}

PerformanceMonitor::~PerformanceMonitor() = default;

void PerformanceMonitor::setConfig(const Config& config) {
    m_config = config;

    // 如果历史数据超过新的最大值，截断历史
    if (m_history.size() > m_config.maxHistorySize) {
        m_history.resize(m_config.maxHistorySize);
    }
}

void PerformanceMonitor::update(const RegistryOptimized& registry, float deltaTime) {
    // 更新帧计数和时间
    m_frameCount++;
    m_accumulatedTime += deltaTime;

    // 计算当前时间差（毫秒）
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastUpdateTime).count();

    // 更新当前快照
    m_currentSnapshot.timestamp = getCurrentTimestamp();
    m_currentSnapshot.entityCount = registry.size();
    m_currentSnapshot.memoryUsage = 0; // 默认为 0，需要 MemoryAnalyzer 获取准确值

    // 计算 FPS 和 TPS
    // deltaTime 应该是秒为单位，m_accumulatedTime 也是秒
    if (m_accumulatedTime > 0) {
        m_currentSnapshot.fps = static_cast<float>(m_frameCount) / m_accumulatedTime;
        m_currentSnapshot.tps = static_cast<float>(m_frameCount) / m_accumulatedTime;
    }

    // 每 1000ms (1秒) 重置一次并添加历史记录
    if (elapsed >= 1000) {
        addHistory();
        m_frameCount = 0;
        m_accumulatedTime = 0.0f;
        m_lastUpdateTime = now;
    }
}

void PerformanceMonitor::update(const RegistryOptimized& registry,
                                 const MemoryAnalyzer& memoryAnalyzer,
                                 float deltaTime) {
    // 先调用基础更新
    update(registry, deltaTime);

    // 更新内存使用
    auto memoryInfo = memoryAnalyzer.analyze(registry);
    m_currentSnapshot.memoryUsage = memoryInfo.totalMemory;
}

PerformanceMonitor::Snapshot PerformanceMonitor::getCurrentSnapshot() const {
    return m_currentSnapshot;
}

const std::vector<PerformanceMonitor::Snapshot>& PerformanceMonitor::getHistory() const {
    return m_history;
}

void PerformanceMonitor::clearHistory() {
    m_history.clear();
    m_frameCount = 0;
    m_accumulatedTime = 0.0f;
    m_lastUpdateTime = std::chrono::steady_clock::now();
}

PerformanceMonitor::HistoryStats PerformanceMonitor::getHistoryStats() const {
    HistoryStats stats = {};

    if (m_history.empty()) {
        return stats;
    }

    float sumFps = 0.0f;
    float sumEntityCount = 0.0f;
    float sumMemoryUsage = 0.0f;

    stats.minFps = m_history[0].fps;
    stats.maxFps = m_history[0].fps;
    stats.peakEntityCount = m_history[0].entityCount;
    stats.peakMemoryUsage = m_history[0].memoryUsage;

    for (const auto& snapshot : m_history) {
        sumFps += snapshot.fps;
        sumEntityCount += static_cast<float>(snapshot.entityCount);
        sumMemoryUsage += static_cast<float>(snapshot.memoryUsage);

        if (snapshot.fps < stats.minFps) {
            stats.minFps = snapshot.fps;
        }
        if (snapshot.fps > stats.maxFps) {
            stats.maxFps = snapshot.fps;
        }
        if (snapshot.entityCount > stats.peakEntityCount) {
            stats.peakEntityCount = snapshot.entityCount;
        }
        if (snapshot.memoryUsage > stats.peakMemoryUsage) {
            stats.peakMemoryUsage = snapshot.memoryUsage;
        }
    }

    const float count = static_cast<float>(m_history.size());
    stats.avgFps = sumFps / count;
    stats.avgEntityCount = sumEntityCount / count;
    stats.avgMemoryUsage = static_cast<size_t>(sumMemoryUsage / count);

    return stats;
}

void PerformanceMonitor::recordOperation(const std::string& operationName, float timeMs) {
    m_currentSnapshot.operationTimes[operationName] = timeMs;
}

void PerformanceMonitor::recordOperationCount(const std::string& operationName, float count) {
    auto it = m_currentSnapshot.operationCounts.find(operationName);
    if (it != m_currentSnapshot.operationCounts.end()) {
        it->second += count;
    } else {
        m_currentSnapshot.operationCounts[operationName] = count;
    }
}

void PerformanceMonitor::resetOperationStats() {
    m_currentSnapshot.operationTimes.clear();
    m_currentSnapshot.operationCounts.clear();
}

float PerformanceMonitor::calculateAverageFps() const {
    if (m_history.empty()) {
        return m_currentSnapshot.fps;
    }

    float sum = 0.0f;
    for (const auto& snapshot : m_history) {
        sum += snapshot.fps;
    }
    return sum / static_cast<float>(m_history.size());
}

float PerformanceMonitor::calculateAverageTps() const {
    if (m_history.empty()) {
        return m_currentSnapshot.tps;
    }

    float sum = 0.0f;
    for (const auto& snapshot : m_history) {
        sum += snapshot.tps;
    }
    return sum / static_cast<float>(m_history.size());
}

void PerformanceMonitor::printCurrentSnapshot() const {
    std::cout << "\n=== Performance Monitor - Current Snapshot ===\n";
    std::cout << "Timestamp: " << m_currentSnapshot.timestamp << " ms\n";
    std::cout << "FPS: " << std::fixed << std::setprecision(2) << m_currentSnapshot.fps << "\n";
    std::cout << "TPS: " << m_currentSnapshot.tps << "\n";
    std::cout << "Entity Count: " << m_currentSnapshot.entityCount << "\n";
    std::cout << "Memory Usage: " << (m_currentSnapshot.memoryUsage / 1024) << " KB\n";

    if (!m_currentSnapshot.operationTimes.empty()) {
        std::cout << "\nOperation Times:\n";
        for (const auto& pair : m_currentSnapshot.operationTimes) {
            std::cout << "  " << pair.first << ": " << std::fixed << std::setprecision(4)
                      << pair.second << " ms\n";
        }
    }

    if (!m_currentSnapshot.operationCounts.empty()) {
        std::cout << "\nOperation Counts:\n";
        for (const auto& pair : m_currentSnapshot.operationCounts) {
            std::cout << "  " << pair.first << ": " << std::fixed << std::setprecision(0)
                      << pair.second << "\n";
        }
    }

    std::cout << "=============================================\n";
}

void PerformanceMonitor::printHistoryStats() const {
    auto stats = getHistoryStats();

    std::cout << "\n=== Performance Monitor - History Stats ===\n";
    std::cout << "FPS Statistics:\n";
    std::cout << "  Average: " << std::fixed << std::setprecision(2) << stats.avgFps << "\n";
    std::cout << "  Min: " << stats.minFps << "\n";
    std::cout << "  Max: " << stats.maxFps << "\n";

    std::cout << "\nEntity Count Statistics:\n";
    std::cout << "  Average: " << std::fixed << std::setprecision(0) << stats.avgEntityCount << "\n";
    std::cout << "  Peak: " << stats.peakEntityCount << "\n";

    std::cout << "\nMemory Usage Statistics:\n";
    std::cout << "  Average: " << (stats.avgMemoryUsage / 1024) << " KB\n";
    std::cout << "  Peak: " << (stats.peakMemoryUsage / 1024) << " KB\n";

    std::cout << "\nHistory Size: " << m_history.size() << " snapshots\n";
    std::cout << "=============================================\n";
}

bool PerformanceMonitor::exportReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << "=== Performance Monitor Report ===\n\n";

    // 写入当前快照
    file << "Current Snapshot:\n";
    file << "  Timestamp: " << m_currentSnapshot.timestamp << " ms\n";
    file << "  FPS: " << m_currentSnapshot.fps << "\n";
    file << "  TPS: " << m_currentSnapshot.tps << "\n";
    file << "  Entity Count: " << m_currentSnapshot.entityCount << "\n";
    file << "  Memory Usage: " << (m_currentSnapshot.memoryUsage / 1024) << " KB\n";

    if (!m_currentSnapshot.operationTimes.empty()) {
        file << "\n  Operation Times:\n";
        for (const auto& pair : m_currentSnapshot.operationTimes) {
            file << "    " << pair.first << ": " << pair.second << " ms\n";
        }
    }

    if (!m_currentSnapshot.operationCounts.empty()) {
        file << "\n  Operation Counts:\n";
        for (const auto& pair : m_currentSnapshot.operationCounts) {
            file << "    " << pair.first << ": " << pair.second << "\n";
        }
    }

    // 写入历史统计
    auto stats = getHistoryStats();
    file << "\nHistory Statistics:\n";
    file << "  FPS Average: " << stats.avgFps << "\n";
    file << "  FPS Min: " << stats.minFps << "\n";
    file << "  FPS Max: " << stats.maxFps << "\n";
    file << "  Entity Average: " << stats.avgEntityCount << "\n";
    file << "  Entity Peak: " << stats.peakEntityCount << "\n";
    file << "  Memory Average: " << (stats.avgMemoryUsage / 1024) << " KB\n";
    file << "  Memory Peak: " << (stats.peakMemoryUsage / 1024) << " KB\n";

    file << "\nHistory Size: " << m_history.size() << " snapshots\n";

    file.close();
    return true;
}

bool PerformanceMonitor::exportJsonReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << exportSnapshotToJson();
    file << "\n";
    file << exportHistoryToJson();

    file.close();
    return true;
}

std::string PerformanceMonitor::exportSnapshotToJson() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"currentSnapshot\": {\n";
    oss << "    \"timestamp\": " << m_currentSnapshot.timestamp << ",\n";
    oss << "    \"fps\": " << m_currentSnapshot.fps << ",\n";
    oss << "    \"tps\": " << m_currentSnapshot.tps << ",\n";
    oss << "    \"entityCount\": " << m_currentSnapshot.entityCount << ",\n";
    oss << "    \"memoryUsage\": " << m_currentSnapshot.memoryUsage << ",\n";

    if (!m_currentSnapshot.operationTimes.empty()) {
        oss << "    \"operationTimes\": {\n";
        bool first = true;
        for (const auto& pair : m_currentSnapshot.operationTimes) {
            if (!first) oss << ",\n";
            oss << "      \"" << pair.first << "\": " << pair.second;
            first = false;
        }
        oss << "\n    },\n";
    }

    if (!m_currentSnapshot.operationCounts.empty()) {
        oss << "    \"operationCounts\": {\n";
        bool first = true;
        for (const auto& pair : m_currentSnapshot.operationCounts) {
            if (!first) oss << ",\n";
            oss << "      \"" << pair.first << "\": " << pair.second;
            first = false;
        }
        oss << "\n    }\n";
    } else {
        oss << "    \"operationTimes\": {},\n";
        oss << "    \"operationCounts\": {}\n";
    }

    oss << "  }";
    return oss.str();
}

std::string PerformanceMonitor::exportHistoryToJson() const {
    std::ostringstream oss;
    oss << ",\n  \"historyStats\": {\n";

    auto stats = getHistoryStats();
    oss << "    \"avgFps\": " << stats.avgFps << ",\n";
    oss << "    \"minFps\": " << stats.minFps << ",\n";
    oss << "    \"maxFps\": " << stats.maxFps << ",\n";
    oss << "    \"avgEntityCount\": " << stats.avgEntityCount << ",\n";
    oss << "    \"peakEntityCount\": " << stats.peakEntityCount << ",\n";
    oss << "    \"avgMemoryUsage\": " << stats.avgMemoryUsage << ",\n";
    oss << "    \"peakMemoryUsage\": " << stats.peakMemoryUsage << ",\n";
    oss << "    \"historySize\": " << m_history.size() << "\n";

    oss << "  }\n}";
    return oss.str();
}

void PerformanceMonitor::addHistory() {
    m_history.push_back(m_currentSnapshot);

    // 限制历史记录大小
    if (m_history.size() > m_config.maxHistorySize) {
        m_history.erase(m_history.begin());
    }
}

uint64_t PerformanceMonitor::getCurrentTimestamp() const {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

END_NAMESPACE_ECS
