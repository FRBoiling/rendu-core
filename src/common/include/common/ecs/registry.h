#pragma once

#include <functional>

#include "common/define.h"
#include <memory>
#include <string>
#include <typeindex>

BEGIN_NAMESPACE_COMMON
    namespace Ecs
    {
        // 前置声明
        class RegistryImpl;
        
        // Entity类型定义
        using Entity = uint32_t;
        
        // Registry类 - 完全基于接口的非模板API
        class Registry
        {
        public:
            Registry();
            ~Registry();
            
            // 禁止拷贝和移动
            Registry(const Registry&) = delete;
            Registry& operator=(const Registry&) = delete;
            Registry(Registry&&) = delete;
            Registry& operator=(Registry&&) = delete;
            
            // 实体管理
            Entity create();
            void destroy(Entity entity);
            bool valid(Entity entity) const;
            
            // 组件管理（基于接口）
            void register_component(const std::type_index& type, size_t size);
            void add_component(Entity entity, const std::type_index& type, void* component);
            void remove_component(Entity entity, const std::type_index& type);
            bool has_component(Entity entity, const std::type_index& type) const;
            void* get_component(Entity entity, const std::type_index& type);
            const void* get_component(Entity entity, const std::type_index& type) const;
            
            // 实体组查询
            class EntityView {
            public:
                EntityView(void* impl);
                ~EntityView();
                
                // 遍历实体
                void each(std::function<void(Entity)> callback);
                
                // 检查是否为空
                bool empty() const;
                
                // 获取实体数量
                size_t size() const;
                
            private:
                void* impl_;
            };
            
            // 创建实体视图（查询包含特定组件的实体）
            EntityView create_view(const std::vector<std::type_index>& component_types);
            
            // 事件系统接口
            void subscribe(const std::type_index& event_type, std::function<void(const void*)> callback);
            void unsubscribe(const std::type_index& event_type, std::function<void(const void*)> callback);
            void publish(const std::type_index& event_type, const void* event_data);
            
        private:
            std::unique_ptr<RegistryImpl> impl_;  // PIMPL模式隐藏实现细节
        };
    }
END_NAMESPACE_COMMON