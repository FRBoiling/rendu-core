//
// Created by boil on 2026/1/15.
// 线程安全的 ECS 注册表
//

#ifndef RENDU_ECS_THREAD_SAFE_REGISTRY_H
#define RENDU_ECS_THREAD_SAFE_REGISTRY_H

#include "common/define.h"
#include "common/ecs/registry_optimized.h"
#include <shared_mutex>
#include <mutex>
#include <future>
#include <vector>

BEGIN_NAMESPACE_ECS

    // ============================================================================
    // 读写锁策略
    // ============================================================================

    /**
     * @brief 读写锁包装器
     *
     * 使用共享互斥锁实现读写锁：
     * - 读操作：共享锁（多个读可以同时进行）
     * - 写操作：独占锁（独占访问）
     */
    class RC_COMMON_API ReadWriteLock
    {
    public:
        ReadWriteLock();

        // 获取读锁（共享）
        void lockRead();
        void unlockRead();

        // 获取写锁（独占）
        void lockWrite();
        void unlockWrite();

    private:
        std::shared_mutex m_mutex;
    };

    /**
     * @brief 读锁 RAII 包装器
     */
    class RC_COMMON_API ReadLockGuard
    {
    public:
        explicit ReadLockGuard(ReadWriteLock& lock);
        ~ReadLockGuard();

    private:
        ReadWriteLock& m_lock;
    };

    /**
     * @brief 写锁 RAII 包装器
     */
    class RC_COMMON_API WriteLockGuard
    {
    public:
        explicit WriteLockGuard(ReadWriteLock& lock);
        ~WriteLockGuard();

    private:
        WriteLockGuard(const WriteLockGuard&) = delete;
        WriteLockGuard& operator=(const WriteLockGuard&) = delete;

        ReadWriteLock& m_lock;
    };

    // ============================================================================
    // 线程安全注册表
    // ============================================================================

    /**
     * @brief 线程安全的 ECS 注册表
     *
     * 所有操作都是线程安全的：
     * - 查询操作使用读锁（多个线程可以同时查询）
     * - 修改操作使用写锁（独占访问）
     */
    class RC_COMMON_API RegistryThreadSafe
    {
    public:
        RegistryThreadSafe();
        ~RegistryThreadSafe();

        // ========================================================================
        // 实体管理
        // ========================================================================

        /**
         * @brief 创建新实体（线程安全）
         */
        [[nodiscard]] Entity create();

        /**
         * @brief 销毁实体（线程安全）
         */
        void destroy(Entity entity);

        /**
         * @brief 检查实体是否有效（线程安全）
         */
        [[nodiscard]] bool valid(Entity entity) const;

        // ========================================================================
        // 组件管理
        // ========================================================================

        /**
         * @brief 为实体添加组件（线程安全）
         */
        template <typename... ComponentTypes, typename... Args>
        void emplace(Entity entity, Args&&... args)
        {
            WriteLockGuard lock(m_lock);
            m_registry.emplace<ComponentTypes...>(entity, std::forward<Args>(args)...);
        }

        /**
         * @brief 获取实体的组件（线程安全）
         */
        template <typename... ComponentTypes>
        [[nodiscard]] std::tuple<ComponentTypes*...> tryGet(Entity entity)
        {
            ReadLockGuard lock(m_lock);
            return m_registry.tryGet<ComponentTypes...>(entity);
        }

        /**
         * @brief 获取实体的组件（引用版本，线程安全）
         */
        template <typename... ComponentTypes>
        [[nodiscard]] std::tuple<ComponentTypes&...> get(Entity entity)
        {
            ReadLockGuard lock(m_lock);
            return m_registry.get<ComponentTypes...>(entity);
        }

        /**
         * @brief 检查实体是否有组件（线程安全）
         */
        template <typename... ComponentTypes>
        [[nodiscard]] bool has(Entity entity) const
        {
            ReadLockGuard lock(m_lock);
            return m_registry.has<ComponentTypes...>(entity);
        }

        // ========================================================================
        // 查询
        // ========================================================================

        /**
         * @brief 获取视图（线程安全）
         */
        template <typename... ComponentTypes>
        [[nodiscard]] auto view()
        {
            ReadLockGuard lock(m_lock);
            return m_registry.view<ComponentTypes...>();
        }

        /**
         * @brief 获取排除视图（线程安全）
         */
        template <typename... ComponentTypes>
        [[nodiscard]] auto viewExclude()
        {
            ReadLockGuard lock(m_lock);
            return m_registry.viewExclude<ComponentTypes...>();
        }

        // ========================================================================
        // 批量操作
        // ========================================================================

        /**
         * @brief 批量创建实体（线程安全）
         */
        template <typename... ComponentTypes>
        [[nodiscard]] std::vector<Entity> createBatch(size_t count)
        {
            WriteLockGuard lock(m_lock);
            return m_registry.createBatch<ComponentTypes...>(count);
        }

        // ========================================================================
        // 并行操作
        // ========================================================================

        /**
         * @brief 并行遍历实体
         */
        template <typename Func>
        void parallelEach(size_t threadCount, Func&& func)
        {
            ReadLockGuard lock(m_lock);
            auto view = m_registry.view<Entity>(); // 简化示例

            std::vector<std::future<void>> futures;
            size_t batchSize = view.size() / threadCount;

            for (size_t t = 0; t < threadCount; ++t)
            {
                size_t start = t * batchSize;
                size_t end = (t == threadCount - 1) ? view.size() : start + batchSize;

                futures.emplace_back(std::async(std::launch::async, [&]() {
                    size_t idx = 0;
                    for (auto it = view.begin(); it != view.end(); ++it, ++idx)
                    {
                        if (idx >= start && idx < end)
                        {
                            func(*it);
                        }
                    }
                }));
            }

            for (auto& future : futures)
            {
                future.wait();
            }
        }

        // ========================================================================
        // 状态查询
        // ========================================================================

        /**
         * @brief 获取实体数量（线程安全）
         */
        [[nodiscard]] size_t size() const
        {
            ReadLockGuard lock(m_lock);
            return m_registry.size();
        }

        /**
         * @brief 获取 Archetype 数量（线程安全）
         */
        [[nodiscard]] size_t archetypeCount() const
        {
            ReadLockGuard lock(m_lock);
            // 简化实现：返回 0
            // 需要在 RegistryOptimized 中添加 archetypeCount() 方法
            return 0;
        }

    private:
        mutable ReadWriteLock m_lock;
        RegistryOptimized m_registry;
    };

END_NAMESPACE_ECS

#endif // RENDU_ECS_THREAD_SAFE_REGISTRY_H
