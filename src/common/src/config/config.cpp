#include "common/config/config.h"
#include <sstream>
#include <algorithm>

BEGIN_NAMESPACE_COMMON
namespace config {

std::vector<std::string> Config::split_path(const std::string& path) {
    std::vector<std::string> result;
    std::string current;
    std::istringstream iss(path);

    while (std::getline(iss, current, '.')) {
        if (!current.empty()) {
            result.push_back(current);
        }
    }

    return result;
}

void Config::set(const std::string& key, const ConfigValue& value) {
    auto path = split_path(key);

    if (path.empty()) {
        return;
    }

    if (path.size() == 1) {
        // 顶层键，直接设置
        data_[path[0]] = value;
        return;
    }

    // 嵌套路径，需要递归创建/查找
    std::unordered_map<std::string, ConfigValue>* current_level = &data_;

    for (size_t i = 0; i < path.size() - 1; ++i) {
        const std::string& segment = path[i];
        auto it = current_level->find(segment);

        if (it == current_level->end()) {
            // 创建新的嵌套 Config
            auto new_config = std::make_shared<Config>();
            (*current_level)[segment] = ConfigValue(new_config);
            current_level = &new_config->data();
        } else {
            // 检查是否为 Config 类型
            if (!std::holds_alternative<std::shared_ptr<Config>>(it->second)) {
                // 覆盖为新的 Config
                auto new_config = std::make_shared<Config>();
                it->second = ConfigValue(new_config);
            }
            current_level = &std::get<std::shared_ptr<Config>>(it->second)->data();
        }
    }

    // 设置最终值
    (*current_level)[path.back()] = value;
}

bool Config::has(const std::string& key) const {
    return find_value(key) != nullptr;
}

std::optional<Config> Config::get_sub_config(const std::string& key) const {
    auto value = find_value(key);
    if (!value) {
        return std::nullopt;
    }

    if (!std::holds_alternative<std::shared_ptr<Config>>(*value)) {
        return std::nullopt;
    }

    auto config_ptr = std::get<std::shared_ptr<Config>>(*value);
    if (!config_ptr) {
        return std::nullopt;
    }

    return *config_ptr;
}

void Config::merge(const Config& other) {
    for (const auto& [key, value] : other.data_) {
        // 检查是否两个都是 Config 类型，可以递归合并
        auto it = data_.find(key);
        if (it != data_.end() &&
            std::holds_alternative<std::shared_ptr<Config>>(it->second) &&
            std::holds_alternative<std::shared_ptr<Config>>(value)) {
            // 递归合并嵌套配置
            auto my_config = std::get<std::shared_ptr<Config>>(it->second);
            auto other_config = std::get<std::shared_ptr<Config>>(value);
            if (my_config && other_config) {
                my_config->merge(*other_config);
            }
        } else {
            // 直接覆盖
            data_[key] = value;
        }
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
    auto path = split_path(key);

    if (path.empty()) {
        return nullptr;
    }

    if (path.size() == 1) {
        // 顶层键
        auto it = data_.find(path[0]);
        if (it != data_.end()) {
            return &it->second;
        }
        return nullptr;
    }

    // 嵌套路径，需要递归查找
    const std::unordered_map<std::string, ConfigValue>* current_level = &data_;

    for (size_t i = 0; i < path.size() - 1; ++i) {
        const std::string& segment = path[i];
        auto it = current_level->find(segment);

        if (it == current_level->end()) {
            return nullptr;
        }

        if (!std::holds_alternative<std::shared_ptr<Config>>(it->second)) {
            return nullptr;
        }

        auto config_ptr = std::get<std::shared_ptr<Config>>(it->second);
        if (!config_ptr) {
            return nullptr;
        }

        current_level = &config_ptr->data();
    }

    // 查找最终值
    auto it = current_level->find(path.back());
    if (it != current_level->end()) {
        return &it->second;
    }

    return nullptr;
}

ConfigValue* Config::find_value(const std::string& key) {
    auto path = split_path(key);

    if (path.empty()) {
        return nullptr;
    }

    if (path.size() == 1) {
        // 顶层键
        auto it = data_.find(path[0]);
        if (it != data_.end()) {
            return &it->second;
        }
        return nullptr;
    }

    // 嵌套路径，需要递归查找
    std::unordered_map<std::string, ConfigValue>* current_level = &data_;

    for (size_t i = 0; i < path.size() - 1; ++i) {
        const std::string& segment = path[i];
        auto it = current_level->find(segment);

        if (it == current_level->end()) {
            return nullptr;
        }

        if (!std::holds_alternative<std::shared_ptr<Config>>(it->second)) {
            return nullptr;
        }

        auto config_ptr = std::get<std::shared_ptr<Config>>(it->second);
        if (!config_ptr) {
            return nullptr;
        }

        current_level = &config_ptr->data();
    }

    // 查找最终值
    auto it = current_level->find(path.back());
    if (it != current_level->end()) {
        return &it->second;
    }

    return nullptr;
}

} // namespace config
END_NAMESPACE_COMMON
