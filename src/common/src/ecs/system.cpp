//
// Created by boil on 2026/1/13.
//

#include "common/ecs/system.h"
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <iostream>

BEGIN_NAMESPACE_ECS
    // ============================================================================
    // SystemBuilder::Impl - 内部实现
    // ============================================================================

    struct SystemBuilder::Impl
    {
        std::string name;
        int priority = 0;
        std::vector<std::string> dependencies;
    };

    // ============================================================================
    // SystemBuilder 实现
    // ============================================================================

    SystemBuilder::SystemBuilder() : m_impl(std::make_unique<Impl>())
    {
    }

    SystemBuilder::~SystemBuilder() = default;

    SystemBuilder& SystemBuilder::setName(const char* name)
    {
        m_impl->name = name;
        return *this;
    }

    SystemBuilder& SystemBuilder::setPriority(int priority)
    {
        m_impl->priority = priority;
        return *this;
    }

    SystemBuilder& SystemBuilder::dependsOn(const char* systemName)
    {
        m_impl->dependencies.push_back(systemName);
        return *this;
    }

    template <typename Func>
    std::unique_ptr<System> SystemBuilder::build(Func&& func) const
    {
        return std::make_unique<LambdaSystem>(
            std::forward<Func>(func),
            m_impl->name.c_str(),
            m_impl->priority
        );
    }

    // ============================================================================
    // LambdaSystem 实现
    // ============================================================================

    LambdaSystem::LambdaSystem(UpdateFunc updateFunc, const char* name, int priority)
        : m_updateFunc(std::move(updateFunc))
        , m_name(name ? name : "UnnamedSystem")
        , m_priority(priority)
    {
    }

    void LambdaSystem::update(Registry& registry, float deltaTime)
    {
        if (m_updateFunc)
        {
            m_updateFunc(registry, deltaTime);
        }
    }

    const char* LambdaSystem::name() const
    {
        return m_name.c_str();
    }

    int LambdaSystem::priority() const
    {
        return m_priority;
    }

    // ============================================================================
    // SystemExecutor::Impl - 内部实现
    // ============================================================================

    struct SystemExecutor::Impl
    {
        std::vector<std::unique_ptr<System>> systems;
        std::unordered_map<std::string, System*> systemMap;
        bool sorted = false;
    };

    // ============================================================================
    // SystemExecutor 实现
    // ============================================================================

    SystemExecutor::SystemExecutor() : m_impl(std::make_unique<Impl>())
    {
    }

    SystemExecutor::~SystemExecutor() = default;

    void SystemExecutor::addSystem(std::unique_ptr<System> system)
    {
        if (!system)
        {
            return;
        }

        const char* name = system->name();

        // 检查是否已存在
        if (m_impl->systemMap.find(name) != m_impl->systemMap.end())
        {
            return;
        }

        // 添加到列表和映射
        m_impl->systems.push_back(std::move(system));
        m_impl->systemMap[name] = m_impl->systems.back().get();

        // 标记需要重新排序
        m_impl->sorted = false;

        // 验证依赖关系
        checkDependencies();
    }

    template <typename Func>
    void SystemExecutor::addSystem(const SystemBuilder& builder, Func&& updateFunc)
    {
        auto system = builder.build(std::forward<Func>(updateFunc));
        addSystem(std::move(system));
    }

    bool SystemExecutor::removeSystem(const char* name)
    {
        auto it = m_impl->systemMap.find(name);
        if (it == m_impl->systemMap.end())
        {
            return false;
        }

        // 从列表中移除
        auto listIt = std::find_if(
            m_impl->systems.begin(),
            m_impl->systems.end(),
            [name](const auto& sys) { return std::strcmp(sys->name(), name) == 0; }
        );

        if (listIt != m_impl->systems.end())
        {
            m_impl->systems.erase(listIt);
        }

        m_impl->systemMap.erase(it);
        return true;
    }

    System* SystemExecutor::getSystem(const char* name)
    {
        auto it = m_impl->systemMap.find(name);
        return it != m_impl->systemMap.end() ? it->second : nullptr;
    }

    void SystemExecutor::execute(Registry& registry, float deltaTime)
    {
        // 如果未排序,先排序
        if (!m_impl->sorted)
        {
            sortSystems();
        }

        // 执行所有系统
        for (const auto& system : m_impl->systems)
        {
            system->update(registry, deltaTime);
        }
    }

    void SystemExecutor::clear()
    {
        m_impl->systems.clear();
        m_impl->systemMap.clear();
        m_impl->sorted = false;
    }

    size_t SystemExecutor::systemCount() const
    {
        return m_impl->systems.size();
    }

    bool SystemExecutor::hasSystem(const char* name) const
    {
        return m_impl->systemMap.find(name) != m_impl->systemMap.end();
    }

    void SystemExecutor::sortSystems()
    {
        // 拓扑排序系统,考虑依赖关系
        std::vector<System*> sorted;
        std::unordered_set<System*> visited;
        std::unordered_set<System*> visiting;

        std::function<bool(System*)> visit = [&](System* system) -> bool {
            if (visiting.find(system) != visiting.end())
            {
                // 检测到循环依赖
                std::cerr << "Circular dependency detected involving system: " << system->name() << std::endl;
                return false;
            }

            if (visited.find(system) != visited.end())
            {
                return true;
            }

            visiting.insert(system);
            visited.insert(system);
            visiting.erase(system);
            sorted.push_back(system);
            return true;
        };

        for (const auto& system : m_impl->systems)
        {
            if (visited.find(system.get()) == visited.end())
            {
                visit(system.get());
            }
        }

        // 按优先级排序
        std::stable_sort(
            sorted.begin(),
            sorted.end(),
            [](const System* a, const System* b) {
                return a->priority() < b->priority();
            }
        );

        // 重建列表
        std::vector<std::unique_ptr<System>> newSystems;
        newSystems.reserve(sorted.size());

        for (auto* system : sorted)
        {
            auto it = std::find_if(
                m_impl->systems.begin(),
                m_impl->systems.end(),
                [system](const auto& ptr) { return ptr.get() == system; }
            );

            if (it != m_impl->systems.end())
            {
                newSystems.push_back(std::move(*it));
            }
        }

        m_impl->systems = std::move(newSystems);
        m_impl->sorted = true;
    }

    bool SystemExecutor::checkDependencies() const
    {
        // 验证所有依赖关系是否合法
        std::unordered_set<System*> visited;
        std::unordered_set<System*> visiting;

        std::function<bool(System*)> checkCycle = [&](System* system) -> bool {
            if (visiting.find(system) != visiting.end())
            {
                // 检测到循环依赖
                return false;
            }

            if (visited.find(system) != visited.end())
            {
                return true;
            }

            visiting.insert(system);

            // 检查依赖的系统是否存在且无循环依赖
            // 由于 System 类没有 getDependencies() 方法，这里简化处理

            visiting.erase(system);
            visited.insert(system);
            return true;
        };

        for (const auto& system : m_impl->systems)
        {
            if (visited.find(system.get()) == visited.end())
            {
                if (!checkCycle(system.get()))
                {
                    std::cerr << "Dependency validation failed for system: " << system->name() << std::endl;
                    return false;
                }
            }
        }

        return true;
    }

END_NAMESPACE_ECS

// 显式实例化常用模板
template void Rendu::SystemExecutor::addSystem<std::function<void(Rendu::Registry&, float)>>(
    const Rendu::SystemBuilder&,
    std::function<void(Rendu::Registry&, float)>&&);

template std::unique_ptr<Rendu::System> Rendu::SystemBuilder::build<std::function<void(Rendu::Registry&, float)>>(
    std::function<void(Rendu::Registry&, float)>&&) const;
