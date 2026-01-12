//
// Created by boil on 2026/1/15.
// 实体关系系统
//

#ifndef RENDU_ECS_RELATIONSHIPS_H
#define RENDU_ECS_RELATIONSHIPS_H

#include "common/ecs/entity.h"
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <functional>

BEGIN_NAMESPACE_ECS

    // ============================================================================
    // 关系组件
    // ============================================================================

    /**
     * @brief 父子关系 - 用于构建层级结构
     */
    struct RC_COMMON_API Parent
    {
        Entity parent;

        Parent() = default;
        Parent(Entity e) : parent(e) {}
    };

    /**
     * @brief 子实体列表 - 用于遍历子节点
     */
    struct RC_COMMON_API Children
    {
        std::vector<Entity> children;

        Children() = default;
        void addChild(Entity e) { children.push_back(e); }
        void removeChild(Entity e)
        {
            children.erase(
                std::remove(children.begin(), children.end(), e),
                children.end()
            );
        }
        [[nodiscard]] bool hasChild(Entity e) const
        {
            return std::find(children.begin(), children.end(), e) != children.end();
        }
    };

    /**
     * @brief 关系管理器 - 管理实体间的层级关系
     */
    class RC_COMMON_API RelationshipManager
    {
    public:
        /**
         * @brief 建立父子关系
         */
        void setParent(Entity child, Entity parent);

        /**
         * @brief 移除父子关系
         */
        void removeParent(Entity child);

        /**
         * @brief 获取父实体
         */
        [[nodiscard]] Entity getParent(Entity child) const;

        /**
         * @brief 获取所有子实体
         */
        [[nodiscard]] const std::vector<Entity>& getChildren(Entity parent) const;

        /**
         * @brief 遍历所有子实体
         */
        template <typename Func>
        void forEachChild(Entity parent, Func&& func) const
        {
            const auto& children = getChildren(parent);
            for (auto child : children)
            {
                func(child);
            }
        }

        /**
         * @brief 遍历所有后代（递归）
         */
        template <typename Func>
        void forEachDescendant(Entity parent, Func&& func) const
        {
            std::function<void(Entity)> traverse = [&](Entity entity) {
                const auto& children = getChildren(entity);
                for (auto child : children)
                {
                    func(child);
                    traverse(child);
                }
            };

            traverse(parent);
        }

        /**
         * @brief 检查是否是后代
         */
        [[nodiscard]] bool isDescendant(Entity parent, Entity child) const;

        /**
         * @brief 清空所有关系
         */
        void clear();

        /**
         * @brief 获取所有参与关系的实体索引
         */
        [[nodiscard]] std::vector<uint32> getAllEntityIndices() const;

    private:
        // 子实体索引 -> 父实体映射
        std::unordered_map<uint32, Entity> m_childToParent;

        // 父实体 -> 子实体列表映射
        std::unordered_map<uint32, std::vector<Entity>> m_parentToChildren;
    };

END_NAMESPACE_ECS

#endif //RENDU_ECS_RELATIONSHIPS_H
