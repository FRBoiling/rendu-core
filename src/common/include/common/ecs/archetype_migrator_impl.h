//
// Created by boil on 2026/1/16.
//

#ifndef RENDU_ECS_ARCHETYPE_MIGRATOR_IMPL_H
#define RENDU_ECS_ARCHETYPE_MIGRATOR_IMPL_H

#include "common/ecs/archetype_migrator.h"

BEGIN_NAMESPACE_ECS

    // ============================================================================
    // ArchetypeMigrator 实现
    // ============================================================================

    void ArchetypeMigrator::registerMigrateFunc(const std::type_index& fromType,
                                               const std::type_index& toType,
                                               MigrateFunc func)
    {
        m_migrateFuncs[fromType][toType] = std::move(func);
    }

    bool ArchetypeMigrator::migrate(RegistryOptimized& registry,
                                   uint32 entityIndex,
                                   ArchetypeBase* targetArchetype)
    {
        auto* sourceArchetype = registry.getEntityArchetype(entityIndex);
        if (!sourceArchetype || !targetArchetype)
        {
            return false;
        }

        // 检查是否已注册迁移函数
        std::type_index sourceType(typeid(*sourceArchetype));
        std::type_index targetType(typeid(*targetArchetype));

        auto fromIt = m_migrateFuncs.find(sourceType);
        if (fromIt != m_migrateFuncs.end())
        {
            auto toIt = fromIt->second.find(targetType);
            if (toIt != fromIt->second.end())
            {
                // 执行已注册的迁移函数
                toIt->second(entityIndex, sourceArchetype, targetArchetype);

                // 更新实体 Archetype 映射
                registry.setEntityArchetype(entityIndex, targetArchetype);

                return true;
            }
        }

        // 通用迁移：尝试使用类型转换
        // 注意：这里需要更复杂的实现来处理不同类型组合
        // 简化处理：返回 false，提示需要注册特定的迁移函数
        return false;
    }

    // ============================================================================
    // 类型安全的 Archetype 迁移辅助函数
    // ============================================================================

    /**
     * @brief 从源 Archetype 提取组件数据到目标 Archetype
     */
    template <typename... TargetComponents, typename... SourceComponents>
    bool migrateArchetypeData(Archetype<SourceComponents...>* source,
                              Archetype<TargetComponents...>* target,
                              uint32 entityIndex)
    {
        // 检查源 Archetype 是否包含该实体
        if (!source->contains(entityIndex))
        {
            return false;
        }

        // 获取源实体位置
        const auto& entityToPosition = source->getEntityToPosition();
        auto it = entityToPosition.find(entityIndex);
        if (it == entityToPosition.end())
        {
            return false;
        }

        // 获取源组件数据
        auto sourceComponents = source->tryGet(entityIndex);

        // 提取共有组件并构造目标组件
        migrateExtractComponents<TargetComponents...>(
            target, entityIndex, sourceComponents,
            std::index_sequence_for<SourceComponents...>{}
        );

        return true;
    }

    /**
     * @brief 递归提取组件（主模板）
     */
    template <typename TargetComponent, typename... RestComponents,
              typename... SourceComponents, size_t... Is>
    void migrateExtractComponents(Archetype<TargetComponent, RestComponents...>* target,
                                  uint32 entityIndex,
                                  const std::tuple<SourceComponents*...>& sourceComponents,
                                  std::index_sequence<Is...>)
    {
        // 尝试从源中提取 TargetComponent
        TargetComponent* srcComp = nullptr;
        extractComponent<TargetComponent, SourceComponents...>(srcComp, sourceComponents);

        if (srcComp)
        {
            // 从源组件复制
            target->emplace(entityIndex, *srcComp);
        }
        else
        {
            // 使用默认构造
            target->emplace(entityIndex);
        }

        // 继续处理剩余组件
        if constexpr (sizeof...(RestComponents) > 0)
        {
            migrateExtractComponents<RestComponents...>(
                target, entityIndex, sourceComponents,
                std::index_sequence_for<SourceComponents...>{}
            );
        }
    }

    /**
     * @brief 从源组件元组中提取特定类型
     */
    template <typename TargetType, typename SourceType, typename... RestTypes>
    void extractComponent(TargetType*& result, const std::tuple<SourceType*, RestTypes*...>& components)
    {
        if constexpr (std::is_same_v<TargetType, SourceType>)
        {
            result = std::get<SourceType*>(components);
        }
        else if constexpr (sizeof...(RestTypes) > 0)
        {
            extractComponent<TargetType, RestTypes...>(result, components);
        }
    }

    // ========================================================================
    // 组件添加迁移实现
    // ========================================================================

    template <typename NewComponent, typename... ExistingComponents, typename... Args>
    bool ArchetypeMigrator::addComponentAndMigrate(RegistryOptimized& registry,
                                                    uint32 entityIndex,
                                                    Args&&... args)
    {
        // 获取当前 Archetype
        auto* currentArchetype = registry.getEntityArchetype(entityIndex);
        if (!currentArchetype)
        {
            return false;
        }

        // 转换为类型化 Archetype
        auto* typedCurrent = dynamic_cast<Archetype<ExistingComponents...>*>(currentArchetype);
        if (!typedCurrent)
        {
            return false;
        }

        // 获取或创建目标 Archetype（包含新组件）
        auto* targetArchetype = registry.getOrCreateArchetype<ExistingComponents..., NewComponent>();

        // 从源 Archetype 提取组件数据
        bool success = migrateArchetypeData<ExistingComponents..., NewComponent>(
            typedCurrent, targetArchetype, entityIndex
        );

        if (!success)
        {
            return false;
        }

        // 从源 Archetype 移除实体
        typedCurrent->remove(entityIndex);

        // 更新实体的 Archetype 映射
        registry.setEntityArchetype(entityIndex, targetArchetype);

        // 更新动态组件映射
        auto& components = registry.m_impl.entityComponents[entityIndex];
        components.push_back(std::make_unique<TypedComponent<NewComponent>>(
            NewComponent(std::forward<Args>(args)...)
        ));

        return true;
    }

    // ========================================================================
    // 组件移除迁移实现
    // ========================================================================

    template <typename ComponentToRemove, typename... RemainingComponents>
    bool ArchetypeMigrator::removeComponentAndMigrate(RegistryOptimized& registry,
                                                       uint32 entityIndex)
    {
        // 获取当前 Archetype
        auto* currentArchetype = registry.getEntityArchetype(entityIndex);
        if (!currentArchetype)
        {
            return false;
        }

        // 获取或创建目标 Archetype（不包含被移除的组件）
        auto* targetArchetype = registry.getOrCreateArchetype<RemainingComponents...>();

        // 从源 Archetype 迁移数据（排除要移除的组件）
        bool success = migrateArchetypeData<RemainingComponents...>(
            currentArchetype, targetArchetype, entityIndex
        );

        if (!success)
        {
            return false;
        }

        // 从源 Archetype 移除实体
        currentArchetype->remove(entityIndex);

        // 更新实体的 Archetype 映射
        registry.setEntityArchetype(entityIndex, targetArchetype);

        // 从动态组件映射中移除
        auto it = registry.m_impl.entityComponents.find(entityIndex);
        if (it != registry.m_impl.entityComponents.end())
        {
            auto& components = it->second;
            components.erase(
                std::remove_if(components.begin(), components.end(),
                    [](const std::unique_ptr<TypeErasedComponent>& comp) {
                        return comp->type() == typeid(ComponentToRemove);
                    }),
                components.end()
            );
        }

        return true;
    }

END_NAMESPACE_ECS

#endif //RENDU_ECS_ARCHETYPE_MIGRATOR_IMPL_H
