//
// Entity Browser - 实体浏览器
// 用于浏览和调试 ECS 系统中的实体和组件
//

#pragma once

#include "entity.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

#include "common/define.h"

BEGIN_NAMESPACE_ECS

// 前向声明
class RegistryOptimized;
class RelationshipManager;

/// @brief 实体浏览器 - 提供实体和组件的查询和浏览功能
class EntityBrowser {
public:
    /// @brief 组件信息
    struct ComponentInfo {
        std::string typeName;    ///< 组件类型名称
        std::string value;        ///< 组件值的字符串表示
        size_t size;              ///< 组件大小（字节）
        size_t offset;            ///< 在 Archetype 中的偏移量
    };

    /// @brief 实体信息
    struct EntityInfo {
        Entity entity;                          ///< 实体 ID
        std::vector<ComponentInfo> components;   ///< 组件列表
        Entity parent;                          ///< 父实体
        std::vector<Entity> children;           ///< 子实体列表
        bool valid;                             ///< 实体是否有效
    };

    /// @brief 搜索结果
    struct SearchResult {
        std::vector<EntityInfo> entities;   ///< 匹配的实体
        int totalMatches;                   ///< 总匹配数
        std::string query;                  ///< 搜索查询
    };

    /// @brief 统计信息
    struct Statistics {
        size_t totalEntities;                ///< 总实体数
        size_t validEntities;                ///< 有效实体数
        size_t totalComponents;              ///< 总组件数
        size_t uniqueComponentTypes;         ///< 唯一组件类型数
        std::unordered_map<std::string, size_t> componentCounts;  ///< 各类型组件数量
    };

    /// @brief 构造函数
    EntityBrowser();

    /// @brief 析构函数
    ~EntityBrowser();

    /// @brief 刷新浏览器数据
    /// @param registry 要浏览的注册表
    void refresh(const RegistryOptimized& registry);

    /// @brief 刷新浏览器数据（带关系管理器）
    /// @param registry 要浏览的注册表
    /// @param relationships 关系管理器
    void refresh(const RegistryOptimized& registry, const RelationshipManager& relationships);

    /// @brief 获取所有实体信息
    /// @return 实体信息列表
    std::vector<EntityInfo> getEntities() const;

    /// @brief 获取有效实体信息
    /// @return 有效实体信息列表
    std::vector<EntityInfo> getValidEntities() const;

    /// @brief 根据实体 ID 获取实体信息
    /// @param entity 实体 ID
    /// @return 实体信息，如果不存在则返回空 EntityInfo
    EntityInfo getEntityInfo(Entity entity) const;

    /// @brief 获取实体的组件信息
    /// @param entity 实体 ID
    /// @return 组件信息列表
    std::vector<ComponentInfo> getComponents(Entity entity) const;

    /// @brief 搜索实体（按组件类型）
    /// @param componentType 组件类型名称
    /// @return 匹配的实体列表
    std::vector<EntityInfo> searchByComponent(const std::string& componentType) const;

    /// @brief 搜索实体（按组件值）
    /// @param componentType 组件类型名称
    /// @param valuePattern 组件值模式（简单字符串匹配）
    /// @return 匹配的实体列表
    std::vector<EntityInfo> searchByValue(const std::string& componentType,
                                           const std::string& valuePattern) const;

    /// @brief 高级搜索
    /// @param filter 过滤条件函数，返回 true 表示匹配
    /// @return 匹配的实体列表
    std::vector<EntityInfo> advancedSearch(const std::function<bool(const EntityInfo&)>& filter) const;

    /// @brief 获取统计信息
    /// @return 统计信息
    Statistics getStatistics() const;

    /// @brief 打印实体信息（用于调试）
    /// @param info 实体信息
    static void printEntityInfo(const EntityInfo& info);

    /// @brief 打印所有实体信息
    void printAllEntities() const;

    /// @brief 打印统计信息
    void printStatistics() const;

    /// @brief 导出实体信息为 JSON 格式字符串
    /// @param entity 实体 ID
    /// @return JSON 字符串
    std::string exportEntityToJson(Entity entity) const;

    /// @brief 导出所有实体为 JSON 格式字符串
    /// @return JSON 字符串
    std::string exportAllToJson() const;

    /// @brief 导出统计信息为 JSON 格式字符串
    /// @return JSON 字符串
    std::string exportStatisticsToJson() const;

private:
    /// @brief 实体信息缓存
    std::unordered_map<uint32, EntityInfo> m_entityCache;

    /// @brief 统计信息
    Statistics m_statistics;

    /// @brief 是否已刷新
    bool m_refreshed;

    /// @brief 注册表指针（用于延迟刷新）
    const RegistryOptimized* m_registry;
    const RelationshipManager* m_relationships;

    /// @brief 更新统计信息
    void updateStatistics();

    /// @brief 获取组件类型名称（辅助函数）
    template<typename T>
    static std::string getTypeName();
};

END_NAMESPACE_ECS
