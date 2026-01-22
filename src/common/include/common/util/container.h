#pragma once

#include "common/define.h"
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <algorithm>

BEGIN_NAMESPACE_COMMON
namespace container {

/**
 * @brief 检查容器是否包含指定元素
 * @tparam C 容器类型
 * @tparam T 元素类型
 * @param container 容器
 * @param value 值
 * @return 是否包含
 */
template<typename C, typename T>
bool contains(const C& container, const T& value) {
    return std::find(container.begin(), container.end(), value) != container.end();
}

/**
 * @brief 条件删除容器元素
 * @tparam C 容器类型
 * @tparam Pred 谓词类型
 * @param container 容器
 * @param pred 谓词
 */
template<typename C, typename Pred>
void erase_if(C& container, Pred pred) {
#if __cpp_lib_erase_if >= 202002L
    std::erase_if(container, pred);
#else
    container.erase(
        std::remove_if(container.begin(), container.end(), pred),
        container.end()
    );
#endif
}

/**
 * @brief 查找键或返回默认值
 * @tparam M 映射容器类型
 * @tparam K 键类型
 * @tparam V 值类型
 * @param map 映射容器
 * @param key 键
 * @param default_value 默认值
 * @return 找到的值或默认值
 */
template<typename M, typename K, typename V>
auto find_or(const M& map, const K& key, const V& default_value) -> V {
    auto it = map.find(key);
    return (it != map.end()) ? it->second : default_value;
}

/**
 * @brief 获取映射容器的所有键
 * @tparam M 映射容器类型
 * @param map 映射容器
 * @return 键数组
 */
template<typename M>
auto map_keys(const M& map) -> std::vector<typename M::key_type> {
    std::vector<typename M::key_type> keys;
    keys.reserve(map.size());
    for (const auto& [k, v] : map) {
        keys.push_back(k);
    }
    return keys;
}

/**
 * @brief 获取映射容器的所有值
 * @tparam M 映射容器类型
 * @param map 映射容器
 * @return 值数组
 */
template<typename M>
auto map_values(const M& map) -> std::vector<typename M::mapped_type> {
    std::vector<typename M::mapped_type> values;
    values.reserve(map.size());
    for (const auto& [k, v] : map) {
        values.push_back(v);
    }
    return values;
}

} // namespace container
END_NAMESPACE_COMMON
