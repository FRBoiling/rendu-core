//
// Created by boil on 2026/1/15.
// Archetype 迁移系统
//

#ifndef RENDU_ECS_ARCHETYPE_MIGRATION_H
#define RENDU_ECS_ARCHETYPE_MIGRATION_H

#include "common/define.h"
#include "archetype.h"
#include "entity.h"
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <tuple>
#include <functional>

BEGIN_NAMESPACE_ECS

    // ============================================================================
    // Archetype 迁移器
    // ============================================================================

    /**
     * @brief std::pair<std::type_index, std::type_index> 的哈希函数
     */
    struct PairHash
    {
        size_t operator()(const std::pair<std::type_index, std::type_index>& key) const noexcept
        {
            size_t h1 = key.first.hash_code();
            size_t h2 = key.second.hash_code();
            return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
        }
    };

    /**
     * @brief Archetype 迁移管理器
     *
     * 负责将实体从一个 Archetype 迁移到另一个 Archetype，
     * 提取共有组件并添加新组件。
     */
    class RC_COMMON_API ArchetypeMigrator
    {
    public:
        using MigrateFunc = std::function<void(
            ArchetypeBase* source,
            ArchetypeBase* target,
            uint32 entityIndex
        )>;

        /**
         * @brief 注册迁移函数
         *
         * @note 当前简化实现：迁移时组件使用默认值
         * 未来可扩展为智能提取共有组件
         */
        template <typename SourceArchetypeType, typename TargetArchetypeType>
        void registerMigrateFunc()
        {
            using SourceTuple = typename SourceArchetypeType::ComponentTuple;
            using TargetTuple = typename TargetArchetypeType::ComponentTuple;

            std::type_index sourceType(typeid(SourceArchetypeType));
            std::type_index targetType(typeid(TargetArchetypeType));

            m_migrateFuncs[{sourceType, targetType}] = [](
                ArchetypeBase* source,
                ArchetypeBase* target,
                uint32 entityIndex)
            {
                // 从源 Archetype 移除实体
                source->remove(entityIndex);

                // 在目标 Archetype 中创建实体（使用默认值）
                target->emplace(entityIndex);
            };
        }

        /**
         * @brief 执行迁移
         */
        void migrate(
            ArchetypeBase* source,
            ArchetypeBase* target,
            uint32 entityIndex)
        {
            if (!source || !target)
            {
                throw std::runtime_error("Invalid archetype pointers");
            }

            std::type_index sourceType(typeid(*source));
            std::type_index targetType(typeid(*target));

            auto key = std::make_pair(sourceType, targetType);
            auto it = m_migrateFuncs.find(key);

            if (it != m_migrateFuncs.end())
            {
                it->second(source, target, entityIndex);
            }
            else
            {
                // 使用通用迁移（只移动 ID，组件使用默认值）
                source->remove(entityIndex);
                target->emplace(entityIndex);
            }
        }

        /**
         * @brief 检查是否支持迁移
         */
        [[nodiscard]] bool canMigrate(
            ArchetypeBase* source,
            ArchetypeBase* target) const
        {
            if (!source || !target) return false;

            std::type_index sourceType(typeid(*source));
            std::type_index targetType(typeid(*target));

            return m_migrateFuncs.count({sourceType, targetType}) > 0;
        }

    private:
        using MigrateKey = std::pair<std::type_index, std::type_index>;
        std::unordered_map<MigrateKey, MigrateFunc, PairHash> m_migrateFuncs;
    };

END_NAMESPACE_ECS

#endif // RENDU_ECS_ARCHETYPE_MIGRATION_H
