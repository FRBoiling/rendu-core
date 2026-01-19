//
// Created by boil on 2026/1/13.
//

#ifndef RENDU_ECS_REGISTRY_BASE_H
#define RENDU_ECS_REGISTRY_BASE_H

#include "common/define.h"
#include "entity.h"
#include "common/utils/types.h"

#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>
#include <typeindex>
#include <cstdint>
#include <unordered_set>

BEGIN_NAMESPACE_ECS

    // ============================================================================
    // SparseSet - 稀疏集（缓存友好的集合结构）
    // ============================================================================

    /**
     * @brief 稀疏集实现
     *
     * 提供O(1)的查找、插入和删除操作，同时保持良好的缓存局部性。
     * 内部使用两个数组：sparse（稀疏）和dense（密集）。
     * - sparse[index] 存储元素在 dense 数组中的位置
     * - dense[pos] 存储实际的元素索引
     *
     * 这种结构使得遍历 dense 数组可以高效访问所有元素。
     */
    class SparseSet
    {
    public:
        SparseSet() = default;
        ~SparseSet() = default;

        SparseSet(const SparseSet&) = delete;
        SparseSet(SparseSet&&) noexcept = default;
        SparseSet& operator=(const SparseSet&) = delete;
        SparseSet& operator=(SparseSet&&) noexcept = default;

        /**
         * @brief 检查是否包含元素
         */
        [[nodiscard]] bool contains(uint32 index) const
        {
            if (index >= m_sparse.size()) return false;
            uint32 pos = m_sparse[index];
            return pos < m_dense.size() && m_dense[pos] == index;
        }

        /**
         * @brief 插入元素
         */
        void insert(uint32 index)
        {
            if (!contains(index))
            {
                if (index >= m_sparse.size())
                {
                    m_sparse.resize(index + 1, ~0u);
                }
                m_sparse[index] = static_cast<uint32>(m_dense.size());
                m_dense.push_back(index);
            }
        }

        /**
         * @brief 移除元素
         */
        bool erase(uint32 index)
        {
            if (!contains(index)) return false;

            uint32 pos = m_sparse[index];
            uint32 last = m_dense.back();

            m_dense[pos] = last;
            m_sparse[last] = pos;
            m_dense.pop_back();
            m_sparse[index] = ~0u;

            return true;
        }

        /**
         * @brief 获取元素数量
         */
        [[nodiscard]] size_t size() const noexcept
        {
            return m_dense.size();
        }

        /**
         * @brief 检查是否为空
         */
        [[nodiscard]] bool empty() const noexcept
        {
            return m_dense.empty();
        }

        /**
         * @brief 清空所有元素
         */
        void clear() noexcept
        {
            m_dense.clear();
            m_sparse.clear();
        }

        /**
         * @brief 获取 dense 数组（用于遍历）
         */
        [[nodiscard]] const std::vector<uint32>& data() const noexcept
        {
            return m_dense;
        }

        /**
         * @brief 获取 dense 数组位置（通过实体索引）
         */
        [[nodiscard]] uint32 position(uint32 index) const
        {
            return m_sparse[index];
        }

    private:
        std::vector<uint32> m_sparse;
        std::vector<uint32> m_dense;
    };

    // ============================================================================
    // Component - 组件基类标记
    // ============================================================================

    /**
     * @brief 组件基类标记
     *
     * 所有组件类型都应该继承自此类,用于类型标记。
     * 这是一个空基类,仅用于编译时类型检查。
     */
    struct Component {};

    // ============================================================================
    // RegistryBase - 注册中心(实体组件系统管理器)
    // ============================================================================

    /**
     * @brief 实体组件系统注册中心
     *
     * 管理 Entity、Component 的核心类。
     * 使用稀疏集和紧凑存储优化性能。
     */
    class RC_COMMON_API RegistryBase
    {
    public:
        /**
         * @brief 默认构造函数
         */
        RegistryBase();

        /**
         * @brief 拷贝构造函数 (已删除)
         */
        RegistryBase(const RegistryBase&) = delete;

        /**
         * @brief 移动构造函数
         */
        RegistryBase(RegistryBase&&) noexcept = default;

        /**
         * @brief 拷贝赋值运算符 (已删除)
         */
        RegistryBase& operator=(const RegistryBase&) = delete;

        /**
         * @brief 移动赋值运算符
         */
        RegistryBase& operator=(RegistryBase&&) noexcept = default;

        /**
         * @brief 析构函数
         */
        ~RegistryBase();

        // ========================================================================
        // 实体管理
        // ========================================================================

        /**
         * @brief 创建新实体
         * @return 新创建的实体
         */
        Entity create();

        /**
         * @brief 创建多个实体
         * @param count 要创建的实体数量
         * @return 实体列表
         */
        std::vector<Entity> createMany(size_t count);

        /**
         * @brief 销毁实体
         * @param entity 要销毁的实体
         */
        void destroy(Entity entity);

        /**
         * @brief 销毁多个实体
         * @param entities 要销毁的实体列表
         */
        void destroyMany(const std::vector<Entity>& entities);

        /**
         * @brief 检查实体是否有效
         * @param entity 要检查的实体
         * @return true 如果实体有效,否则 false
         */
        [[nodiscard]] bool valid(Entity entity) const;

        /**
         * @brief 获取实体数量
         * @return 当前活跃实体数量
         */
        [[nodiscard]] size_t size() const;

        /**
         * @brief 获取实体容量
         * @return 当前实体容量
         */
        [[nodiscard]] size_t capacity() const;

        /**
         * @brief 清空所有实体
         */
        void clear();

        /**
         * @brief 预留实体容量
         * @param capacity 要预留的容量
         */
        void reserve(size_t capacity);

        // ========================================================================
        // 组件管理 - 模板接口
        // ========================================================================

        /**
         * @brief 为实体添加组件
         * @tparam ComponentType 组件类型
         * @tparam Args 构造参数类型
         * @param entity 目标实体
         * @param args 组件构造参数
         * @return 组件的引用
         */
        template <typename ComponentType, typename... Args>
        ComponentType& emplace(Entity entity, Args&&... args);

        /**
         * @brief 移除实体的组件
         * @tparam ComponentType 组件类型
         * @param entity 目标实体
         */
        template <typename ComponentType>
        void remove(Entity entity);

        /**
         * @brief 获取实体的组件
         * @tparam ComponentType 组件类型
         * @param entity 目标实体
         * @return 组件的指针,如果不存在则返回 nullptr
         */
        template <typename ComponentType>
        [[nodiscard]] ComponentType* tryGet(Entity entity);

        /**
         * @brief 获取实体的组件 (const 版本)
         * @tparam ComponentType 组件类型
         * @param entity 目标实体
         * @return 组件的指针,如果不存在则返回 nullptr
         */
        template <typename ComponentType>
        [[nodiscard]] const ComponentType* tryGet(Entity entity) const;

        /**
         * @brief 检查实体是否拥有组件
         * @tparam ComponentType 组件类型
         * @param entity 目标实体
         * @return true 如果实体拥有该组件,否则 false
         */
        template <typename ComponentType>
        [[nodiscard]] bool has(Entity entity) const;

        /**
         * @brief 获取或添加组件
         * @tparam ComponentType 组件类型
         * @param entity 目标实体
         * @return 组件的引用
         */
        template <typename ComponentType>
        ComponentType& getOrEmplace(Entity entity);

        // ========================================================================
        // 视图和迭代
        // ========================================================================

        /**
         * @brief 遍历所有实体
         * @tparam Func 回调函数类型
         * @param func 回调函数
         */
        template <typename Func>
        void each(Func&& func);

        /**
         * @brief 遍历所有实体 (const 版本)
         * @tparam Func 回调函数类型
         * @param func 回调函数
         */
        template <typename Func>
        void each(Func&& func) const;

        /**
         * @brief 视图 - 查询特定组件
         * @tparam ComponentTypes 组件类型列表
         */
        template <typename... ComponentTypes>
        class View;

        /**
         * @brief 创建视图
         * @tparam ComponentTypes 组件类型
         * @return 视图对象
         */
        template <typename... ComponentTypes>
        View<ComponentTypes...> view();

        /**
         * @brief 创建常量视图
         * @tparam ComponentTypes 组件类型
         * @return 视图对象
         */
        template <typename... ComponentTypes>
        View<const ComponentTypes...> view() const;

    private:
        struct ComponentPoolBase
        {
            virtual ~ComponentPoolBase() = default;

            // 稀疏集跟踪哪些实体拥有此组件
            SparseSet entitySet;

            // 移除实体的组件（纯虚函数）
            virtual void remove(uint32 index) = 0;

            // 清空所有组件
            virtual void clear() = 0;

            // 获取大小
            virtual size_t size() const = 0;

            // 获取实体的位置
            virtual uint32 position(uint32 index) const = 0;

            // 获取实体数据
            [[nodiscard]] const std::vector<uint32>& data() const
            {
                return entitySet.data();
            }
        };

        template <typename ComponentType>
        struct ComponentPool : public ComponentPoolBase
        {
            // 紧凑的组件数组，与 entitySet.data() 对应
            std::vector<ComponentType> components;

            void remove(uint32 index) override
            {
                if (!entitySet.contains(index)) return;

                uint32 pos = entitySet.position(index);
                uint32 lastIndex = entitySet.data().back();

                // 将最后一个组件移到被删除的位置
                components[pos] = std::move(components.back());
                components.pop_back();

                // 更新稀疏集中的位置
                entitySet.erase(index);
            }

            void clear() override
            {
                components.clear();
                entitySet.clear();
            }

            size_t size() const override
            {
                return components.size();
            }

            /**
             * @brief 获取实体对应的组件
             */
            [[nodiscard]] ComponentType* get(uint32 index)
            {
                if (!entitySet.contains(index)) return nullptr;
                return &components[entitySet.position(index)];
            }

            /**
             * @brief 获取实体对应的组件（const 版本）
             */
            [[nodiscard]] const ComponentType* get(uint32 index) const
            {
                if (!entitySet.contains(index)) return nullptr;
                return &components[entitySet.position(index)];
            }

            /**
             * @brief 获取实体位置
             */
            uint32 position(uint32 index) const override
            {
                return entitySet.position(index);
            }

            /**
             * @brief 添加或获取组件
             */
            template <typename... Args>
            ComponentType& emplace(uint32 index, Args&&... args)
            {
                if (entitySet.contains(index))
                {
                    return components[entitySet.position(index)];
                }

                entitySet.insert(index);
                components.emplace_back(ComponentType(std::forward<Args>(args)...));
                return components.back();
            }
        };

        struct Impl
        {
            uint32 nextEntityId = 1;
            SparseSet activeEntities;

            // 已销毁实体的索引（用于重用）
            std::vector<uint32> destroyedIndices;

            // 实体版本映射（index -> version）
            std::vector<uint32> entityVersions;

            // 组件存储 - 按类型索引
            std::unordered_map<std::type_index, std::unique_ptr<ComponentPoolBase>> componentPools;
        };

        /**
         * @brief 获取实体版本
         */
        [[nodiscard]] uint32 getEntityVersion(uint32 index) const;

        /**
         * @brief 获取组件池
         */
        template <typename ComponentType>
        [[nodiscard]] ComponentPool<ComponentType>* getComponentPool();

        /**
         * @brief 获取组件池 (const 版本)
         */
        template <typename ComponentType>
        [[nodiscard]] const ComponentPool<ComponentType>* getComponentPool() const;

        /**
         * @brief 通过索引获取组件
         */
        template <typename ComponentType>
        [[nodiscard]] ComponentType* tryGet(uint32 index);

        /**
         * @brief 通过索引获取组件 (const 版本)
         */
        template <typename ComponentType>
        [[nodiscard]] const ComponentType* tryGet(uint32 index) const;

        /**
         * @brief 检查索引是否拥有组件
         */
        template <typename ComponentType>
        [[nodiscard]] bool has(uint32 index) const;

        Impl m_impl;
    };

    // ============================================================================
    // View - 视图模板类
    // ============================================================================

    template <typename... ComponentTypes>
    class RegistryBase::View
    {
        public:
            View() = default;
            ~View() = default;

            /**
             * @brief 检查视图是否为空
             */
            [[nodiscard]] bool empty() const
            {
                return size() == 0;
            }

            /**
             * @brief 获取视图大小
             */
            [[nodiscard]] size_t size() const
            {
                if (!m_registry) return 0;

                const auto& smallestSet = getSmallestComponentSet();
                if (!smallestSet) return 0;

                size_t count = 0;
                for (uint32 index : smallestSet->data())
                {
                    if (hasAllComponents(index))
                    {
                        ++count;
                    }
                }
                return count;
            }

            /**
             * @brief 遍历视图中的实体
             * @tparam Func 回调函数类型，签名: void(Entity, ComponentTypes&...)
             * @param func 回调函数
             */
            template <typename Func>
            void each(Func&& func) const
            {
                if (!m_registry) return;

                const auto& smallestSet = getSmallestComponentSet();
                if (!smallestSet) return;

                for (uint32 index : smallestSet->data())
                {
                    if (hasAllComponents(index))
                    {
                        invokeEach(Entity(index, m_registry->getEntityVersion(index)),
                                   std::forward<Func>(func),
                                   std::index_sequence_for<ComponentTypes...>{});
                    }
                }
            }

        private:
            friend class RegistryBase;

            View(RegistryBase* registry) : m_registry(registry) {}

            /**
             * @brief 获取包含实体最少的组件集合（优化遍历起点）
             */
            const ComponentPoolBase* getSmallestComponentSet() const
            {
                const ComponentPoolBase* smallest = nullptr;
                size_t minSize = SIZE_MAX;

                (getComponentPoolSize<ComponentTypes>(smallest, minSize), ...);

                return smallest;
            }

            template <typename ComponentType>
            void getComponentPoolSize(const ComponentPoolBase*& smallest, size_t& minSize) const
            {
                auto pool = m_registry->getComponentPool<ComponentType>();
                if (pool && pool->size() < minSize)
                {
                    minSize = pool->size();
                    smallest = pool;
                }
            }

            /**
             * @brief 检查实体是否拥有所有组件
             */
            bool hasAllComponents(uint32 index) const
            {
                return (m_registry->has<ComponentTypes>(index) && ...);
            }

            template <typename ComponentType>
            bool has(uint32 index) const
            {
                return m_registry->has<ComponentType>(index);
            }

            template <typename Func, std::size_t... Is>
            void invokeEach(Entity e, Func&& func, std::index_sequence<Is...>) const
            {
                func(e, m_registry->tryGet<ComponentTypes>(e)...);
            }

            RegistryBase* m_registry = nullptr;
        };

    // ============================================================================
    // 模板方法的内联实现
    // ============================================================================

    template <typename ComponentType, typename... Args>
    ComponentType& RegistryBase::emplace(Entity entity, Args&&... args)
    {
        std::type_index index(typeid(ComponentType));

        auto it = m_impl.componentPools.find(index);
        if (it == m_impl.componentPools.end())
        {
            auto pool = std::make_unique<ComponentPool<ComponentType>>();
            it = m_impl.componentPools.emplace(index, std::move(pool)).first;
        }

        auto* pool = static_cast<ComponentPool<ComponentType>*>(it->second.get());
        return pool->emplace(entity.index(), std::forward<Args>(args)...);
    }

    template <typename ComponentType>
    void RegistryBase::remove(Entity entity)
    {
        std::type_index index(typeid(ComponentType));

        auto it = m_impl.componentPools.find(index);
        if (it != m_impl.componentPools.end())
        {
            auto* pool = static_cast<ComponentPool<ComponentType>*>(it->second.get());
            pool->remove(entity.index());
        }
    }

    template <typename ComponentType>
    ComponentType* RegistryBase::tryGet(Entity entity)
    {
        return tryGet<ComponentType>(entity.index());
    }

    template <typename ComponentType>
    const ComponentType* RegistryBase::tryGet(Entity entity) const
    {
        return tryGet<ComponentType>(entity.index());
    }

    template <typename ComponentType>
    ComponentType* RegistryBase::tryGet(uint32 index)
    {
        auto* pool = getComponentPool<ComponentType>();
        return pool ? pool->get(index) : nullptr;
    }

    template <typename ComponentType>
    const ComponentType* RegistryBase::tryGet(uint32 index) const
    {
        auto* pool = getComponentPool<ComponentType>();
        return pool ? pool->get(index) : nullptr;
    }

    template <typename ComponentType>
    bool RegistryBase::has(Entity entity) const
    {
        return tryGet<ComponentType>(entity) != nullptr;
    }

    template <typename ComponentType>
    bool RegistryBase::has(uint32 index) const
    {
        return tryGet<ComponentType>(index) != nullptr;
    }

    template <typename ComponentType>
    ComponentType& RegistryBase::getOrEmplace(Entity entity)
    {
        auto* ptr = tryGet<ComponentType>(entity);
        if (ptr) return *ptr;
        return emplace<ComponentType>(entity);
    }

    template <typename Func>
    void RegistryBase::each(Func&& func)
    {
        for (uint32 index : m_impl.activeEntities.data())
        {
            func(Entity(index, getEntityVersion(index)));
        }
    }

    template <typename Func>
    void RegistryBase::each(Func&& func) const
    {
        for (uint32 index : m_impl.activeEntities.data())
        {
            func(Entity(index, getEntityVersion(index)));
        }
    }

    template <typename... ComponentTypes>
    RegistryBase::View<ComponentTypes...> RegistryBase::view()
    {
        return View<ComponentTypes...>(this);
    }

    template <typename ComponentType>
    auto RegistryBase::getComponentPool() -> ComponentPool<ComponentType>*
    {
        std::type_index index(typeid(ComponentType));
        auto it = m_impl.componentPools.find(index);
        if (it == m_impl.componentPools.end()) return nullptr;
        return static_cast<ComponentPool<ComponentType>*>(it->second.get());
    }

    template <typename ComponentType>
    auto RegistryBase::getComponentPool() const -> const ComponentPool<ComponentType>*
    {
        std::type_index index(typeid(ComponentType));
        auto it = m_impl.componentPools.find(index);
        if (it == m_impl.componentPools.end()) return nullptr;
        return static_cast<const ComponentPool<ComponentType>*>(it->second.get());
    }

    inline uint32 RegistryBase::getEntityVersion(uint32 index) const
    {
        if (index < m_impl.entityVersions.size())
        {
            return m_impl.entityVersions[index];
        }
        return 0;
    }

END_NAMESPACE_ECS

#endif //RENDU_ECS_REGISTRY_BASE_H
