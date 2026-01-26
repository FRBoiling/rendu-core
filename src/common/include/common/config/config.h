#pragma once

#include "common/define.h"
#include "common/util/error.h"
#include <string>
#include <variant>
#include <unordered_map>
#include <vector>
#include <memory>

BEGIN_NAMESPACE_COMMON
namespace config {

/**
 * @brief 配置值类型，支持多种数据类型
 *
 * 注意：为了简化实现，数组和对象中的值限制为基本类型
 */
using ConfigValue = std::variant<
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int64_t>,
    std::vector<double>,
    std::vector<bool>,
    std::vector<std::string>,
    std::unordered_map<std::string, int64_t>,
    std::unordered_map<std::string, double>,
    std::unordered_map<std::string, bool>,
    std::unordered_map<std::string, std::string>
>;

/**
 * @brief 配置管理类
 *
 * 提供类型安全的配置访问接口，支持嵌套配置结构
 */
class Config {
public:
    Config() = default;
    ~Config() = default;

    /**
     * @brief 获取配置值（指定类型）
     * @tparam T 目标类型（int64_t, double, bool, std::string）
     * @param key 配置键，支持点号分隔的嵌套路径（如 "database.port"）
     * @return Result<T> 包含配置值或错误信息
     */
    template<typename T>
    Result<T> get(const std::string& key) const {
        auto value = find_value(key);
        if (!value) {
            return Error(ErrorCode::NotFound, "Key not found: " + key);
        }

        if (!std::holds_alternative<T>(*value)) {
            return Error(ErrorCode::InvalidArgument,
                "Type mismatch for key '" + key + "'");
        }

        return std::get<T>(*value);
    }

    /**
     * @brief 获取配置值（带默认值）
     * @tparam T 目标类型
     * @param key 配置键
     * @param default_value 默认值
     * @return 配置值或默认值
     */
    template<typename T>
    T get_or_default(const std::string& key, const T& default_value) const {
        auto result = get<T>(key);
        if (std::holds_alternative<T>(result)) {
            return std::get<T>(result);
        }
        return default_value;
    }

    /**
     * @brief 设置配置值
     * @param key 配置键，支持点号分隔的嵌套路径
     * @param value 配置值
     */
    void set(const std::string& key, const ConfigValue& value);

    /**
     * @brief 检查配置键是否存在
     * @param key 配置键
     * @return 是否存在
     */
    bool has(const std::string& key) const;

    /**
     * @brief 合并另一个配置
     * @param other 另一个配置对象
     */
    void merge(const Config& other);

    /**
     * @brief 清空所有配置
     */
    void clear() {
        data_.clear();
    }

    /**
     * @brief 获取配置项数量
     */
    size_t size() const {
        return data_.size();
    }

    /**
     * @brief 检查是否为空
     */
    bool empty() const {
        return data_.empty();
    }

    /**
     * @brief 获取所有顶层键
     */
    std::vector<std::string> keys() const;

    // 允许直接访问数据（用于测试）
    std::unordered_map<std::string, ConfigValue>& data() { return data_; }
    const std::unordered_map<std::string, ConfigValue>& data() const { return data_; }

private:
    const ConfigValue* find_value(const std::string& key) const;
    ConfigValue* find_value(const std::string& key);

    std::unordered_map<std::string, ConfigValue> data_;
};

} // namespace config
END_NAMESPACE_COMMON
