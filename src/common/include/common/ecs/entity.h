//
// Created by boil on 2026/1/13.
//

#ifndef RENDU_ECS_ENTITY_H
#define RENDU_ECS_ENTITY_H

#include "common/define.h"
#include <cstdint>
#include <functional>

BEGIN_NAMESPACE_ECS

// 前向声明
class RegistryOptimized;

template <typename...>
class Archetype;
    // ============================================================================
    // Entity - 实体类型封装
    // ============================================================================

    /**
     * @brief 实体标识符
     *
     * 使用 64 位整数存储：
     * - 低 32 位：实体索引（entity index）
     * - 高 32 位：实体版本（version）
     *
     * 版本机制用于检测悬空引用，当实体被销毁并重用时会增加版本号。
     */
    class RC_COMMON_API Entity
    {
    public:
        /**
         * @brief 默认构造函数 - 创建无效实体
         */
        Entity() noexcept : m_value(0) {}

        /**
         * @brief 从整数构造实体（兼容旧代码）
         * @param value 实体值（只有低32位有效）
         */
        explicit Entity(uint32 value) noexcept : m_value(static_cast<uint64>(value)) {}

        /**
         * @brief 拷贝构造函数
         */
        Entity(const Entity& other) noexcept = default;

        /**
         * @brief 移动构造函数
         */
        Entity(Entity&& other) noexcept = default;

        /**
         * @brief 拷贝赋值运算符
         */
        Entity& operator=(const Entity& other) noexcept = default;

        /**
         * @brief 移动赋值运算符
         */
        Entity& operator=(Entity&& other) noexcept = default;

        /**
         * @brief 析构函数
         */
        ~Entity() = default;

        /**
         * @brief 获取实体索引（低32位）
         * @return 实体的索引值
         */
        [[nodiscard]] uint32 index() const noexcept
        {
            return static_cast<uint32>(m_value & 0xFFFFFFFFULL);
        }

        /**
         * @brief 获取实体版本（高32位）
         * @return 实体的版本号
         */
        [[nodiscard]] uint32 version() const noexcept
        {
            return static_cast<uint32>(m_value >> 32);
        }

        /**
         * @brief 获取完整实体值
         * @return 实体的完整64位值
         */
        [[nodiscard]] uint64 value() const noexcept
        {
            return m_value;
        }

        /**
         * @brief 检查实体是否有效
         * @return true 如果实体有效,否则 false
         */
        [[nodiscard]] bool valid() const noexcept
        {
            return m_value != 0;
        }

        /**
         * @brief 相等比较
         */
        [[nodiscard]] bool operator==(const Entity& other) const noexcept
        {
            return m_value == other.m_value;
        }

        /**
         * @brief 不等比较
         */
        [[nodiscard]] bool operator!=(const Entity& other) const noexcept
        {
            return m_value != other.m_value;
        }

        /**
         * @brief 小于比较
         */
        [[nodiscard]] bool operator<(const Entity& other) const noexcept
        {
            return m_value < other.m_value;
        }

        /**
         * @brief 获取无效实体
         */
        static Entity invalid() noexcept
        {
            return Entity{};
        }

    private:
        /**
         * @brief 私有构造函数，用于创建带版本的实体
         */
        Entity(uint32 index, uint32 version) noexcept
            : m_value(static_cast<uint64>(version) << 32 | index) {}

        uint64 m_value;

        friend class RegistryBase;
        friend class RegistryOptimized;

        // 为 Archetype 提供友元访问
        template <typename...>
        friend class Archetype;
    };

    /**
     * @brief 无效实体的哈希函数
     */
    struct EntityHash
    {
        std::size_t operator()(const Entity& entity) const noexcept
        {
            return std::hash<uint64>{}(entity.value());
        }
    };

END_NAMESPACE_ECS

#endif //RENDU_ECS_ENTITY_H
