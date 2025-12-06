#include "common/ecs/system_manager.h"
#include <stdexcept>
#include <queue>
#include <unordered_set>
#include <future>

BEGIN_NAMESPACE_COMMON
    using namespace Ecs;

    // ==================== 阶段执行模式 ====================

    PhaseExecutionMode SystemManager::get_phase_mode(SystemPhase phase) const
    {
        switch (phase)
        {
            case SystemPhase::CONFIGURE:
            case SystemPhase::INITIALIZE:
            case SystemPhase::UPDATE_PARALLEL:
            case SystemPhase::SHUTDOWN:
                return PhaseExecutionMode::PARALLEL;
            case SystemPhase::UPDATE_SEQUENTIAL:
            case SystemPhase::CLEANUP:
            default:
                return PhaseExecutionMode::ORDERED;
        }
    }

    const char* SystemManager::get_phase_mode_name(PhaseExecutionMode mode) const
    {
        switch (mode)
        {
            case PhaseExecutionMode::ORDERED:
                return "ORDERED";
            case PhaseExecutionMode::PARALLEL:
                return "PARALLEL";
            default:
                return "UNKNOWN";
        }
    }

    const char* SystemManager::get_system_phase_name(SystemPhase phase) const
    {
        switch (phase)
        {
            case SystemPhase::CONFIGURE:
                return "CONFIGURE";
            case SystemPhase::INITIALIZE:
                return "INITIALIZE";
            case SystemPhase::UPDATE_SEQUENTIAL:
                return "UPDATE_SEQUENTIAL";
            case SystemPhase::UPDATE_PARALLEL:
                return "UPDATE_PARALLEL";
            case SystemPhase::SHUTDOWN:
                return "SHUTDOWN";
            case SystemPhase::CLEANUP:
                return "CLEANUP";
            default:
                return "UNKNOWN";
        }
    }

    // ==================== 系统阶段执行 ====================

    void SystemManager::configure_all(World* world)
    {
        RC_LOG_INFO("application", "Configuring systems...");
        execute_phase(world, SystemPhase::CONFIGURE);
        RC_LOG_INFO("application", "Systems configured");
    }

    void SystemManager::initialize_all()
    {
        RC_LOG_INFO("application", "Initializing systems...");
        execute_phase(nullptr, SystemPhase::INITIALIZE);
        RC_LOG_INFO("application", "Systems initialized");
    }

    void SystemManager::update_all(float delta_time)
    {
        execute_phase(nullptr, SystemPhase::UPDATE_SEQUENTIAL, delta_time);
        execute_phase(nullptr, SystemPhase::UPDATE_PARALLEL, delta_time);
    }

    void SystemManager::shutdown_all()
    {
        RC_LOG_INFO("application", "Shutting down systems...");
        execute_phase(nullptr, SystemPhase::SHUTDOWN);
        RC_LOG_INFO("application", "Systems shutdown complete");
    }

    void SystemManager::cleanup_all()
    {
        RC_LOG_INFO("application", "Cleaning up systems...");
        execute_phase(nullptr, SystemPhase::CLEANUP);
        RC_LOG_INFO("application", "Systems cleanup complete");
    }

    void SystemManager::execute_phase(World* world, SystemPhase phase, float delta_time)
    {
        const PhaseExecutionMode mode = get_phase_mode(phase);
        if (mode == PhaseExecutionMode::PARALLEL)
        {
            // 并行执行模式：按依赖层级分组，同层级系统可并行执行
            for (const auto& level : leveled_systems_)
            {
                if (level.empty()) continue;

                // 分离并发系统和独占系统
                std::vector<ISystem*> concurrent_systems;
                std::vector<ISystem*> exclusive_systems;

                for (auto* system : level)
                {
                    if (system->get_execution_mode() == SystemExecutionMode::SEQUENTIAL)
                    {
                        exclusive_systems.push_back(system);
                    }
                    else
                    {
                        concurrent_systems.push_back(system);
                    }
                }

                // 先并行执行并发系统
                if (!concurrent_systems.empty())
                {
                    std::vector<std::future<void>> futures;
                    futures.reserve(concurrent_systems.size());

                    for (auto* system : concurrent_systems)
                    {
                        futures.push_back(
                            thread_pool_.Submit([world, system, phase, delta_time]() {
                                switch (phase)
                                {
                                    case SystemPhase::CONFIGURE:
                                        system->configure(world);
                                        RC_LOG_TRACE("application", "System {} configured (concurrently", system->get_name());
                                        break;
                                    case SystemPhase::INITIALIZE:
                                        system->initialize();
                                        RC_LOG_INFO("application", "System {} initialized (concurrently)", system->get_name());
                                        break;
                                    case SystemPhase::UPDATE_SEQUENTIAL:
                                        system->update_sequential(delta_time);
                                        RC_LOG_TRACE("application", "System {} update_sequential (concurrently) delta_time: {}", system->get_name(), delta_time);
                                        break;
                                    case SystemPhase::UPDATE_PARALLEL:
                                        system->update_parallel(delta_time);
                                        RC_LOG_TRACE("application", "System {} update_parallel (concurrently) delta_time: {}", system->get_name(), delta_time);
                                        break;
                                    case SystemPhase::SHUTDOWN:
                                        system->shutdown();
                                        RC_LOG_INFO("application", "System {} shutdown (concurrently)", system->get_name());
                                        break;
                                    case SystemPhase::CLEANUP:
                                        system->cleanup();
                                        RC_LOG_TRACE("application", "System {} cleanup (concurrently)", system->get_name());
                                        break;
                                }
                            })
                        );
                    }

                    for (auto& future : futures)
                    {
                        future.wait();
                    }
                }

                // 然后按顺序执行独占系统（确保在所有并发系统完成后）
                for (auto* system : exclusive_systems)
                {
                    switch (phase)
                    {
                        case SystemPhase::CONFIGURE:
                            system->configure(world);
                            RC_LOG_TRACE("application", "System {} configured (exclusive)", system->get_name());
                            break;
                        case SystemPhase::INITIALIZE:
                            system->initialize();
                            RC_LOG_INFO("application", "System {} initialized (exclusive)", system->get_name());
                            break;
                        case SystemPhase::UPDATE_SEQUENTIAL:
                            system->update_sequential(delta_time);
                            RC_LOG_TRACE("application", "System {} update_sequential (exclusive) delta_time: {}", system->get_name(), delta_time);
                            break;
                        case SystemPhase::UPDATE_PARALLEL:
                            system->update_parallel(delta_time);
                            RC_LOG_TRACE("application", "System {} update_parallel  (exclusive) delta_time: {}", system->get_name(), delta_time);
                            break;
                        case SystemPhase::SHUTDOWN:
                            system->shutdown();
                            RC_LOG_INFO("application", "System {} shutdown (exclusive)", system->get_name());
                            break;
                        case SystemPhase::CLEANUP:
                            system->cleanup();
                            RC_LOG_TRACE("application", "System {} cleanup (exclusive)", system->get_name());
                            break;
                    }
                }
            }
        }
        else
        {
            // 有序执行模式：按依赖关系顺序执行
            for (auto* system : sorted_systems_)
            {
                switch (phase)
                {
                    case SystemPhase::CONFIGURE:
                        system->configure(world);
                        RC_LOG_TRACE("application", "System {} configure", system->get_name());
                        break;
                    case SystemPhase::INITIALIZE:
                        system->initialize();
                        RC_LOG_INFO("application", "System {} initialize", system->get_name());
                        break;
                    case SystemPhase::UPDATE_SEQUENTIAL:
                        system->update_sequential(delta_time);
                        RC_LOG_TRACE("application", "System {} update_sequential delta_time: {}", system->get_name(), delta_time);
                        break;
                    case SystemPhase::UPDATE_PARALLEL:
                        system->update_parallel(delta_time);
                        RC_LOG_TRACE("application", "System {} update_parallel delta_time: {}", system->get_name(), delta_time);
                        break;
                    case SystemPhase::SHUTDOWN:
                        system->shutdown();
                        RC_LOG_INFO("application", "System {} shutdown", system->get_name());
                        break;
                    case SystemPhase::CLEANUP:
                        system->cleanup();
                        RC_LOG_TRACE("application", "System {} cleanup", system->get_name());
                        break;
                }
            }
        }
    }

    // ==================== 依赖排序 ====================

    std::vector<ISystem*> SystemManager::topological_sort_systems()
    {
        std::unordered_map<ISystem*, std::vector<ISystem*>> graph;
        std::unordered_map<ISystem*, int> in_degree;
        std::vector<ISystem*> sorted;

        // 构建依赖图
        for (auto& [name, system] : systems_)
        {
            const auto& dependencies = system->get_dependencies();
            in_degree[system.get()] = 0;

            for (const auto& dep_name : dependencies)
            {
                auto it = systems_.find(dep_name);
                if (it != systems_.end())
                {
                    graph[it->second.get()].push_back(system.get());
                    in_degree[system.get()]++;
                }
            }
        }

        // Kahn算法拓扑排序
        std::queue<ISystem*> q;
        for (auto& [system, degree] : in_degree)
        {
            if (degree == 0)
            {
                q.push(system);
            }
        }

        while (!q.empty())
        {
            auto system = q.front();
            q.pop();
            sorted.push_back(system);

            for (auto dep : graph[system])
            {
                in_degree[dep]--;
                if (in_degree[dep] == 0)
                {
                    q.push(dep);
                }
            }
        }

        // 检查是否有循环依赖
        if (sorted.size() != systems_.size())
        {
            throw std::runtime_error("Circular dependency detected in systems");
        }

        // 更新按层级分组的系统
        leveled_systems_ = group_systems_by_level();

        return sorted;
    }

    std::vector<std::vector<ISystem*>> SystemManager::group_systems_by_level()
    {
        std::unordered_map<ISystem*, std::vector<ISystem*>> graph;
        std::unordered_map<ISystem*, int> in_degree;
        std::unordered_map<ISystem*, int> level;
        std::vector<std::vector<ISystem*>> levels;

        // 构建依赖图和入度
        for (auto& [name, system] : systems_)
        {
            const auto& dependencies = system->get_dependencies();
            in_degree[system.get()] = 0;
            level[system.get()] = 0;

            for (const auto& dep_name : dependencies)
            {
                auto it = systems_.find(dep_name);
                if (it != systems_.end())
                {
                    graph[it->second.get()].push_back(system.get());
                    in_degree[system.get()]++;
                }
            }
        }

        // 按层级分层系统
        std::queue<ISystem*> q;
        std::unordered_set<ISystem*> processed;

        // 找出所有入度为0的系统作为第0层
        for (auto& [system, degree] : in_degree)
        {
            if (degree == 0)
            {
                q.push(system);
                level[system] = 0;
            }
        }

        // BFS计算每个系统的层级
        while (!q.empty())
        {
            auto system = q.front();
            q.pop();

            if (processed.find(system) == processed.end())
            {
                processed.insert(system);
            }

            for (auto dep : graph[system])
            {
                // 更新层级为：当前层级 + 1
                level[dep] = std::max(level[dep], level[system] + 1);
                in_degree[dep]--;

                if (in_degree[dep] == 0)
                {
                    q.push(dep);
                }
            }
        }

        // 按层级分组
        int max_level = 0;
        for (auto& [system, lvl] : level)
        {
            max_level = std::max(max_level, lvl);
        }

        levels.resize(max_level + 1);
        for (auto& [system, lvl] : level)
        {
            levels[lvl].push_back(system);
        }

        RC_LOG_DEBUG("application", "Systems grouped into {} parallel levels", levels.size());
        for (size_t i = 0; i < levels.size(); ++i)
        {
            RC_LOG_DEBUG("application", "  Level {}: {} systems", i, levels[i].size());
        }

        return levels;
    }

END_NAMESPACE_COMMON
