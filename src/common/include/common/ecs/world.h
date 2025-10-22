#pragma once

#include "registry.h"
#include "i_system.h"

#include <memory>
#include <string>
#include <vector>
#include <future>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <typeindex>

#include "common/threading/thead_pool.h"


BEGIN_NAMESPACE_COMMON
    namespace Ecs
    {
        // 系统执行阶段
        enum class SystemPhase
        {
            PRE_INIT,
            INIT,
            UPDATE,
            POST_UPDATE,
            SHUTDOWN,
            POST_SHUTDOWN
        };



        // 世界类，ECS的中央管理器
        class World
        {
        public:
            World(size_t num_threads = 4);
            ~World();

            // 添加系统
            template <typename SystemType, typename... Args>
            SystemType* add_system(Args&&... args);

            // 获取系统
            template <typename SystemType>
            SystemType* get_system();

            // 按名称获取系统
            ISystem* get_system(const std::string& name);

            // 配置所有系统
            void configure_systems();

            // 初始化所有系统
            void initialize_systems();

            // 更新所有系统
            void update_systems(float delta_time);

            // 关闭所有系统
            void shutdown_systems();

            // 执行指定阶段的系统
            void execute_phase(SystemPhase phase, float delta_time = 0.0f);

            // 获取注册表
            Registry& get_registry() { return registry_; }
            const Registry& get_registry() const { return registry_; }

            // 获取线程池
            ThreadPool& get_thread_pool() { return thread_pool_; }

        private:
            // 系统依赖排序
            std::vector<ISystem*> topological_sort_systems();

            Registry registry_;
            std::unordered_map<std::string, std::unique_ptr<ISystem>> systems_;
            std::unordered_map<std::type_index, ISystem*> system_type_map_;
            ThreadPool thread_pool_;
            std::vector<ISystem*> sorted_systems_;
        };


        // World模板实现

        template <typename SystemType, typename... Args>
        SystemType* World::add_system(Args&&... args)
        {
            static_assert(std::is_base_of<ISystem, SystemType>::value, "SystemType must derive from ISystem");

            auto system = std::make_unique<SystemType>(std::forward<Args>(args)...);
            auto system_ptr = system.get();

            std::string name = system->get_name();
            systems_[name] = std::move(system);
            system_type_map_[std::type_index(typeid(SystemType))] = system_ptr;

            // 重新排序系统依赖
            sorted_systems_ = topological_sort_systems();

            return system_ptr;
        }

        template <typename SystemType>
        SystemType* World::get_system()
        {
            auto it = system_type_map_.find(std::type_index(typeid(SystemType)));
            if (it != system_type_map_.end())
            {
                return static_cast<SystemType*>(it->second);
            }
            return nullptr;
        }
    } // namespace ecs
END_NAMESPACE_COMMON
