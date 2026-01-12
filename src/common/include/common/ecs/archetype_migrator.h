//
// Created by boil on 2026/1/16.
//

#ifndef RENDU_ECS_ARCHETYPE_MIGRATOR_H
#define RENDU_ECS_ARCHETYPE_MIGRATOR_H

#include "common/ecs/archetype.h"
#include "common/ecs/registry_optimized.h"
#include <typeindex>
#include <unordered_map>
#include <functional>

BEGIN_NAMESPACE_ECS

    // ============================================================================
    // ArchetypeMigrator - 实体原型迁移器
    // ============================================================================

    /**
     * @brief Archetype 迁移器
     *
     * 支持实体在添加或删除组件时自动迁移到正确的 Archetype。
     * 功能包括：
     * 1. 组件提取：从源 Archetype 提取共有组件
     * 2. 数据重建：在目标 Archetype 中重建实体
     * 3. 映射更新：更新实体的 Archetype 映射
     */
    class RC_COMMON_API ArchetypeMigrator
    {
    public:
        using MigrateFunc = std::function<void(uint32, ArchetypeBase*, ArchetypeBase*)>;

        ArchetypeMigrator() = default;
        ~ArchetypeMigrator() = default;

        /**
         * @brief 注册迁移函数
         * 用于特定 Archetype 类型组合之间的迁移
         */
        void registerMigrateFunc(const std::type_index& fromType,
                                const std::type_index& toType,
                                MigrateFunc func);

        /**
         * @brief 迁移实体到目标 Archetype
         * @return 是否成功迁移
         */
        bool migrate(RegistryOptimized& registry, uint32 entityIndex, ArchetypeBase* targetArchetype);

        /**
         * @brief 添加组件并迁移
         */
        template <typename NewComponent, typename... ExistingComponents, typename... Args>
        static bool addComponentAndMigrate(RegistryOptimized& registry,
                                          uint32 entityIndex,
                                          Args&&... args);

        /**
         * @brief 移除组件并迁移
         */
        template <typename ComponentToRemove, typename... RemainingComponents>
        static bool removeComponentAndMigrate(RegistryOptimized& registry,
                                              uint32 entityIndex);

    private:
        // 迁移函数映射
        std::unordered_map<std::type_index,
                          std::unordered_map<std::type_index, MigrateFunc>> m_migrateFuncs;
    };

END_NAMESPACE_ECS

#endif //RENDU_ECS_ARCHETYPE_MIGRATOR_H
