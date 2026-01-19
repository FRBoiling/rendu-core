//
// Created by boil on 2026/1/15.
//

#ifndef RENDU_ECS_ARCHETYPE_H
#define RENDU_ECS_ARCHETYPE_H

#include "common/define.h"
#include "entity.h"
#include <vector>
#include <tuple>
#include <sstream>

BEGIN_NAMESPACE_ECS

    // ============================================================================
    // Archetype - 实体原型（高性能 SOA 存储）
    // ============================================================================

    /**
     * @brief 实体原型 - 使用 SOA 布局存储相同组件组合的实体
     *
     * 所有相同组件组合的实体存储在同一个 Archetype 中。
     * 组件数据按列存储（SOA），极大地提高缓存命中率。
     *
     * 内存布局:
     * ┌─────────────────────────────────────────────────────┐
     * │ Chunk (16KB):                                        │
     * │   entityIds: [e1, e2, e3, ..., eN]                  │
     * │   Component1: [c1, c2, c3, ..., cN]                 │
     * │   Component2: [c1, c2, c3, ..., cN]                 │
     * │   ...                                                │
     * └─────────────────────────────────────────────────────┘
     */
    class RC_COMMON_API ArchetypeBase
    {
    public:
        virtual ~ArchetypeBase() = default;
        virtual bool contains(uint32 entityIndex) const = 0;
        virtual bool remove(uint32 entityIndex) = 0;
        virtual size_t size() const = 0;
        virtual size_t capacity() const = 0;
        virtual const std::vector<uint32>& entities() const = 0;
        
        // 新增接口用于内存分析
        virtual size_t memoryUsage() const = 0;
        virtual std::string getComponentTypes() const = 0;
    };

    template <typename... ComponentTypes>
    class Archetype : public ArchetypeBase
    {
    public:
        using ComponentTuple = std::tuple<ComponentTypes...>;
        static constexpr size_t CHUNK_SIZE = 16 * 1024; // 16KB 块，匹配 L1 缓存行
        static constexpr size_t DEFAULT_CAPACITY = 1024;

        Archetype() = default;
        ~Archetype() = default;

        Archetype(const Archetype&) = delete;
        Archetype(Archetype&&) noexcept = default;
        Archetype& operator=(const Archetype&) = delete;
        Archetype& operator=(Archetype&&) noexcept = default;

        /**
         * @brief 检查是否包含实体
         */
        [[nodiscard]] bool contains(uint32 entityIndex) const override
        {
            return m_entityToPosition.find(entityIndex) != m_entityToPosition.end();
        }

        /**
         * @brief 添加实体（带构造参数）
         * 使用 tuple 传递所有组件的构造参数
         */
        template <typename... Args>
        void emplace(uint32 entityIndex, Args&&... args)
        {
            fprintf(stderr, "Archetype::emplace: entity=%u position=%zu\n", entityIndex, m_entityToPosition.size());
            static_assert(sizeof...(ComponentTypes) == sizeof...(Args),
                "Number of arguments must match number of component types");

            size_t position = m_entityToPosition.size();

            // 检查是否需要扩容
            if (position >= m_capacity)
            {
                reserve(std::max(m_capacity * 2, DEFAULT_CAPACITY));
            }

            // 存储实体位置
            m_entityToPosition[entityIndex] = position;
            m_entityIndices.push_back(entityIndex);

            // 存储组件数据（原地构造，避免拷贝）
            emplaceComponents<0, Args...>(std::forward<Args>(args)...);
        }

        /**
         * @brief 添加实体（使用默认构造）
         */
        void emplace(uint32 entityIndex)
        {
            size_t position = m_entityToPosition.size();

            // 检查是否需要扩容
            if (position >= m_capacity)
            {
                reserve(std::max(m_capacity * 2, DEFAULT_CAPACITY));
            }

            // 存储实体位置
            m_entityToPosition[entityIndex] = position;
            m_entityIndices.push_back(entityIndex);

            // 存储组件数据（默认构造）
            emplaceDefaultComponents<0>();
        }

        /**
         * @brief 移除实体（O(1) 交换删除）
         */
        bool remove(uint32 entityIndex) override
        {
            auto it = m_entityToPosition.find(entityIndex);
            if (it == m_entityToPosition.end()) return false;

            size_t position = it->second;
            size_t lastPosition = m_entityToPosition.size() - 1;

            if (position != lastPosition)
            {
                // 将最后一个实体移到当前位置
                uint32 lastEntity = m_entityIndices.back();
                m_entityToPosition[lastEntity] = position;
                m_entityIndices[position] = lastEntity;

                // 移动组件数据
                moveComponents<0>(position, lastPosition);
            }

            // 移除最后一个元素
            m_entityIndices.pop_back();
            m_entityToPosition.erase(it);

            // 弹出最后一个组件
            popComponents<0>();

            return true;
        }

        /**
         * @brief 获取实体数量
         */
        [[nodiscard]] size_t size() const override
        {
            return m_entityToPosition.size();
        }

        /**
         * @brief 获取容量
         */
        [[nodiscard]] size_t capacity() const override
        {
            return m_capacity;
        }

        /**
         * @brief 预留容量
         */
        void reserve(size_t newCapacity)
        {
            if (newCapacity <= m_capacity) return;

            m_capacity = newCapacity;
            reserveComponents<0>(newCapacity);
        }

        /**
         * @brief 获取实体的组件
         */
        [[nodiscard]] std::tuple<ComponentTypes*...> tryGet(uint32 entityIndex)
        {
            auto it = m_entityToPosition.find(entityIndex);
            if (it == m_entityToPosition.end())
            {
                return std::tuple<ComponentTypes*...>(static_cast<ComponentTypes*>(nullptr)...);
            }

            return getComponents<0>(it->second);
        }

        /**
         * @brief 获取实体的组件 (const 版本)
         */
        [[nodiscard]] std::tuple<const ComponentTypes*...> tryGet(uint32 entityIndex) const
        {
            auto it = m_entityToPosition.find(entityIndex);
            if (it == m_entityToPosition.end())
            {
                return std::tuple<const ComponentTypes*...>(static_cast<const ComponentTypes*>(nullptr)...);
            }

            return getComponentsConst<0>(it->second);
        }

        /**
         * @brief 获取实体的组件（直接返回引用，更快）
         */
        [[nodiscard]] std::tuple<ComponentTypes&...> get(uint32 entityIndex)
        {
            auto it = m_entityToPosition.find(entityIndex);
            if (it == m_entityToPosition.end())
            {
                // 如果实体不存在，抛出异常（比返回 nullptr 更安全）
                throw std::runtime_error("Entity not found in archetype");
            }

            return getComponentsRef<0>(it->second);
        }

        /**
         * @brief 获取实体的组件（const 版本）
         */
        [[nodiscard]] std::tuple<const ComponentTypes&...> get(uint32 entityIndex) const
        {
            auto it = m_entityToPosition.find(entityIndex);
            if (it == m_entityToPosition.end())
            {
                throw std::runtime_error("Entity not found in archetype");
            }

            return getComponentsRefConst<0>(it->second);
        }

        /**
         * @brief 获取指定类型的组件（从多组件 Archetype 中）
         */
        template <typename T>
        [[nodiscard]] T* getComponent(uint32 entityIndex)
        {
            auto it = m_entityToPosition.find(entityIndex);
            if (it == m_entityToPosition.end())
            {
                return nullptr;
            }

            return getComponentByType<T, 0>(it->second);
        }

        /**
         * @brief 更新实体的指定组件
         * @param entityIndex 实体索引
         * @param component 新的组件值
         * @return 是否更新成功
         */
        template <typename T, typename... Args>
        bool updateComponent(uint32 entityIndex, Args&&... args)
        {
            auto it = m_entityToPosition.find(entityIndex);
            if (it == m_entityToPosition.end())
            {
                return false;
            }

            return updateComponentByType<T, 0>(it->second, std::forward<Args>(args)...);
        }

        /**
         * @brief 高性能遍历 - SOA 缓存友好
         */
        template <typename Func>
        void each(Func&& func)
        {
            const size_t count = size();
            for (size_t i = 0; i < count; ++i)
            {
                // 预取下一个元素（如果存在）
                if (i + 1 < count)
                {
                    prefetch<0>(i + 1);
                }

                Entity entity(m_entityIndices[i], 0);
                invokeEach<0>(i, entity, std::forward<Func>(func));
            }
        }

        /**
         * @brief 获取实体索引数组
         */
        [[nodiscard]] const std::vector<uint32>& entities() const override
        {
            return m_entityIndices;
        }

        /**
         * @brief 获取内存使用量
         */
        [[nodiscard]] size_t memoryUsage() const override
        {
            size_t total = 0;
            // 计算实体索引数组内存
            total += m_entityIndices.capacity() * sizeof(uint32);
            // 计算组件数据内存
            total += calculateMemoryUsage<0>();
            // 计算映射表内存
            total += m_entityToPosition.size() * (sizeof(uint32) + sizeof(size_t));
            return total;
        }

        /**
         * @brief 获取组件类型信息
         */
        [[nodiscard]] std::string getComponentTypes() const override
        {
            std::ostringstream oss;
            oss << "[";
            printComponentTypes<0>(oss);
            oss << "]";
            return oss.str();
        }

    private:
        // 实体索引到位置的映射（O(1) 查找）
        std::unordered_map<uint32, size_t> m_entityToPosition;

        // 实体索引数组（紧凑存储）
        std::vector<uint32> m_entityIndices;

        // 组件数据存储（SOA 布局）
        std::tuple<std::vector<ComponentTypes>...> m_components;

        // 当前容量
        size_t m_capacity = 0;

        // ========== 组件操作辅助函数 ==========

        template <size_t Index>
        [[nodiscard]] auto getComponents(size_t position)
        {
            auto& componentVec = std::get<Index>(m_components);
            if constexpr (Index + 1 == sizeof...(ComponentTypes))
            {
                return std::make_tuple(&componentVec[position]);
            }
            else
            {
                return std::tuple_cat(
                    std::make_tuple(&componentVec[position]),
                    getComponents<Index + 1>(position)
                );
            }
        }

        template <size_t Index>
        [[nodiscard]] auto getComponentsConst(size_t position) const
        {
            const auto& componentVec = std::get<Index>(m_components);
            if constexpr (Index + 1 == sizeof...(ComponentTypes))
            {
                return std::make_tuple(&componentVec[position]);
            }
            else
            {
                return std::tuple_cat(
                    std::make_tuple(&componentVec[position]),
                    getComponentsConst<Index + 1>(position)
                );
            }
        }

        template <size_t Index>
        [[nodiscard]] auto getComponentsRef(size_t position)
        {
            auto& componentVec = std::get<Index>(m_components);
            if constexpr (Index + 1 == sizeof...(ComponentTypes))
            {
                return std::tie(componentVec[position]);
            }
            else
            {
                return std::tuple_cat(
                    std::tie(componentVec[position]),
                    getComponentsRef<Index + 1>(position)
                );
            }
        }

        template <size_t Index>
        [[nodiscard]] auto getComponentsRefConst(size_t position) const
        {
            const auto& componentVec = std::get<Index>(m_components);
            if constexpr (Index + 1 == sizeof...(ComponentTypes))
            {
                return std::forward_as_tuple(componentVec[position]);
            }
            else
            {
                return std::tuple_cat(
                    std::forward_as_tuple(componentVec[position]),
                    getComponentsRefConst<Index + 1>(position)
                );
            }
        }

        template <size_t Index>
        void reserveComponents(size_t newCapacity)
        {
            std::get<Index>(m_components).reserve(newCapacity);
            if constexpr (Index + 1 < sizeof...(ComponentTypes))
            {
                reserveComponents<Index + 1>(newCapacity);
            }
        }

        template <size_t Index, typename FirstArg, typename... RestArgs>
        void emplaceComponents(FirstArg&& firstArg, RestArgs&&... restArgs)
        {
            auto& componentVec = std::get<Index>(m_components);
            componentVec.emplace_back(std::forward<FirstArg>(firstArg));

            if constexpr (Index + 1 < sizeof...(ComponentTypes))
            {
                emplaceComponents<Index + 1, RestArgs...>(std::forward<RestArgs>(restArgs)...);
            }
        }

        template <size_t Index>
        void emplaceComponents()
        {
            // 所有组件都已处理完毕
        }

        template <size_t Index>
        void emplaceDefaultComponents()
        {
            auto& componentVec = std::get<Index>(m_components);
            componentVec.emplace_back();

            if constexpr (Index + 1 < sizeof...(ComponentTypes))
            {
                emplaceDefaultComponents<Index + 1>();
            }
        }

        template <size_t Index>
        void moveComponents(size_t targetPos, size_t sourcePos)
        {
            std::get<Index>(m_components)[targetPos] = std::move(std::get<Index>(m_components)[sourcePos]);
            if constexpr (Index + 1 < sizeof...(ComponentTypes))
            {
                moveComponents<Index + 1>(targetPos, sourcePos);
            }
        }

        template <size_t Index>
        void popComponents()
        {
            std::get<Index>(m_components).pop_back();
            if constexpr (Index + 1 < sizeof...(ComponentTypes))
            {
                popComponents<Index + 1>();
            }
        }

        template <size_t Index>
        void prefetch(size_t position)
        {
            const auto& componentVec = std::get<Index>(m_components);
            __builtin_prefetch(&componentVec[position]);
            if constexpr (Index + 1 < sizeof...(ComponentTypes))
            {
                prefetch<Index + 1>(position);
            }
        }

        template <size_t Index, typename Func, typename... Args>
        void invokeEachImpl(size_t position, Entity entity, Func&& func, Args&&... args)
        {
            if constexpr (Index + 1 < sizeof...(ComponentTypes))
            {
                // 递归收集组件参数
                invokeEachImpl<Index + 1>(position, entity, std::forward<Func>(func),
                                         std::forward<Args>(args)...,
                                         std::get<Index>(m_components)[position]);
            }
            else
            {
                // 最后一个组件，调用函数
                func(entity, std::forward<Args>(args)..., std::get<Index>(m_components)[position]);
            }
        }

        template <size_t Index, typename Func>
            void invokeEach(size_t position, Entity entity, Func&& func)
            {
                invokeEachImpl<0>(position, entity, std::forward<Func>(func));
            }

        template <typename T, size_t Index>
        [[nodiscard]] T* getComponentByType(size_t position)
        {
            if constexpr (Index < sizeof...(ComponentTypes))
            {
                using CurrentType = typename std::tuple_element<Index, std::tuple<ComponentTypes...>>::type;

                if (std::is_same_v<T, CurrentType>)
                {
                    auto& componentVec = std::get<Index>(m_components);
                    if (position < componentVec.size())
                    {
                        return &componentVec[position];
                    }
                }
                else
                {
                    return getComponentByType<T, Index + 1>(position);
                }
            }

            return nullptr;
        }

        // 更新组件辅助函数
        template <typename T, size_t Index, typename... Args>
        bool updateComponentByType(size_t position, Args&&... args)
        {
            if constexpr (Index < sizeof...(ComponentTypes))
            {
                using CurrentType = typename std::tuple_element<Index, std::tuple<ComponentTypes...>>::type;

                if (std::is_same_v<T, CurrentType>)
                {
                    auto& componentVec = std::get<Index>(m_components);
                    if (position < componentVec.size())
                    {
                        componentVec[position] = T(std::forward<Args>(args)...);
                        return true;
                    }
                }
                else
                {
                    return updateComponentByType<T, Index + 1>(position, std::forward<Args>(args)...);
                }
            }

            return false;
        }

        // 内存使用计算辅助函数
        template <size_t Index>
        [[nodiscard]] size_t calculateMemoryUsage() const
        {
            const auto& componentVec = std::get<Index>(m_components);
            size_t usage = componentVec.capacity() * sizeof(typename std::tuple_element<Index, std::tuple<ComponentTypes...>>::type);
            
            if constexpr (Index + 1 < sizeof...(ComponentTypes))
            {
                usage += calculateMemoryUsage<Index + 1>();
            }
            
            return usage;
        }

        // 组件类型信息打印辅助函数
        template <size_t Index>
        void printComponentTypes(std::ostringstream& oss) const
        {
            using ComponentType = typename std::tuple_element<Index, std::tuple<ComponentTypes...>>::type;
            
            if constexpr (Index > 0)
            {
                oss << ", ";
            }
            oss << typeid(ComponentType).name();
            
            if constexpr (Index + 1 < sizeof...(ComponentTypes))
            {
                printComponentTypes<Index + 1>(oss);
            }
        }
    };

END_NAMESPACE_ECS

#endif //RENDU_ECS_ARCHETYPE_H
