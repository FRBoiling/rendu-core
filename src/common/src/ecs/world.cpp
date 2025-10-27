#include "common/ecs/world.h"
#include <stdexcept>

using namespace common::Ecs;

World::World(size_t num_threads)
    : thread_pool_(num_threads) {
}

World::~World() {
    shutdown_systems();
}

ISystem* World::get_system(const std::string& name) {
    auto it = systems_.find(name);
    if (it != systems_.end()) {
        return it->second.get();
    }
    return nullptr;
}

void World::configure_systems() {
    execute_phase(SystemPhase::PRE_INIT);
    
    for (auto& system : sorted_systems_) {
        system->configure(this);
    }
}

void World::initialize_systems() {
    execute_phase(SystemPhase::INIT);
}

void World::update_systems(float delta_time) {
    execute_phase(SystemPhase::UPDATE, delta_time);
    execute_phase(SystemPhase::POST_UPDATE, delta_time);
}

void World::shutdown_systems() {
    execute_phase(SystemPhase::SHUTDOWN);
    execute_phase(SystemPhase::POST_SHUTDOWN);
}

void World::execute_phase(SystemPhase phase, float delta_time) {
    std::vector<std::future<void>> futures;
    
    // 收集所有系统到一个临时容器
    std::vector<ISystem*> current_phase_systems = sorted_systems_;
    
    // 提交所有系统的更新任务到线程池
    for (auto& system : current_phase_systems) {
        switch (phase) {
            case SystemPhase::INIT:
                futures.push_back(thread_pool_.submit(&ISystem::initialize, system));
                break;
            case SystemPhase::UPDATE:
                futures.push_back(thread_pool_.submit(&ISystem::update, system, delta_time));
                break;
            case SystemPhase::SHUTDOWN:
                futures.push_back(thread_pool_.submit(&ISystem::shutdown, system));
                break;
            // 其他阶段暂时不支持多线程
            default:
                switch (phase) {
                    case SystemPhase::PRE_INIT:
                    case SystemPhase::POST_UPDATE:
                    case SystemPhase::POST_SHUTDOWN:
                        // 这些阶段在单线程中执行
                        break;
                    default:
                        break;
                }
                break;
        }
    }
    
    // 等待所有任务完成
    for (auto& future : futures) {
        future.wait();
    }
    
    thread_pool_.wait_for_all();
}

std::vector<ISystem*> World::topological_sort_systems() {
    std::unordered_map<ISystem*, std::vector<ISystem*>> graph;
    std::unordered_map<ISystem*, int> in_degree;
    std::vector<ISystem*> sorted;
    
    // 构建依赖图
    for (auto& [name, system] : systems_) {
        const auto& dependencies = system->get_dependencies();
        in_degree[system.get()] = 0;
        
        for (const auto& dep_name : dependencies) {
            auto it = systems_.find(dep_name);
            if (it != systems_.end()) {
                graph[it->second.get()].push_back(system.get());
                in_degree[system.get()]++;
            }
        }
    }
    
    // Kahn算法拓扑排序
    std::queue<ISystem*> q;
    for (auto& [system, degree] : in_degree) {
        if (degree == 0) {
            q.push(system);
        }
    }
    
    while (!q.empty()) {
        auto system = q.front();
        q.pop();
        sorted.push_back(system);
        
        for (auto dep : graph[system]) {
            in_degree[dep]--;
            if (in_degree[dep] == 0) {
                q.push(dep);
            }
        }
    }
    
    // 检查是否有循环依赖
    if (sorted.size() != systems_.size()) {
        throw std::runtime_error("Circular dependency detected in systems");
    }
    
    return sorted;
}