//
// Created by boil on 2026/1/15.
//

#include "common/ecs/relationships.h"
#include <unordered_set>

BEGIN_NAMESPACE_ECS

    // ============================================================================
    // RelationshipManager 实现
    // ============================================================================

    void RelationshipManager::setParent(Entity child, Entity parent)
    {
        uint32 childId = child.index();
        uint32 parentId = parent.index();

        // 如果子实体已有父节点，先移除旧关系
        if (auto it = m_childToParent.find(childId); it != m_childToParent.end())
        {
            Entity oldParent = it->second;
            auto& siblings = m_parentToChildren[oldParent.index()];
            siblings.erase(
                std::remove(siblings.begin(), siblings.end(), child),
                siblings.end()
            );
        }

        // 建立新关系
        m_childToParent[childId] = parent;
        m_parentToChildren[parentId].push_back(child);
    }

    void RelationshipManager::removeParent(Entity child)
    {
        uint32 childId = child.index();

        auto it = m_childToParent.find(childId);
        if (it == m_childToParent.end())
        {
            return; // 没有父节点
        }

        Entity parent = it->second;

        // 从父实体的子列表中移除
        auto& siblings = m_parentToChildren[parent.index()];
        siblings.erase(
            std::remove(siblings.begin(), siblings.end(), child),
            siblings.end()
        );

        // 移除映射
        m_childToParent.erase(it);
    }

    Entity RelationshipManager::getParent(Entity child) const
    {
        uint32 childId = child.index();
        auto it = m_childToParent.find(childId);
        return (it != m_childToParent.end()) ? it->second : Entity();
    }

    const std::vector<Entity>& RelationshipManager::getChildren(Entity parent) const
    {
        uint32 parentId = parent.index();
        static const std::vector<Entity> empty;
        auto it = m_parentToChildren.find(parentId);
        return (it != m_parentToChildren.end()) ? it->second : empty;
    }

    bool RelationshipManager::isDescendant(Entity parent, Entity child) const
    {
        bool found = false;

        std::function<void(Entity)> traverse = [&](Entity entity) {
            if (found) return;

            const auto& children = getChildren(entity);
            for (auto c : children)
            {
                if (c == child)
                {
                    found = true;
                    return;
                }
                traverse(c);
                if (found) return;
            }
        };

        traverse(parent);
        return found;
    }

    void RelationshipManager::clear()
    {
        m_childToParent.clear();
        m_parentToChildren.clear();
    }

    std::vector<uint32> RelationshipManager::getAllEntityIndices() const
    {
        std::vector<uint32> indices;
        std::unordered_set<uint32> entitySet;

        // 收集所有作为子节点的实体索引
        for (const auto& pair : m_childToParent) {
            entitySet.insert(pair.first);
        }

        // 收集所有作为父节点的实体索引
        for (const auto& pair : m_parentToChildren) {
            entitySet.insert(pair.first);
        }

        // 转换为向量
        for (uint32 index : entitySet) {
            indices.push_back(index);
        }

        return indices;
    }

END_NAMESPACE_ECS
