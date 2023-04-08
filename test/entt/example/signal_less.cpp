//
// Created by boil on 2023/2/14.
//

#include <iterator>
#include <type_traits>
#include <gtest/gtest.h>
#include <entt/entt.hpp>

template<typename Type, typename Entity>
struct entt::storage_type<Type, Entity> {
    // no signal regardless of component type ...
    using type = basic_storage<Type, Entity>;
};

template<typename, typename, typename = void>
struct has_on_construct: std::false_type {};

template<typename Entity, typename Type>
struct has_on_construct<Entity, Type, std::void_t<decltype(&entt::storage_type_t<Type>::on_construct)>>: std::true_type {};

template<typename Entity, typename Type>
inline constexpr auto has_on_construct_v = has_on_construct<Entity, Type>::value;

TEST(Example, SignalLess) {
    // invoking registry::on_construct<int> is a compile-time error
    static_assert(!has_on_construct_v<entt::entity, int>);

    entt::registry registry;
    const entt::entity entity[1u]{registry.create()};

    // literally a test for storage_adapter_mixin
    registry.emplace<int>(entity[0], 0);
    registry.erase<int>(entity[0]);
    registry.insert<int>(std::begin(entity), std::end(entity), 3);
    registry.patch<int>(entity[0], [](auto &value) { value = 42; });

    ASSERT_EQ(registry.get<int>(entity[0]), 42);
}
