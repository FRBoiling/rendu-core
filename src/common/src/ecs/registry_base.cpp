//
// Created by boil on 2026/1/13.
//

#include "common/ecs/registry_base.h"

BEGIN_NAMESPACE_ECS

    // ============================================================================
    // RegistryBase 实现
    // ============================================================================

    RegistryBase::RegistryBase() = default;

    RegistryBase::~RegistryBase() = default;

    Entity RegistryBase::create()
    {
        uint32 entityId;
        uint32 version;

        if (!m_impl.activeEntities.empty() && !m_impl.destroyedIndices.empty())
        {
            // 重用已销毁的实体索引
            entityId = m_impl.destroyedIndices.back();
            m_impl.destroyedIndices.pop_back();

            // 增加版本号
            if (entityId >= m_impl.entityVersions.size())
            {
                m_impl.entityVersions.resize(entityId + 1, 0);
            }
            version = ++m_impl.entityVersions[entityId];
        }
        else
        {
            // 创建新实体
            entityId = m_impl.nextEntityId++;

            // 确保版本数组足够大
            if (entityId >= m_impl.entityVersions.size())
            {
                m_impl.entityVersions.resize(entityId + 1, 0);
            }
            version = m_impl.entityVersions[entityId];
        }

        m_impl.activeEntities.insert(entityId);

        return Entity(entityId, version);
    }

    std::vector<Entity> RegistryBase::createMany(size_t count)
    {
        std::vector<Entity> result;
        result.reserve(count);
        for (size_t i = 0; i < count; ++i)
        {
            result.push_back(create());
        }
        return result;
    }

    void RegistryBase::destroy(Entity entity)
    {
        uint32 index = entity.index();

        if (!m_impl.activeEntities.contains(index)) return;

        // 检查版本是否匹配
        uint32 currentVersion = getEntityVersion(index);
        if (entity.version() != currentVersion) return;

        // 从活跃实体集中移除
        m_impl.activeEntities.erase(index);

        // 记录被销毁的索引以便重用
        m_impl.destroyedIndices.push_back(index);

        // 移除该实体的所有组件
        for (auto& pair : m_impl.componentPools)
        {
            pair.second->remove(index);
        }
    }

    void RegistryBase::destroyMany(const std::vector<Entity>& entities)
    {
        for (const auto& entity : entities)
        {
            destroy(entity);
        }
    }

    bool RegistryBase::valid(Entity entity) const
    {
        uint32 index = entity.index();

        if (!m_impl.activeEntities.contains(index)) return false;

        // 检查版本是否匹配
        uint32 currentVersion = getEntityVersion(index);
        return entity.version() == currentVersion;
    }

    size_t RegistryBase::size() const
    {
        return m_impl.activeEntities.size();
    }

    size_t RegistryBase::capacity() const
    {
        return m_impl.activeEntities.size();
    }

    void RegistryBase::clear()
    {
        m_impl.activeEntities.clear();
        m_impl.destroyedIndices.clear();
        m_impl.componentPools.clear();
        m_impl.entityVersions.clear();
        m_impl.nextEntityId = 1;
    }

    void RegistryBase::reserve(size_t capacity)
    {
        // SparseSet 不需要显式预留
    }

END_NAMESPACE_ECS

