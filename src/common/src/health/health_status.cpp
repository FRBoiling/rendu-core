//
// Created by 沸腾 on 2026/2/2.
//

#include "common/health/health_status.h"
#include <sstream>

using namespace COMMON_NAMESPACE;

// ============================================================================
// HealthStatus 实现
// ============================================================================

std::string HealthStatus::to_json() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"healthy\": " << (healthy ? "true" : "false") << ",\n";
    oss << "  \"status\": \"" << status << "\",\n";
    oss << "  \"checks\": {\n";

    bool first = true;
    for (const auto& [name, passed] : checks) {
        if (!first) {
            oss << ",\n";
        }
        oss << "    \"" << name << "\": " << (passed ? "true" : "false");
        first = false;
    }

    oss << "\n  },\n";
    oss << "  \"details\": {\n";

    first = true;
    for (const auto& [name, detail] : details) {
        if (!first) {
            oss << ",\n";
        }
        oss << "    \"" << name << "\": \"" << detail << "\"";
        first = false;
    }

    oss << "\n  }\n";
    oss << "}";
    return oss.str();
}

std::string HealthStatus::to_text() const {
    std::ostringstream oss;
    oss << "Health Status: " << status << "\n";
    oss << "Overall: " << (healthy ? "HEALTHY" : "UNHEALTHY") << "\n";
    oss << "\nChecks:\n";

    for (const auto& [name, passed] : checks) {
        oss << "  [" << (passed ? "✓" : "✗") << "] " << name;
        auto it = details.find(name);
        if (it != details.end()) {
            oss << " - " << it->second;
        }
        oss << "\n";
    }

    return oss.str();
}
