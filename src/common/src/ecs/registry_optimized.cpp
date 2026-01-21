//
// Created by boil on 2026/1/15.
//

#include "common/ecs/registry_optimized.h"

BEGIN_NAMESPACE_ECS

    // ============================================================================
    // RegistryOptimized 实现
    // ============================================================================

    RegistryOptimized::RegistryOptimized()
    {
        // 预留一些空间
        m_impl.entityVersions.reserve(1024);
        m_impl.entityArchetypes.reserve(1024);
    }

    RegistryOptimized::~RegistryOptimized() = default;

    Entity RegistryOptimized::create()
    {
        uint32 entityId;

        // 优先重用空闲实体 ID
        if (!m_impl.freeEntityIds.empty())
        {
            entityId = m_impl.freeEntityIds.back();
            m_impl.freeEntityIds.pop_back();

            // 确保 Archetype 指针为空
            if (entityId < m_impl.entityArchetypes.size())
            {
                m_impl.entityArchetypes[entityId] = nullptr;
            }
        }
        else
        {
            entityId = m_impl.nextEntityId++;
        }

        // 确保版本数组足够大
        if (entityId >= m_impl.entityVersions.size())
        {
            m_impl.entityVersions.resize(entityId + 1, 0);
        }

        // 确保 Archetype 数组足够大
        if (entityId >= m_impl.entityArchetypes.size())
        {
            m_impl.entityArchetypes.resize(entityId + 1, nullptr);
        }

        uint32 version = m_impl.entityVersions[entityId];
        return Entity(entityId, version);
    }

    void RegistryOptimized::destroy(Entity entity)
    {
        uint32 index = entity.index();

        // 检查版本是否匹配
        uint32 currentVersion = getEntityVersion(index);
        if (entity.version() != currentVersion) return;

        // 从 Archetype 中移除实体
        auto* archetype = getEntityArchetype(index);
        if (archetype)
        {
            archetype->remove(index);
            setEntityArchetype(index, nullptr);
        }

        // 清理动态组件映射
        m_impl.entityComponents.erase(index);

        // 清理实体组件类型映射
        m_impl.entityComponentTypes.erase(index);

        // 增加版本号（使旧的 Entity 引用失效）
        if (index < m_impl.entityVersions.size())
        {
            m_impl.entityVersions[index]++;
        }

        // 将实体 ID 加入空闲队列（便于重用）
        m_impl.freeEntityIds.push_back(index);
    }

    bool RegistryOptimized::valid(Entity entity) const
    {
        uint32 index = entity.index();

        // index 为 0 表示无效实体
        if (index == 0) return false;

        if (index >= m_impl.entityVersions.size()) return false;

        // 检查版本是否匹配
        uint32 currentVersion = m_impl.entityVersions[index];
        return entity.version() == currentVersion;
    }

    size_t RegistryOptimized::size() const
    {
        // 统计所有 Archetype 中的实体数量
        size_t total = 0;
        for (const auto& pair : m_impl.archetypes)
        {
            total += pair.second->size();
        }
        return total;
    }

    size_t RegistryOptimized::archetypeCount() const
    {
        return m_impl.archetypes.size();
    }

    void RegistryOptimized::clear()
    {
        m_impl.archetypes.clear();
        m_impl.entityArchetypes.clear();
        m_impl.entityVersions.clear();
        m_impl.nextEntityId = 1;
        m_impl.freeEntityIds.clear();
        m_impl.entityComponents.clear();
        m_impl.groups.clear();
    }

    void RegistryOptimized::moveEntity(Entity entity, ArchetypeBase* from, ArchetypeBase* to)
    {
        // 注意：这是一个简化实现
        // 完整实现应该：
        // 1. 从 from 中提取所有组件
        // 2. 将组件添加到 to
        // 3. 更新实体的 Archetype 指针

        // 由于 Archetype 是模板化的，类型擦除比较复杂
        // 这里提供基本框架，实际使用时需要更复杂的实现
        from->remove(entity.index());
        setEntityArchetype(entity.index(), to);
    }

END_NAMESPACE_ECS
