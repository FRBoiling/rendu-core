#pragma once

#include "common/define.h"
#include "common/ecs/registry.h"
#include <entt/entt.hpp>
#include <functional>
#include <iterator>

BEGIN_NAMESPACE_COMMON
namespace ecs {

template<typename... Components>
class View {
public:
    using entt_view = entt::view<entt::get_t<Components...>>;

    explicit View(entt::registry& registry)
        : view_(registry.view<Components...>()) {}

    template<typename... Exclude>
    explicit View(entt::registry& registry, entt::exclude_t<Exclude...> exclude)
        : view_(registry.view<Components...>(exclude)) {}

    // 遍历所有匹配的实体
    template<typename Func>
    void for_each(Func func) {
        view_.each(func);
    }

    // 遍历并返回实体句柄
    template<typename Func>
    void for_each_entity(Func func) {
        for (auto entity : view_) {
            func(entity);
        }
    }

    // 获取匹配的实体数量
    size_t size() const {
        return std::distance(view_.begin(), view_.end());
    }

    // 检查是否为空
    bool empty() const {
        return view_.empty();
    }

    // 包含实体
    bool contains(Entity entity) const {
        return view_.contains(entity);
    }

    // 获取组件
    template<typename Component>
    decltype(auto) get(Entity entity) {
        return view_.template get<Component>(entity);
    }

    // 访问底层视图
    entt_view& native() { return view_; }
    const entt_view& native() const { return view_; }

private:
    entt_view view_;
};

} // namespace ecs
END_NAMESPACE_COMMON
