/*
* Created by boil on 2023/2/19.
*/

#include <test/rdtest.h>
#include <test/throwing_allocator.h>
#include <test/throwing_type.h>
#include <core/ecs/registry.h>
#include <core/ecs/storage.h>
#include <core/ecs/mixin.h>

struct empty_type {};

struct stable_type {
  static constexpr auto in_place_delete = true;
  int value{};
};

struct non_default_constructible {
  non_default_constructible() = delete;

  non_default_constructible(int v)
      : value{v} {}

  int value{};
};

struct counter {
  int value{};
};

template<typename Registry>
void listener(counter &counter, Registry &, typename Registry::entity_type) {
  ++counter.value;
}


RD_TEST(SighMixin, GenericType) {
  rendu::entity entities[2u]{rendu::entity{3}, rendu::entity{42}};
  rendu::sigh_mixin<rendu::storage<int>> pool;
  rendu::sparse_set &base = pool;
  rendu::registry registry;

  counter on_construct{};
  counter on_destroy{};

  pool.bind(rendu::forward_as_any(registry));

  RD_ASSERT_TRUE(pool.empty());

  pool.insert(entities, entities + 1u);
  pool.erase(entities[0u]);

  RD_ASSERT_TRUE(pool.empty());

  RD_ASSERT_EQ(on_construct.value, 0);
  RD_ASSERT_EQ(on_destroy.value, 0);

  pool.on_construct().connect<&listener<rendu::registry>>(on_construct);
  pool.on_destroy().connect<&listener<rendu::registry>>(on_destroy);

  RD_ASSERT_NE(base.push(entities[0u]), base.end());

  pool.emplace(entities[1u]);

  RD_ASSERT_EQ(on_construct.value, 2);
  RD_ASSERT_EQ(on_destroy.value, 0);
  RD_ASSERT_FALSE(pool.empty());

  RD_ASSERT_EQ(pool.get(entities[0u]), 0);
  RD_ASSERT_EQ(pool.get(entities[1u]), 0);

  base.erase(entities[0u]);
  pool.erase(entities[1u]);

  RD_ASSERT_EQ(on_construct.value, 2);
  RD_ASSERT_EQ(on_destroy.value, 2);
  RD_ASSERT_TRUE(pool.empty());

  RD_ASSERT_NE(base.push(std::begin(entities), std::end(entities)), base.end());

  RD_ASSERT_EQ(pool.get(entities[0u]), 0);
  RD_ASSERT_EQ(pool.get(entities[1u]), 0);
  RD_ASSERT_FALSE(pool.empty());

  base.erase(entities[1u]);

  RD_ASSERT_EQ(on_construct.value, 4);
  RD_ASSERT_EQ(on_destroy.value, 3);
  RD_ASSERT_FALSE(pool.empty());

  base.erase(entities[0u]);

  RD_ASSERT_EQ(on_construct.value, 4);
  RD_ASSERT_EQ(on_destroy.value, 4);
  RD_ASSERT_TRUE(pool.empty());

  pool.insert(std::begin(entities), std::end(entities), 3);

  RD_ASSERT_EQ(on_construct.value, 6);
  RD_ASSERT_EQ(on_destroy.value, 4);
  RD_ASSERT_FALSE(pool.empty());

  RD_ASSERT_EQ(pool.get(entities[0u]), 3);
  RD_ASSERT_EQ(pool.get(entities[1u]), 3);

  pool.erase(std::begin(entities), std::end(entities));

  RD_ASSERT_EQ(on_construct.value, 6);
  RD_ASSERT_EQ(on_destroy.value, 6);
  RD_ASSERT_TRUE(pool.empty());
}

RD_TEST(SighMixin, StableType) {
  rendu::entity entities[2u]{rendu::entity{3}, rendu::entity{42}};
  rendu::sigh_mixin<rendu::storage<stable_type>> pool;
  rendu::sparse_set &base = pool;
  rendu::registry registry;

  counter on_construct{};
  counter on_destroy{};

  pool.bind(rendu::forward_as_any(registry));
  pool.on_construct().connect<&listener<rendu::registry>>(on_construct);
  pool.on_destroy().connect<&listener<rendu::registry>>(on_destroy);

  RD_ASSERT_NE(base.push(entities[0u]), base.end());

  pool.emplace(entities[1u]);

  RD_ASSERT_EQ(on_construct.value, 2);
  RD_ASSERT_EQ(on_destroy.value, 0);
  RD_ASSERT_FALSE(pool.empty());

  RD_ASSERT_EQ(pool.get(entities[0u]).value, 0);
  RD_ASSERT_EQ(pool.get(entities[1u]).value, 0);

  base.erase(entities[0u]);
  pool.erase(entities[1u]);

  RD_ASSERT_EQ(on_construct.value, 2);
  RD_ASSERT_EQ(on_destroy.value, 2);
  RD_ASSERT_FALSE(pool.empty());

  RD_ASSERT_NE(base.push(std::begin(entities), std::end(entities)), base.end());

  RD_ASSERT_EQ(pool.get(entities[0u]).value, 0);
  RD_ASSERT_EQ(pool.get(entities[1u]).value, 0);
  RD_ASSERT_FALSE(pool.empty());

  base.erase(entities[1u]);

  RD_ASSERT_EQ(on_construct.value, 4);
  RD_ASSERT_EQ(on_destroy.value, 3);
  RD_ASSERT_FALSE(pool.empty());

  base.erase(entities[0u]);

  RD_ASSERT_EQ(on_construct.value, 4);
  RD_ASSERT_EQ(on_destroy.value, 4);
  RD_ASSERT_FALSE(pool.empty());

  pool.insert(std::begin(entities), std::end(entities), stable_type{3});

  RD_ASSERT_EQ(on_construct.value, 6);
  RD_ASSERT_EQ(on_destroy.value, 4);
  RD_ASSERT_FALSE(pool.empty());

  RD_ASSERT_EQ(pool.get(entities[0u]).value, 3);
  RD_ASSERT_EQ(pool.get(entities[1u]).value, 3);

  pool.erase(std::begin(entities), std::end(entities));

  RD_ASSERT_EQ(on_construct.value, 6);
  RD_ASSERT_EQ(on_destroy.value, 6);
  RD_ASSERT_FALSE(pool.empty());
}

RD_TEST(SighMixin, EmptyType) {
  rendu::entity entities[2u]{rendu::entity{3}, rendu::entity{42}};
  rendu::sigh_mixin<rendu::storage<empty_type>> pool;
  rendu::sparse_set &base = pool;
  rendu::registry registry;

  counter on_construct{};
  counter on_destroy{};

  pool.bind(rendu::forward_as_any(registry));
  pool.on_construct().connect<&listener<rendu::registry>>(on_construct);
  pool.on_destroy().connect<&listener<rendu::registry>>(on_destroy);

  RD_ASSERT_NE(base.push(entities[0u]), base.end());

  pool.emplace(entities[1u]);

  RD_ASSERT_EQ(on_construct.value, 2);
  RD_ASSERT_EQ(on_destroy.value, 0);
  RD_ASSERT_FALSE(pool.empty());

  RD_ASSERT_TRUE(pool.contains(entities[0u]));
  RD_ASSERT_TRUE(pool.contains(entities[1u]));

  base.erase(entities[0u]);
  pool.erase(entities[1u]);

  RD_ASSERT_EQ(on_construct.value, 2);
  RD_ASSERT_EQ(on_destroy.value, 2);
  RD_ASSERT_TRUE(pool.empty());

  RD_ASSERT_NE(base.push(std::begin(entities), std::end(entities)), base.end());

  RD_ASSERT_TRUE(pool.contains(entities[0u]));
  RD_ASSERT_TRUE(pool.contains(entities[1u]));
  RD_ASSERT_FALSE(pool.empty());

  base.erase(entities[1u]);

  RD_ASSERT_EQ(on_construct.value, 4);
  RD_ASSERT_EQ(on_destroy.value, 3);
  RD_ASSERT_FALSE(pool.empty());

  base.erase(entities[0u]);

  RD_ASSERT_EQ(on_construct.value, 4);
  RD_ASSERT_EQ(on_destroy.value, 4);
  RD_ASSERT_TRUE(pool.empty());

  pool.insert(std::begin(entities), std::end(entities));

  RD_ASSERT_EQ(on_construct.value, 6);
  RD_ASSERT_EQ(on_destroy.value, 4);
  RD_ASSERT_FALSE(pool.empty());

  RD_ASSERT_TRUE(pool.contains(entities[0u]));
  RD_ASSERT_TRUE(pool.contains(entities[1u]));

  pool.erase(std::begin(entities), std::end(entities));

  RD_ASSERT_EQ(on_construct.value, 6);
  RD_ASSERT_EQ(on_destroy.value, 6);
  RD_ASSERT_TRUE(pool.empty());
}

RD_TEST(SighMixin, NonDefaultConstructibleType) {
  rendu::entity entities[2u]{rendu::entity{3}, rendu::entity{42}};
  rendu::sigh_mixin<rendu::storage<non_default_constructible>> pool;
  rendu::sparse_set &base = pool;
  rendu::registry registry;

  counter on_construct{};
  counter on_destroy{};

  pool.bind(rendu::forward_as_any(registry));
  pool.on_construct().connect<&listener<rendu::registry>>(on_construct);
  pool.on_destroy().connect<&listener<rendu::registry>>(on_destroy);

  RD_ASSERT_EQ(base.push(entities[0u]), base.end());

  pool.emplace(entities[1u], 3);

  RD_ASSERT_EQ(pool.size(), 1u);
  RD_ASSERT_EQ(on_construct.value, 1);
  RD_ASSERT_EQ(on_destroy.value, 0);
  RD_ASSERT_FALSE(pool.empty());

  RD_ASSERT_FALSE(pool.contains(entities[0u]));
  RD_ASSERT_EQ(pool.get(entities[1u]).value, 3);

  base.erase(entities[1u]);

  RD_ASSERT_EQ(pool.size(), 0u);
  RD_ASSERT_EQ(on_construct.value, 1);
  RD_ASSERT_EQ(on_destroy.value, 1);
  RD_ASSERT_TRUE(pool.empty());

  RD_ASSERT_EQ(base.push(std::begin(entities), std::end(entities)), base.end());

  RD_ASSERT_FALSE(pool.contains(entities[0u]));
  RD_ASSERT_FALSE(pool.contains(entities[1u]));
  RD_ASSERT_TRUE(pool.empty());

  pool.insert(std::begin(entities), std::end(entities), 3);

  RD_ASSERT_EQ(pool.size(), 2u);
  RD_ASSERT_EQ(on_construct.value, 3);
  RD_ASSERT_EQ(on_destroy.value, 1);
  RD_ASSERT_FALSE(pool.empty());

  RD_ASSERT_EQ(pool.get(entities[0u]).value, 3);
  RD_ASSERT_EQ(pool.get(entities[1u]).value, 3);

  pool.erase(std::begin(entities), std::end(entities));

  RD_ASSERT_EQ(pool.size(), 0u);
  RD_ASSERT_EQ(on_construct.value, 3);
  RD_ASSERT_EQ(on_destroy.value, 3);
  RD_ASSERT_TRUE(pool.empty());
}

RD_TEST(SighMixin, VoidType) {
  rendu::sigh_mixin<rendu::storage<void>> pool;
  rendu::registry registry;

  counter on_construct{};
  counter on_destroy{};

  pool.bind(rendu::forward_as_any(registry));
  pool.on_construct().connect<&listener<rendu::registry>>(on_construct);
  pool.on_destroy().connect<&listener<rendu::registry>>(on_destroy);

  pool.emplace(rendu::entity{99});

  RD_ASSERT_EQ(pool.type(), rendu::type_id<void>());
  RD_ASSERT_TRUE(pool.contains(rendu::entity{99}));

  rendu::sigh_mixin<rendu::storage<void>> other{std::move(pool)};

  RD_ASSERT_FALSE(pool.contains(rendu::entity{99}));
  RD_ASSERT_TRUE(other.contains(rendu::entity{99}));

  pool = std::move(other);

  RD_ASSERT_TRUE(pool.contains(rendu::entity{99}));
  RD_ASSERT_FALSE(other.contains(rendu::entity{99}));

  pool.clear();

  RD_ASSERT_EQ(on_construct.value, 1);
  RD_ASSERT_EQ(on_destroy.value, 1);
}

RD_TEST(SighMixin, Move) {
  rendu::sigh_mixin<rendu::storage<int>> pool;
  rendu::registry registry;

  counter on_construct{};
  counter on_destroy{};

  pool.bind(rendu::forward_as_any(registry));
  pool.on_construct().connect<&listener<rendu::registry>>(on_construct);
  pool.on_destroy().connect<&listener<rendu::registry>>(on_destroy);

  pool.emplace(rendu::entity{3}, 3);

  RD_ASSERT_TRUE(std::is_move_constructible_v<decltype(pool)>);
  RD_ASSERT_TRUE(std::is_move_assignable_v<decltype(pool)>);
  RD_ASSERT_EQ(pool.type(), rendu::type_id<int>());

  rendu::sigh_mixin<rendu::storage<int>> other{std::move(pool)};

  RD_ASSERT_TRUE(pool.empty());
  RD_ASSERT_FALSE(other.empty());
  RD_ASSERT_EQ(other.type(), rendu::type_id<int>());
  RD_ASSERT_EQ(pool.at(0u), static_cast<rendu::entity>(rendu::null));
  RD_ASSERT_EQ(other.at(0u), rendu::entity{3});
  RD_ASSERT_EQ(other.get(rendu::entity{3}), 3);

  pool = std::move(other);

  RD_ASSERT_FALSE(pool.empty());
  RD_ASSERT_TRUE(other.empty());
  RD_ASSERT_EQ(pool.at(0u), rendu::entity{3});
  RD_ASSERT_EQ(pool.get(rendu::entity{3}), 3);
  RD_ASSERT_EQ(other.at(0u), static_cast<rendu::entity>(rendu::null));

  other = rendu::sigh_mixin<rendu::storage<int>>{};
  other.bind(rendu::forward_as_any(registry));

  other.emplace(rendu::entity{42}, 42);
  other = std::move(pool);

  RD_ASSERT_TRUE(pool.empty());
  RD_ASSERT_FALSE(other.empty());
  RD_ASSERT_EQ(pool.at(0u), static_cast<rendu::entity>(rendu::null));
  RD_ASSERT_EQ(other.at(0u), rendu::entity{3});
  RD_ASSERT_EQ(other.get(rendu::entity{3}), 3);

  other.clear();

  RD_ASSERT_EQ(on_construct.value, 1);
  RD_ASSERT_EQ(on_destroy.value, 1);
}

RD_TEST(SighMixin, Swap) {
  rendu::sigh_mixin<rendu::storage<int>> pool;
  rendu::sigh_mixin<rendu::storage<int>> other;
  rendu::registry registry;

  counter on_construct{};
  counter on_destroy{};

  pool.bind(rendu::forward_as_any(registry));
  pool.on_construct().connect<&listener<rendu::registry>>(on_construct);
  pool.on_destroy().connect<&listener<rendu::registry>>(on_destroy);

  other.bind(rendu::forward_as_any(registry));
  other.on_construct().connect<&listener<rendu::registry>>(on_construct);
  other.on_destroy().connect<&listener<rendu::registry>>(on_destroy);

  pool.emplace(rendu::entity{42}, 41);

  other.emplace(rendu::entity{9}, 8);
  other.emplace(rendu::entity{3}, 2);
  other.erase(rendu::entity{9});

  RD_ASSERT_EQ(pool.size(), 1u);
  RD_ASSERT_EQ(other.size(), 1u);

  pool.swap(other);

  RD_ASSERT_EQ(pool.type(), rendu::type_id<int>());
  RD_ASSERT_EQ(other.type(), rendu::type_id<int>());

  RD_ASSERT_EQ(pool.size(), 1u);
  RD_ASSERT_EQ(other.size(), 1u);

  RD_ASSERT_EQ(pool.at(0u), rendu::entity{3});
  RD_ASSERT_EQ(pool.get(rendu::entity{3}), 2);

  RD_ASSERT_EQ(other.at(0u), rendu::entity{42});
  RD_ASSERT_EQ(other.get(rendu::entity{42}), 41);

  pool.clear();
  other.clear();

  RD_ASSERT_EQ(on_construct.value, 3);
  RD_ASSERT_EQ(on_destroy.value, 3);
}


RD_TEST(SighMixin, StorageEntity) {
  using traits_type = rendu::entity_traits<rendu::entity>;

  rendu::sigh_mixin<rendu::storage<rendu::entity>> pool;
  rendu::registry registry;

  counter on_construct{};
  counter on_destroy{};

  pool.bind(rendu::forward_as_any(registry));
  pool.on_construct().connect<&listener<rendu::registry>>(on_construct);
  pool.on_destroy().connect<&listener<rendu::registry>>(on_destroy);

  pool.push(rendu::entity{1});

  RD_ASSERT_EQ(on_construct.value, 1);
  RD_ASSERT_EQ(on_destroy.value, 0);
  RD_ASSERT_EQ(pool.size(), 2u);
  RD_ASSERT_EQ(pool.in_use(), 1u);

  pool.erase(rendu::entity{1});

  RD_ASSERT_EQ(on_construct.value, 1);
  RD_ASSERT_EQ(on_destroy.value, 1);
  RD_ASSERT_EQ(pool.size(), 2u);
  RD_ASSERT_EQ(pool.in_use(), 0u);

  pool.push(traits_type::construct(0, 2));
  pool.push(traits_type::construct(2, 1));

  RD_ASSERT_TRUE(pool.contains(traits_type::construct(0, 2)));
  RD_ASSERT_TRUE(pool.contains(traits_type::construct(1, 1)));
  RD_ASSERT_TRUE(pool.contains(traits_type::construct(2, 1)));

  RD_ASSERT_EQ(on_construct.value, 3);
  RD_ASSERT_EQ(on_destroy.value, 1);
  RD_ASSERT_EQ(pool.size(), 3u);
  RD_ASSERT_EQ(pool.in_use(), 2u);

  pool.clear();

  RD_ASSERT_EQ(pool.size(), 0u);
  RD_ASSERT_EQ(pool.in_use(), 0u);

  RD_ASSERT_EQ(on_construct.value, 3);
  RD_ASSERT_EQ(on_destroy.value, 3);

  pool.spawn();
  pool.spawn(rendu::entity{0});

  rendu::entity entities[1u]{};
  pool.spawn(entities, entities + 1u);

  RD_ASSERT_EQ(on_construct.value, 6);
  RD_ASSERT_EQ(on_destroy.value, 3);
  RD_ASSERT_EQ(pool.size(), 3u);
  RD_ASSERT_EQ(pool.in_use(), 3u);

  pool.clear();

  RD_ASSERT_EQ(pool.size(), 0u);
  RD_ASSERT_EQ(pool.in_use(), 0u);
}

RD_TEST(SighMixin, CustomAllocator) {
  auto test = [](auto pool, auto alloc) {
    using registry_type = typename decltype(pool)::registry_type;
    registry_type registry;

    counter on_construct{};
    counter on_destroy{};

    pool.bind(rendu::forward_as_any(registry));
    pool.on_construct().template connect<&listener<registry_type>>(on_construct);
    pool.on_destroy().template connect<&listener<registry_type>>(on_destroy);

    pool.reserve(1u);

    RD_ASSERT_NE(pool.capacity(), 0u);

    pool.emplace(rendu::entity{0});
    pool.emplace(rendu::entity{1});

    decltype(pool) other{std::move(pool), alloc};

    RD_ASSERT_TRUE(pool.empty());
    RD_ASSERT_FALSE(other.empty());
    RD_ASSERT_EQ(pool.capacity(), 0u);
    RD_ASSERT_NE(other.capacity(), 0u);
    RD_ASSERT_EQ(other.size(), 2u);

    pool = std::move(other);

    RD_ASSERT_FALSE(pool.empty());
    RD_ASSERT_TRUE(other.empty());
    RD_ASSERT_EQ(other.capacity(), 0u);
    RD_ASSERT_NE(pool.capacity(), 0u);
    RD_ASSERT_EQ(pool.size(), 2u);

    pool.swap(other);
    pool = std::move(other);

    RD_ASSERT_FALSE(pool.empty());
    RD_ASSERT_TRUE(other.empty());
    RD_ASSERT_EQ(other.capacity(), 0u);
    RD_ASSERT_NE(pool.capacity(), 0u);
    RD_ASSERT_EQ(pool.size(), 2u);

    pool.clear();

    RD_ASSERT_NE(pool.capacity(), 0u);
    RD_ASSERT_EQ(pool.size(), 0u);

    RD_ASSERT_EQ(on_construct.value, 2);
    RD_ASSERT_EQ(on_destroy.value, 2);
  };

  test::throwing_allocator<rendu::entity> allocator{};

  test(rendu::sigh_mixin<rendu::basic_storage<int, rendu::entity, test::throwing_allocator<int>>>{allocator}, allocator);
  test(rendu::sigh_mixin<rendu::basic_storage<std::true_type, rendu::entity, test::throwing_allocator<std::true_type>>>{allocator}, allocator);
  test(rendu::sigh_mixin<rendu::basic_storage<stable_type, rendu::entity, test::throwing_allocator<stable_type>>>{allocator}, allocator);
}

RD_TEST(SighMixin, ThrowingAllocator) {
  auto test = [](auto pool) {
    using pool_allocator_type = typename decltype(pool)::allocator_type;
    using value_type = typename decltype(pool)::value_type;
    using registry_type = typename decltype(pool)::registry_type;

    typename std::decay_t<decltype(pool)>::base_type &base = pool;
    constexpr auto packed_page_size = rendu::component_traits<typename decltype(pool)::value_type>::page_size;
    constexpr auto sparse_page_size = rendu::entity_traits<typename decltype(pool)::entity_type>::page_size;
    registry_type registry;

    counter on_construct{};
    counter on_destroy{};

    pool.bind(rendu::forward_as_any(registry));
    pool.on_construct().template connect<&listener<registry_type>>(on_construct);
    pool.on_destroy().template connect<&listener<registry_type>>(on_destroy);

    pool_allocator_type::trigger_on_allocate = true;

    RD_ASSERT_THROW(pool.reserve(1u), typename pool_allocator_type::exception_type);
    RD_ASSERT_EQ(pool.capacity(), 0u);

    pool_allocator_type::trigger_after_allocate = true;

    RD_ASSERT_THROW(pool.reserve(2 * packed_page_size), typename pool_allocator_type::exception_type);
    RD_ASSERT_EQ(pool.capacity(), packed_page_size);

    pool.shrink_to_fit();

    RD_ASSERT_EQ(pool.capacity(), 0u);

    test::throwing_allocator<rendu::entity>::trigger_on_allocate = true;

    RD_ASSERT_THROW(pool.emplace(rendu::entity{0}, 0), test::throwing_allocator<rendu::entity>::exception_type);
    RD_ASSERT_FALSE(pool.contains(rendu::entity{0}));
    RD_ASSERT_TRUE(pool.empty());

    test::throwing_allocator<rendu::entity>::trigger_on_allocate = true;

    RD_ASSERT_THROW(base.push(rendu::entity{0}), test::throwing_allocator<rendu::entity>::exception_type);
    RD_ASSERT_FALSE(base.contains(rendu::entity{0}));
    RD_ASSERT_TRUE(base.empty());

    pool_allocator_type::trigger_on_allocate = true;

    RD_ASSERT_THROW(pool.emplace(rendu::entity{0}, 0), typename pool_allocator_type::exception_type);
    RD_ASSERT_FALSE(pool.contains(rendu::entity{0}));
    RD_ASSERT_NO_FATAL_FAILURE(pool.compact());
    RD_ASSERT_TRUE(pool.empty());

    pool.emplace(rendu::entity{0}, 0);
    const rendu::entity entities[2u]{rendu::entity{1}, rendu::entity{sparse_page_size}};
    test::throwing_allocator<rendu::entity>::trigger_after_allocate = true;

    RD_ASSERT_THROW(pool.insert(std::begin(entities), std::end(entities), value_type{0}), test::throwing_allocator<rendu::entity>::exception_type);
    RD_ASSERT_TRUE(pool.contains(rendu::entity{1}));
    RD_ASSERT_FALSE(pool.contains(rendu::entity{sparse_page_size}));

    pool.erase(rendu::entity{1});
    const value_type components[2u]{value_type{1}, value_type{sparse_page_size}};
    test::throwing_allocator<rendu::entity>::trigger_on_allocate = true;
    pool.compact();

    RD_ASSERT_THROW(pool.insert(std::begin(entities), std::end(entities), std::begin(components)), test::throwing_allocator<rendu::entity>::exception_type);
    RD_ASSERT_TRUE(pool.contains(rendu::entity{1}));
    RD_ASSERT_FALSE(pool.contains(rendu::entity{sparse_page_size}));

    RD_ASSERT_EQ(on_construct.value, 1);
    RD_ASSERT_EQ(on_destroy.value, 1);
  };

  test(rendu::sigh_mixin<rendu::basic_storage<int, rendu::entity, test::throwing_allocator<int>>>{});
  test(rendu::sigh_mixin<rendu::basic_storage<stable_type, rendu::entity, test::throwing_allocator<stable_type>>>{});
}

RD_TEST(SighMixin, ThrowingComponent) {
  rendu::sigh_mixin<rendu::storage<test::throwing_type>> pool;
  using registry_type = typename decltype(pool)::registry_type;
  registry_type registry;

  counter on_construct{};
  counter on_destroy{};

  pool.bind(rendu::forward_as_any(registry));
  pool.on_construct().connect<&listener<registry_type>>(on_construct);
  pool.on_destroy().connect<&listener<registry_type>>(on_destroy);

  test::throwing_type::trigger_on_value = 42;

  // strong exception safety
  RD_ASSERT_THROW(pool.emplace(rendu::entity{0}, test::throwing_type{42}), typename test::throwing_type::exception_type);
  RD_ASSERT_TRUE(pool.empty());

  const rendu::entity entities[2u]{rendu::entity{42}, rendu::entity{1}};
  const test::throwing_type components[2u]{42, 1};

  // basic exception safety
  RD_ASSERT_THROW(pool.insert(std::begin(entities), std::end(entities), test::throwing_type{42}), typename test::throwing_type::exception_type);
  RD_ASSERT_EQ(pool.size(), 0u);
  RD_ASSERT_FALSE(pool.contains(rendu::entity{1}));

  // basic exception safety
  RD_ASSERT_THROW(pool.insert(std::begin(entities), std::end(entities), std::begin(components)), typename test::throwing_type::exception_type);
  RD_ASSERT_EQ(pool.size(), 0u);
  RD_ASSERT_FALSE(pool.contains(rendu::entity{1}));

  // basic exception safety
  RD_ASSERT_THROW(pool.insert(std::rbegin(entities), std::rend(entities), std::rbegin(components)), typename test::throwing_type::exception_type);
  RD_ASSERT_EQ(pool.size(), 1u);
  RD_ASSERT_TRUE(pool.contains(rendu::entity{1}));
  RD_ASSERT_EQ(pool.get(rendu::entity{1}), 1);

  pool.clear();
  pool.emplace(rendu::entity{1}, 1);
  pool.emplace(rendu::entity{42}, 42);

  // basic exception safety
  RD_ASSERT_THROW(pool.erase(rendu::entity{1}), typename test::throwing_type::exception_type);
  RD_ASSERT_EQ(pool.size(), 2u);
  RD_ASSERT_TRUE(pool.contains(rendu::entity{42}));
  RD_ASSERT_TRUE(pool.contains(rendu::entity{1}));
  RD_ASSERT_EQ(pool.at(0u), rendu::entity{1});
  RD_ASSERT_EQ(pool.at(1u), rendu::entity{42});
  RD_ASSERT_EQ(pool.get(rendu::entity{42}), 42);
  // the element may have been moved but it's still there
  RD_ASSERT_EQ(pool.get(rendu::entity{1}), test::throwing_type::moved_from_value);

  test::throwing_type::trigger_on_value = 99;
  pool.erase(rendu::entity{1});

  RD_ASSERT_EQ(pool.size(), 1u);
  RD_ASSERT_TRUE(pool.contains(rendu::entity{42}));
  RD_ASSERT_FALSE(pool.contains(rendu::entity{1}));
  RD_ASSERT_EQ(pool.at(0u), rendu::entity{42});
  RD_ASSERT_EQ(pool.get(rendu::entity{42}), 42);

  RD_ASSERT_EQ(on_construct.value, 2);
  RD_ASSERT_EQ(on_destroy.value, 3);
}

struct empty_each_tag final {};

template<>
struct rendu::basic_storage<empty_each_tag, rendu::entity, std::allocator<empty_each_tag>>: rendu::basic_storage<void, rendu::entity, std::allocator<void>> {
  basic_storage(const std::allocator<empty_each_tag> &) {}

  [[nodiscard]] iterable each() noexcept {
    return {internal::extended_storage_iterator{base_type::end()}, internal::extended_storage_iterator{base_type::end()}};
  }
};

RD_TEST(SighMixin, EmptyEachStorage) {
  rendu::sigh_mixin<rendu::storage<empty_each_tag>> pool;
  rendu::registry registry;

  counter on_destroy{};

  pool.bind(rendu::forward_as_any(registry));
  pool.on_destroy().connect<&listener<rendu::registry>>(on_destroy);

  RD_ASSERT_TRUE(pool.empty());
  RD_ASSERT_EQ(on_destroy.value, 0);

  pool.push(rendu::entity{42});

  RD_ASSERT_FALSE(pool.empty());
  RD_ASSERT_EQ(on_destroy.value, 0);

  RD_ASSERT_NE(pool.begin(), pool.end());
  RD_ASSERT_EQ(pool.each().begin(), pool.each().end());
  RD_ASSERT_EQ(on_destroy.value, 0);

  pool.clear();

  RD_ASSERT_EQ(pool.begin(), pool.end());
  RD_ASSERT_EQ(pool.each().begin(), pool.each().end());
  // no signal at all because of the (fake) empty iterable
  RD_ASSERT_EQ(on_destroy.value, 0);
}