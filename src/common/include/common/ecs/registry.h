#pragma once

#include "common/define.h"
#include <entt/entt.hpp>
#include <vector>

BEGIN_NAMESPACE_COMMON
namespace ecs {

using Entity = entt::entity;

class Registry {
public:
    Registry() = default;
    ~Registry() = default;

    // 创建实体
    Entity create() {
        return registry_.create();
    }

    void destroy(Entity entity) {
        if (valid(entity)) {
            registry_.destroy(entity);
        }
    }

    // 检查实体是否有效
    bool valid(Entity entity) const {
        return registry_.valid(entity);
    }

    // 组件操作
    template<typename Component, typename... Args>
    Component& emplace(Entity entity, Args&&... args) {
        return registry_.emplace<Component>(entity, std::forward<Args>(args)...);
    }

    template<typename Component>
    Component& get(Entity entity) {
        return registry_.get<Component>(entity);
    }

    template<typename Component>
    const Component& get(Entity entity) const {
        return registry_.get<Component>(entity);
    }

    template<typename Component>
    bool has(Entity entity) const {
        return registry_.all_of<Component>(entity);
    }

    template<typename Component>
    void remove(Entity entity) {
        if (has<Component>(entity)) {
            registry_.remove<Component>(entity);
        }
    }

    // 获取实体数量（使用视图统计）
    size_t size() const {
        auto view = registry_.view<entt::entity>();
        return view.size();
    }

    // 清空所有实体
    void clear() {
        registry_.clear();
    }

    // 访问底层 registry
    entt::registry& native() { return registry_; }
    const entt::registry& native() const { return registry_; }

private:
    entt::registry registry_;
};

} // namespace ecs
END_NAMESPACE_COMMON
