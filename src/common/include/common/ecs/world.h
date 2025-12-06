#pragma once

#include "registry.h"
#include "i_system.h"
#include "parallel_for.h"
#include "system_manager.h"
#include <memory>
#include <string>
#include <vector>
#include <condition_variable>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <sys/syslog.h>

#include "common/threading/thread_pool_adapter.h"
#include "application.h"
#include "common/logging/log.h"

BEGIN_NAMESPACE_COMMON
    namespace Ecs
    {
        // 世界类，ECS的中央管理器
        class World
        {
        public:
            World(Application* application);
            ~World();

            // 设置应用程序引用
            void set_application(Application* application);

            // 获取应用程序引用
            Application* get_application() const { return application_; }

            // ==================== 系统管理接口 ====================

            // 添加系统
            template <typename SystemType, typename... Args>
            SystemType* add_system(Args&&... args)
            {
                return system_manager_->add_system<SystemType>(std::forward<Args>(args)...);
            }

            // 按类型获取系统
            template <typename SystemType>
            SystemType* get_system()
            {
                return system_manager_->get_system<SystemType>();
            }

            // 按名称获取系统
            ISystem* get_system(const std::string& name)
            {
                return system_manager_->get_system(name);
            }

            // 配置所有系统
            void configure_systems() { system_manager_->configure_all(this); }

            // 初始化所有系统
            void initialize_systems() { system_manager_->initialize_all(); }

            // 更新所有系统
            void update_systems(float delta_time) { system_manager_->update_all(delta_time); }

            // 关闭所有系统
            void shutdown_systems() { system_manager_->shutdown_all(); }

            // 清理所有系统
            void cleanup_systems() { system_manager_->cleanup_all(); }

            // 执行指定阶段的系统
            void execute_phase(SystemPhase phase, float delta_time = 0.0f)
            {
                system_manager_->execute_phase(this, phase, delta_time);
            }

            // ==================== 实体和组件管理接口 ====================

            // 实体管理
            Entity create_entity();
            void destroy_entity(Entity entity);
            bool is_valid(Entity entity) const;

            // 组件管理
            template <typename ComponentType>
            void register_component()
            {
                registry_.register_component(std::type_index(typeid(ComponentType)), sizeof(ComponentType));
            }

            template <typename ComponentType>
            void add_component(Entity entity, ComponentType&& component)
            {
                registry_.add_component(entity, std::type_index(typeid(ComponentType)), &component);
            }

            template <typename ComponentType>
            void remove_component(Entity entity)
            {
                registry_.remove_component(entity, std::type_index(typeid(ComponentType)));
            }

            template <typename ComponentType>
            bool has_component(Entity entity) const
            {
                return registry_.has_component(entity, std::type_index(typeid(ComponentType)));
            }

            template <typename ComponentType>
            ComponentType* get_component(Entity entity)
            {
                return static_cast<ComponentType*>(registry_.get_component(entity, std::type_index(typeid(ComponentType))));
            }

            template <typename ComponentType>
            const ComponentType* get_component(Entity entity) const
            {
                return static_cast<const ComponentType*>(registry_.get_component(entity, std::type_index(typeid(ComponentType))));
            }

            // 视图创建（模板方法，但实现隐藏在.cpp中）
            template <typename... ComponentTypes>
            Registry::EntityView create_view()
            {
                return registry_.create_view({std::type_index(typeid(ComponentTypes))...});
            }

            // 事件系统
            template <typename EventType>
            void subscribe(std::function<void(const EventType&)> callback)
            {
                registry_.subscribe(std::type_index(typeid(EventType)),
                    [callback](const void* event_data) {
                        callback(*static_cast<const EventType*>(event_data));
                    });
            }

            template <typename EventType>
            void publish(const EventType& event)
            {
                registry_.publish(std::type_index(typeid(EventType)), &event);
            }

            [[nodiscard]] Threading::ThreadPoolAdapter& get_thread_pool() const { return *thread_pool_; }

        private:
            Application* application_ = nullptr;
            Registry registry_;
            std::unique_ptr<Threading::ThreadPoolAdapter> thread_pool_;
            std::unique_ptr<SystemManager> system_manager_;
        };

        // ==================== System<Derived> 模板方法实现 ====================
        // 这些方法必须在 World 类定义之后实现，因为它们需要访问 World 的完整定义

        template <typename Derived>
        template <typename... ComponentTypes>
        void System<Derived>::each(std::function<void(Entity, ComponentTypes*...)> func)
        {
            auto view = world_->create_view<ComponentTypes...>();
            view.each([this, func](Entity entity) {
                func(entity, world_->get_component<ComponentTypes>(entity)...);
            });
        }

        template <typename Derived>
        template <typename... ComponentTypes>
        void System<Derived>::parallel_each(std::function<void(Entity, ComponentTypes*...)> func, size_t chunk_size)
        {
            auto view = world_->create_view<ComponentTypes...>();
            const size_t entity_count = view.size();

            if (entity_count == 0) return;

            // 收集实体到临时容器（避免视图遍历时的竞争）
            std::vector<Entity> entities;
            entities.reserve(entity_count);
            view.each([&entities](Entity e) {
                entities.push_back(e);
            });

            // 并行处理
            ParallelFor::each(entities,
                [this, func](Entity entity, size_t) {
                    func(entity, world_->get_component<ComponentTypes>(entity)...);
                },
                world_->get_thread_pool(),
                chunk_size
            );
        }

        template <typename Derived>
        template <typename... ComponentTypes>
        void System<Derived>::parallel_for_range(std::function<void(Entity, size_t, ComponentTypes*...)> func,
                                                      size_t chunk_size)
        {
            auto view = world_->create_view<ComponentTypes...>();
            const size_t entity_count = view.size();

            if (entity_count == 0) return;

            std::vector<Entity> entities;
            entities.reserve(entity_count);
            view.each([&entities](Entity e) {
                entities.push_back(e);
            });

            // 使用 ParallelFor::range 进行更细粒度的控制
            ParallelFor::range(0, entity_count,
                [this, &entities, &func](size_t begin, size_t end) {
                    for (size_t i = begin; i < end; ++i)
                    {
                        func(entities[i], i, world_->get_component<ComponentTypes>(entities[i])...);
                    }
                },
                world_->get_thread_pool(),
                chunk_size
            );
        }

        template <typename Derived>
        template <typename ComponentType>
        ComponentType* System<Derived>::get_component(Entity entity)
        {
            return world_->get_component<ComponentType>(entity);
        }

        template <typename Derived>
        template <typename ComponentType>
        const ComponentType* System<Derived>::get_component(Entity entity) const
        {
            return world_->get_component<ComponentType>(entity);
        }

        template <typename Derived>
        template <typename ComponentType>
        void System<Derived>::add_component(Entity entity, ComponentType&& component)
        {
            world_->add_component<ComponentType>(entity, std::forward<ComponentType>(component));
        }

        template <typename Derived>
        template <typename ComponentType>
        void System<Derived>::remove_component(Entity entity)
        {
            world_->remove_component<ComponentType>(entity);
        }

        template <typename Derived>
        template <typename ComponentType>
        bool System<Derived>::has_component(Entity entity) const
        {
            return world_->has_component<ComponentType>(entity);
        }

    } // namespace Ecs
END_NAMESPACE_COMMON
