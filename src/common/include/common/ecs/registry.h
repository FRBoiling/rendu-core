#pragma once

#include "common/define.h"
#include <entt/entt.hpp>
#include <asio/asio.hpp>

BEGIN_NAMESPACE_COMMON
    namespace Ecs
    {
        // 注册表封装，基于entt::registry
        class Registry
        {
        public:
            using Entity = entt::entity;
            using RegistryType = entt::registry;

            // 在 Registry 构造函数中添加
            Registry() {
            }
            ~Registry() = default;

            // 创建实体
            Entity create();

            // 销毁实体
            void destroy(Entity entity);

            // 检查实体是否有效
            bool valid(Entity entity) const;

            // 组件管理方法
            template <typename Component, typename... Args>
            Component& emplace(Entity entity, Args&&... args);

            template <typename Component>
            void remove(Entity entity);

            template <typename Component>
            bool has(Entity entity) const;

            template <typename Component>
            Component* try_get(Entity entity);

            template <typename Component>
            const Component* try_get(Entity entity) const;

            template <typename Component>
            Component& get(Entity entity);

            template <typename Component>
            const Component& get(Entity entity) const;

            // 视图和组
            template <typename... Components>
            auto view();

            template <typename... Components>
            auto view() const;

            template <typename... Components, typename... Exclude>
            auto view(entt::exclude_t<Exclude...> tag);

            template <typename... Components, typename... Exclude>
            auto view(entt::exclude_t<Exclude...> tag) const;

            // 事件管理
            template <typename Event, typename... Args>
            void trigger(Args&&... args);

            template <typename Event>
            entt::sink<Event> sink();

            // 获取底层注册表
            RegistryType& get_registry() { return registry_; }
            const RegistryType& get_registry() const { return registry_; }

        private:
            RegistryType registry_;
        };

        // 模板方法实现

        inline Registry::Entity Registry::create()
        {
            return registry_.create();
        }

        inline void Registry::destroy(Entity entity)
        {
            registry_.destroy(entity);
        }

        inline bool Registry::valid(Entity entity) const
        {
            return registry_.valid(entity);
        }

        template <typename Component, typename... Args>
        inline Component& Registry::emplace(Entity entity, Args&&... args)
        {
            return registry_.emplace<Component>(entity, std::forward<Args>(args)...);
        }

        template <typename Component>
        inline void Registry::remove(Entity entity)
        {
            registry_.remove<Component>(entity);
        }

        template <typename Component>
        inline bool Registry::has(Entity entity) const
        {
            return registry_.all_of<Component>(entity);
        }

        template <typename Component>
        inline Component* Registry::try_get(Entity entity)
        {
            return registry_.try_get<Component>(entity);
        }

        template <typename Component>
        inline const Component* Registry::try_get(Entity entity) const
        {
            return registry_.try_get<Component>(entity);
        }

        template <typename Component>
        inline Component& Registry::get(Entity entity)
        {
            return registry_.get<Component>(entity);
        }

        template <typename Component>
        inline const Component& Registry::get(Entity entity) const
        {
            return registry_.get<Component>(entity);
        }

        template <typename... Components>
        inline auto Registry::view()
        {
            return registry_.view<Components...>();
        }

        template <typename... Components>
        inline auto Registry::view() const
        {
            return registry_.view<Components...>();
        }

        template <typename... Components, typename... Exclude>
        inline auto Registry::view(entt::exclude_t<Exclude...> tag)
        {
            return registry_.view<Components...>(tag);
        }

        template <typename... Components, typename... Exclude>
        inline auto Registry::view(entt::exclude_t<Exclude...> tag) const
        {
            return registry_.view<Components...>(tag);
        }

    } // namespace Ecs

END_NAMESPACE_COMMON