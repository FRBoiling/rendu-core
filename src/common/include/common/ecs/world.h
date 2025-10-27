#pragma once

#include "registry.h"
#include "i_system.h"
#include <memory>
#include <string>
#include <vector>
#include <condition_variable>
#include <functional>
#include <typeindex>
#include "common/threading/thread_pool.h"

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
            
            // 添加系统（使用类型擦除）
            template <typename SystemType, typename... Args>
            SystemType* add_system(Args&&... args);
            
            // 按类型获取系统（使用类型擦除）
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
            
            // 实体和组件管理接口（转发到registry）
            Entity create_entity();
            void destroy_entity(Entity entity);
            bool is_valid(Entity entity) const;
            
            // 组件管理
            template <typename ComponentType>
            void register_component();
            
            template <typename ComponentType>
            void add_component(Entity entity, ComponentType&& component);
            
            template <typename ComponentType>
            void remove_component(Entity entity);
            
            template <typename ComponentType>
            bool has_component(Entity entity) const;
            
            template <typename ComponentType>
            ComponentType* get_component(Entity entity);
            
            template <typename ComponentType>
            const ComponentType* get_component(Entity entity) const;
            
            // 视图创建（模板方法，但实现隐藏在.cpp中）
            template <typename... ComponentTypes>
            Registry::EntityView create_view();
            
            // 事件系统
            template <typename EventType>
            void subscribe(std::function<void(const EventType&)> callback);
            
            template <typename EventType>
            void publish(const EventType& event);
            
            // 获取线程池
            Threading::ThreadPool& get_thread_pool();
            
        private:
            // 系统依赖排序
            std::vector<ISystem*> topological_sort_systems();
            
            Registry registry_;
            std::unordered_map<std::string, std::unique_ptr<ISystem>> systems_;
            std::unordered_map<std::type_index, ISystem*> system_type_map_;
            Threading::ThreadPool thread_pool_;
            std::vector<ISystem*> sorted_systems_;
        };
        
        // World模板实现（这些模板方法的实现会在.cpp文件中提供）
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
        
        // 组件管理模板方法声明
        template <typename ComponentType>
        void World::register_component() {
            registry_.register_component(std::type_index(typeid(ComponentType)), sizeof(ComponentType));
        }
        
        template <typename ComponentType>
        void World::add_component(Entity entity, ComponentType&& component) {
            registry_.add_component(entity, std::type_index(typeid(ComponentType)), &component);
        }
        
        template <typename ComponentType>
        void World::remove_component(Entity entity) {
            registry_.remove_component(entity, std::type_index(typeid(ComponentType)));
        }
        
        template <typename ComponentType>
        bool World::has_component(Entity entity) const {
            return registry_.has_component(entity, std::type_index(typeid(ComponentType)));
        }
        
        template <typename ComponentType>
        ComponentType* World::get_component(Entity entity) {
            return static_cast<ComponentType*>(registry_.get_component(entity, std::type_index(typeid(ComponentType))));
        }
        
        template <typename ComponentType>
        const ComponentType* World::get_component(Entity entity) const {
            return static_cast<const ComponentType*>(registry_.get_component(entity, std::type_index(typeid(ComponentType))));
        }
        
        // 视图创建模板方法
        template <typename... ComponentTypes>
        Registry::EntityView World::create_view() {
            return registry_.create_view({std::type_index(typeid(ComponentTypes))...});
        }
        
        // 事件系统模板方法
        template <typename EventType>
        void World::subscribe(std::function<void(const EventType&)> callback) {
            registry_.subscribe(std::type_index(typeid(EventType)), 
                [callback](const void* event_data) {
                    callback(*static_cast<const EventType*>(event_data));
                });
        }
        
        template <typename EventType>
        void World::publish(const EventType& event) {
            registry_.publish(std::type_index(typeid(EventType)), &event);
        }
    } // namespace Ecs

END_NAMESPACE_COMMON