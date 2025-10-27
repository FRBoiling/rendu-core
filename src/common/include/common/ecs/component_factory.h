#pragma once

#include "common/ecs/registry.h"
#include <typeindex>

BEGIN_NAMESPACE_COMMON
    namespace Ecs
    {
        // 组件工厂模板类，用于简化组件注册和管理
        template <typename ComponentType>
        class ComponentFactory {
        public:
            // 注册组件类型到Registry
            static void register_component(Registry& registry) {
                registry.register_component(std::type_index(typeid(ComponentType)), sizeof(ComponentType));
            }
            
            // 添加组件到实体
            static void add_component(Registry& registry, Entity entity, ComponentType&& component) {
                registry.add_component(entity, std::type_index(typeid(ComponentType)), &component);
            }
            
            // 从实体中移除组件
            static void remove_component(Registry& registry, Entity entity) {
                registry.remove_component(entity, std::type_index(typeid(ComponentType)));
            }
            
            // 检查实体是否有该组件
            static bool has_component(const Registry& registry, Entity entity) {
                return registry.has_component(entity, std::type_index(typeid(ComponentType)));
            }
            
            // 获取组件
            static ComponentType* get_component(Registry& registry, Entity entity) {
                return static_cast<ComponentType*>(registry.get_component(entity, std::type_index(typeid(ComponentType))));
            }
            
            // 获取组件（const版本）
            static const ComponentType* get_component(const Registry& registry, Entity entity) {
                return static_cast<const ComponentType*>(registry.get_component(entity, std::type_index(typeid(ComponentType))));
            }
        };
    }
END_NAMESPACE_COMMON