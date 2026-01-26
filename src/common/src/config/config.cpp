#include "common/config/config.h"
#include <sstream>
#include <algorithm>

BEGIN_NAMESPACE_COMMON
namespace config {

void Config::set(const std::string& key, const ConfigValue& value) {
    // 简化实现：仅支持顶层键
    // 点号分隔的键会被当作完整键名处理
    data_[key] = value;
}

bool Config::has(const std::string& key) const {
    return find_value(key) != nullptr;
}

void Config::merge(const Config& other) {
    for (const auto& [key, value] : other.data_) {
        // 简化：直接覆盖
        data_[key] = value;
    }
}

std::vector<std::string> Config::keys() const {
    std::vector<std::string> result;
    result.reserve(data_.size());
    for (const auto& [key, _] : data_) {
        result.push_back(key);
    }
    return result;
}

const ConfigValue* Config::find_value(const std::string& key) const {
    auto it = data_.find(key);
    if (it != data_.end()) {
        return &it->second;
    }
    return nullptr;
}

ConfigValue* Config::find_value(const std::string& key) {
    auto it = data_.find(key);
    if (it != data_.end()) {
        return &it->second;
    }
    return nullptr;
}

} // namespace config
END_NAMESPACE_COMMON
