/*
* Created by boil on 2023/2/19.
*/

#include <test/rdtest.h>
#include <test/throwing_allocator.h>
#include <core/ecs/entity.h>
#include <core/ecs/sparse_set.h>

struct empty_type {};

struct boxed_int {
  int value;
};

RD_TEST(SparseSet, Functionalities) {
  rendu::sparse_set set;

  RD_ASSERT_NO_FATAL_FAILURE([[maybe_unused]] auto alloc = set.get_allocator());
  RD_ASSERT_EQ(set.type(), rendu::type_id<void>());

  set.reserve(42);

  RD_ASSERT_EQ(set.capacity(), 42u);
  RD_ASSERT_TRUE(set.empty());
  RD_ASSERT_EQ(set.size(), 0u);
  RD_ASSERT_TRUE(set.contiguous());
  RD_ASSERT_EQ(std::as_const(set).begin(), std::as_const(set).end());
  RD_ASSERT_EQ(set.begin(), set.end());
  RD_ASSERT_FALSE(set.contains(rendu::entity{0}));
  RD_ASSERT_FALSE(set.contains(rendu::entity{42}));

  set.reserve(0);

  RD_ASSERT_EQ(set.capacity(), 42u);
  RD_ASSERT_TRUE(set.empty());

  set.push(rendu::entity{42});

  RD_ASSERT_FALSE(set.empty());
  RD_ASSERT_EQ(set.size(), 1u);
  RD_ASSERT_TRUE(set.contiguous());
  RD_ASSERT_NE(std::as_const(set).begin(), std::as_const(set).end());
  RD_ASSERT_NE(set.begin(), set.end());
  RD_ASSERT_FALSE(set.contains(rendu::entity{0}));
  RD_ASSERT_TRUE(set.contains(rendu::entity{42}));
  RD_ASSERT_EQ(set.index(rendu::entity{42}), 0u);
  RD_ASSERT_EQ(set.at(0u), rendu::entity{42});
  RD_ASSERT_EQ(set.at(1u), static_cast<rendu::entity>(rendu::null));
  RD_ASSERT_EQ(set[0u], rendu::entity{42});
  RD_ASSERT_EQ(set.value(rendu::entity{42}), nullptr);

  set.erase(rendu::entity{42});

  RD_ASSERT_TRUE(set.empty());
  RD_ASSERT_EQ(set.size(), 0u);
  RD_ASSERT_TRUE(set.contiguous());
  RD_ASSERT_EQ(std::as_const(set).begin(), std::as_const(set).end());
  RD_ASSERT_EQ(set.begin(), set.end());
  RD_ASSERT_FALSE(set.contains(rendu::entity{0}));
  RD_ASSERT_FALSE(set.contains(rendu::entity{42}));
  RD_ASSERT_EQ(set.at(0u), static_cast<rendu::entity>(rendu::null));
  RD_ASSERT_EQ(set.at(1u), static_cast<rendu::entity>(rendu::null));

  set.push(rendu::entity{42});

  RD_ASSERT_FALSE(set.empty());
  RD_ASSERT_EQ(set.index(rendu::entity{42}), 0u);
  RD_ASSERT_EQ(set.at(0u), rendu::entity{42});
  RD_ASSERT_EQ(set.at(1u), static_cast<rendu::entity>(rendu::null));
  RD_ASSERT_EQ(set[0u], rendu::entity{42});

  set.clear();

  RD_ASSERT_TRUE(set.empty());
  RD_ASSERT_EQ(set.size(), 0u);
  RD_ASSERT_TRUE(set.contiguous());
  RD_ASSERT_EQ(std::as_const(set).begin(), std::as_const(set).end());
  RD_ASSERT_EQ(set.begin(), set.end());
  RD_ASSERT_FALSE(set.contains(rendu::entity{0}));
  RD_ASSERT_FALSE(set.contains(rendu::entity{42}));

  RD_ASSERT_NO_FATAL_FAILURE(set.bind(rendu::any{}));
}

RD_TEST(SparseSet, Contains) {
  using traits_type = rendu::entity_traits<rendu::entity>;

  rendu::sparse_set set{rendu::deletion_policy::in_place};

  set.push(rendu::entity{0});
  set.push(rendu::entity{3});
  set.push(rendu::entity{42});
  set.push(rendu::entity{99});
  set.push(traits_type::construct(1, 5));

  RD_ASSERT_FALSE(set.contains(rendu::null));
  RD_ASSERT_FALSE(set.contains(rendu::tombstone));

  RD_ASSERT_TRUE(set.contains(rendu::entity{0}));
  RD_ASSERT_TRUE(set.contains(rendu::entity{3}));
  RD_ASSERT_TRUE(set.contains(rendu::entity{42}));
  RD_ASSERT_TRUE(set.contains(rendu::entity{99}));
  RD_ASSERT_FALSE(set.contains(rendu::entity{1}));
  RD_ASSERT_TRUE(set.contains(traits_type::construct(1, 5)));

  RD_ASSERT_TRUE(set.contains(traits_type::construct(3, 0)));
  RD_ASSERT_FALSE(set.contains(traits_type::construct(42, 1)));
  RD_ASSERT_FALSE(set.contains(traits_type::construct(99, traits_type::to_version(rendu::tombstone))));

  set.erase(rendu::entity{0});
  set.erase(rendu::entity{3});

  set.remove(rendu::entity{42});
  set.remove(rendu::entity{99});

  RD_ASSERT_FALSE(set.contains(rendu::null));
  RD_ASSERT_FALSE(set.contains(rendu::tombstone));

  RD_ASSERT_FALSE(set.contains(rendu::entity{0}));
  RD_ASSERT_FALSE(set.contains(rendu::entity{3}));
  RD_ASSERT_FALSE(set.contains(rendu::entity{42}));
  RD_ASSERT_FALSE(set.contains(rendu::entity{99}));
  RD_ASSERT_FALSE(set.contains(rendu::entity{1}));
  RD_ASSERT_TRUE(set.contains(traits_type::construct(1, 5)));

  RD_ASSERT_FALSE(set.contains(traits_type::construct(99, traits_type::to_version(rendu::tombstone))));
}

RD_TEST(SparseSet, Current) {
  using traits_type = rendu::entity_traits<rendu::entity>;

  rendu::sparse_set set{rendu::deletion_policy::in_place};

  RD_ASSERT_EQ(set.current(traits_type::construct(0, 0)), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(traits_type::construct(3, 3)), traits_type::to_version(rendu::tombstone));

  set.push(traits_type::construct(0, 0));
  set.push(traits_type::construct(3, 3));

  RD_ASSERT_NE(set.current(traits_type::construct(0, 0)), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_NE(set.current(traits_type::construct(3, 3)), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(traits_type::construct(3, 0)), traits_type::to_version(traits_type::construct(3, 3)));
  RD_ASSERT_EQ(set.current(traits_type::construct(42, 1)), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(traits_type::construct(traits_type::page_size, 1)),
               traits_type::to_version(rendu::tombstone));

  set.remove(rendu::entity{0});

  RD_ASSERT_EQ(set.current(traits_type::construct(0, 0)), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(traits_type::construct(3, 0)), traits_type::to_version(traits_type::construct(3, 3)));
}

RD_TEST(SparseSet, Index) {
  using traits_type = rendu::entity_traits<rendu::entity>;

  rendu::sparse_set set{};

  set.push(traits_type::construct(0, 0));
  set.push(traits_type::construct(3, 3));

  RD_ASSERT_EQ(set.index(traits_type::construct(0, 0)), 0u);
  RD_ASSERT_EQ(set.index(traits_type::construct(3, 3)), 1u);

  set.erase(traits_type::construct(0, 0));

  RD_ASSERT_EQ(set.index(traits_type::construct(3, 3)), 0u);
}

RD_DEBUG_TEST(SparseSetDeathTest, Index) {
  using traits_type = rendu::entity_traits<rendu::entity>;

  rendu::sparse_set set{};

  RD_ASSERT_DEATH(static_cast<void>(set.index(traits_type::construct(3, 0))), "");
  RD_ASSERT_DEATH(static_cast<void>(set.index(rendu::null)), "");
}

RD_TEST(SparseSet, Move) {
  rendu::sparse_set set;
  set.push(rendu::entity{42});

  RD_ASSERT_TRUE(std::is_move_constructible_v<decltype(set)>);
  RD_ASSERT_TRUE(std::is_move_assignable_v<decltype(set)>);

  rendu::sparse_set other{std::move(set)};

  RD_ASSERT_TRUE(set.empty());
  RD_ASSERT_FALSE(other.empty());
  RD_ASSERT_EQ(set.at(0u), static_cast<rendu::entity>(rendu::null));
  RD_ASSERT_EQ(other.at(0u), rendu::entity{42});

  set = std::move(other);

  RD_ASSERT_FALSE(set.empty());
  RD_ASSERT_TRUE(other.empty());
  RD_ASSERT_EQ(set.at(0u), rendu::entity{42});
  RD_ASSERT_EQ(other.at(0u), static_cast<rendu::entity>(rendu::null));

  other = rendu::sparse_set{};
  other.push(rendu::entity{3});
  other = std::move(set);

  RD_ASSERT_TRUE(set.empty());
  RD_ASSERT_FALSE(other.empty());
  RD_ASSERT_EQ(set.at(0u), static_cast<rendu::entity>(rendu::null));
  RD_ASSERT_EQ(other.at(0u), rendu::entity{42});
}

RD_TEST(SparseSet, Swap) {
  rendu::sparse_set set;
  rendu::sparse_set other{rendu::deletion_policy::in_place};

  set.push(rendu::entity{42});

  other.push(rendu::entity{9});
  other.push(rendu::entity{3});
  other.erase(rendu::entity{9});

  RD_ASSERT_EQ(set.size(), 1u);
  RD_ASSERT_EQ(other.size(), 2u);

  set.swap(other);

  RD_ASSERT_EQ(set.size(), 2u);
  RD_ASSERT_EQ(other.size(), 1u);

  RD_ASSERT_EQ(set.at(1u), rendu::entity{3});
  RD_ASSERT_EQ(other.at(0u), rendu::entity{42});
}

RD_TEST(SparseSet, Pagination) {
  using traits_type = rendu::entity_traits<rendu::entity>;

  rendu::sparse_set set{};

  RD_ASSERT_EQ(set.extent(), 0u);

  set.push(rendu::entity{traits_type::page_size - 1u});

  RD_ASSERT_EQ(set.extent(), traits_type::page_size);
  RD_ASSERT_TRUE(set.contains(rendu::entity{traits_type::page_size - 1u}));

  set.push(rendu::entity{traits_type::page_size});

  RD_ASSERT_EQ(set.extent(), 2 * traits_type::page_size);
  RD_ASSERT_TRUE(set.contains(rendu::entity{traits_type::page_size - 1u}));
  RD_ASSERT_TRUE(set.contains(rendu::entity{traits_type::page_size}));
  RD_ASSERT_FALSE(set.contains(rendu::entity{traits_type::page_size + 1u}));

  set.erase(rendu::entity{traits_type::page_size - 1u});

  RD_ASSERT_EQ(set.extent(), 2 * traits_type::page_size);
  RD_ASSERT_FALSE(set.contains(rendu::entity{traits_type::page_size - 1u}));
  RD_ASSERT_TRUE(set.contains(rendu::entity{traits_type::page_size}));

  set.shrink_to_fit();
  set.erase(rendu::entity{traits_type::page_size});

  RD_ASSERT_EQ(set.extent(), 2 * traits_type::page_size);
  RD_ASSERT_FALSE(set.contains(rendu::entity{traits_type::page_size - 1u}));
  RD_ASSERT_FALSE(set.contains(rendu::entity{traits_type::page_size}));

  set.shrink_to_fit();

  RD_ASSERT_EQ(set.extent(), 2 * traits_type::page_size);
}

RD_TEST(SparseSet, Push) {
  rendu::sparse_set set{rendu::deletion_policy::in_place};
  rendu::entity entities[2u]{rendu::entity{3}, rendu::entity{42}};

  RD_ASSERT_TRUE(set.empty());
  RD_ASSERT_NE(set.push(entities[0u]), set.end());

  set.erase(entities[0u]);

  RD_ASSERT_NE(set.push(entities[1u]), set.end());
  RD_ASSERT_NE(set.push(entities[0u]), set.end());

  RD_ASSERT_EQ(set.at(0u), entities[1u]);
  RD_ASSERT_EQ(set.at(1u), entities[0u]);
  RD_ASSERT_EQ(set.index(entities[0u]), 1u);
  RD_ASSERT_EQ(set.index(entities[1u]), 0u);

  set.erase(std::begin(entities), std::end(entities));

  RD_ASSERT_NE(set.push(entities[1u]), set.end());
  RD_ASSERT_NE(set.push(entities[0u]), set.end());

  RD_ASSERT_EQ(set.at(0u), entities[1u]);
  RD_ASSERT_EQ(set.at(1u), entities[0u]);
  RD_ASSERT_EQ(set.index(entities[0u]), 1u);
  RD_ASSERT_EQ(set.index(entities[1u]), 0u);
}

RD_TEST(SparseSet, PushRange) {
  rendu::sparse_set set{rendu::deletion_policy::in_place};
  rendu::entity entities[2u]{rendu::entity{3}, rendu::entity{42}};

  set.push(rendu::entity{12});

  RD_ASSERT_EQ(set.push(std::end(entities), std::end(entities)), set.end());
  RD_ASSERT_NE(set.push(std::begin(entities), std::end(entities)), set.end());

  set.push(rendu::entity{24});

  RD_ASSERT_TRUE(set.contains(entities[0u]));
  RD_ASSERT_TRUE(set.contains(entities[1u]));
  RD_ASSERT_FALSE(set.contains(rendu::entity{0}));
  RD_ASSERT_FALSE(set.contains(rendu::entity{9}));
  RD_ASSERT_TRUE(set.contains(rendu::entity{12}));
  RD_ASSERT_TRUE(set.contains(rendu::entity{24}));

  RD_ASSERT_FALSE(set.empty());
  RD_ASSERT_EQ(set.size(), 4u);
  RD_ASSERT_EQ(set.index(rendu::entity{12}), 0u);
  RD_ASSERT_EQ(set.index(entities[0u]), 1u);
  RD_ASSERT_EQ(set.index(entities[1u]), 2u);
  RD_ASSERT_EQ(set.index(rendu::entity{24}), 3u);
  RD_ASSERT_EQ(set.data()[set.index(rendu::entity{12})], rendu::entity{12});
  RD_ASSERT_EQ(set.data()[set.index(entities[0u])], entities[0u]);
  RD_ASSERT_EQ(set.data()[set.index(entities[1u])], entities[1u]);
  RD_ASSERT_EQ(set.data()[set.index(rendu::entity{24})], rendu::entity{24});

  set.erase(std::begin(entities), std::end(entities));

  RD_ASSERT_NE(set.push(std::rbegin(entities), std::rend(entities)), set.end());

  RD_ASSERT_EQ(set.size(), 6u);
  RD_ASSERT_EQ(set.at(4u), entities[1u]);
  RD_ASSERT_EQ(set.at(5u), entities[0u]);
  RD_ASSERT_EQ(set.index(entities[0u]), 5u);
  RD_ASSERT_EQ(set.index(entities[1u]), 4u);
}

RD_DEBUG_TEST(SparseSetDeathTest, Push) {
  rendu::sparse_set set{rendu::deletion_policy::in_place};
  rendu::entity entities[2u]{rendu::entity{3}, rendu::entity{42}};
  set.push(rendu::entity{42});

  RD_ASSERT_DEATH(set.push(rendu::entity{42}), "");
  RD_ASSERT_DEATH(set.push(std::begin(entities), std::end(entities)), "");
}

RD_TEST(SparseSet, PushOutOfBounds) {
  using traits_type = rendu::entity_traits<rendu::entity>;

  rendu::sparse_set set{rendu::deletion_policy::in_place};
  rendu::entity entities[2u]{rendu::entity{0}, rendu::entity{traits_type::page_size}};

  RD_ASSERT_NE(set.push(entities[0u]), set.end());
  RD_ASSERT_EQ(set.extent(), traits_type::page_size);
  RD_ASSERT_EQ(set.index(entities[0u]), 0u);

  set.erase(entities[0u]);

  RD_ASSERT_NE(set.push(entities[1u]), set.end());
  RD_ASSERT_EQ(set.extent(), 2u * traits_type::page_size);
  RD_ASSERT_EQ(set.index(entities[1u]), 0u);
}

RD_TEST(SparseSet, Bump) {
  using traits_type = rendu::entity_traits<rendu::entity>;

  rendu::sparse_set set;
  rendu::entity entities[3u]{rendu::entity{3}, rendu::entity{42}, traits_type::construct(9, 3)};
  set.push(std::begin(entities), std::end(entities));

  RD_ASSERT_EQ(set.current(entities[0u]), 0u);
  RD_ASSERT_EQ(set.current(entities[1u]), 0u);
  RD_ASSERT_EQ(set.current(entities[2u]), 3u);

  set.bump(entities[0u]);
  set.bump(traits_type::construct(traits_type::to_entity(entities[1u]), 1));
  set.bump(traits_type::construct(traits_type::to_entity(entities[2u]), 0));

  RD_ASSERT_EQ(set.current(entities[0u]), 0u);
  RD_ASSERT_EQ(set.current(entities[1u]), 1u);
  RD_ASSERT_EQ(set.current(entities[2u]), 0u);
}

RD_DEBUG_TEST(SparseSetDeathTest, Bump) {
  using traits_type = rendu::entity_traits<rendu::entity>;

  rendu::sparse_set set{rendu::deletion_policy::in_place};
  set.push(rendu::entity{3});

  RD_ASSERT_DEATH(set.bump(rendu::null), "");
  RD_ASSERT_DEATH(set.bump(rendu::tombstone), "");
  RD_ASSERT_DEATH(set.bump(rendu::entity{42}), "");
  RD_ASSERT_DEATH(set.bump(traits_type::construct(traits_type::to_entity(rendu::entity{3}),
                                                  traits_type::to_version(rendu::tombstone))), "");
}

RD_TEST(SparseSet, Erase) {
  using traits_type = rendu::entity_traits<rendu::entity>;

  rendu::sparse_set set;
  rendu::entity entities[3u]{rendu::entity{3}, rendu::entity{42}, traits_type::construct(9, 3)};

  RD_ASSERT_EQ(set.policy(), rendu::deletion_policy::swap_and_pop);
  RD_ASSERT_TRUE(set.empty());

  set.push(std::begin(entities), std::end(entities));
  set.erase(set.begin(), set.end());

  RD_ASSERT_TRUE(set.empty());
  RD_ASSERT_EQ(set.current(entities[0u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[1u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[2u]), traits_type::to_version(rendu::tombstone));

  set.push(std::begin(entities), std::end(entities));
  set.erase(entities, entities + 2u);

  RD_ASSERT_FALSE(set.empty());
  RD_ASSERT_EQ(set.current(entities[0u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[1u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[2u]), traits_type::to_version(entities[2u]));
  RD_ASSERT_EQ(*set.begin(), entities[2u]);

  set.erase(entities[2u]);

  RD_ASSERT_TRUE(set.empty());
  RD_ASSERT_EQ(set.current(entities[2u]), traits_type::to_version(rendu::tombstone));

  set.push(std::begin(entities), std::end(entities));
  std::swap(entities[1u], entities[2u]);
  set.erase(entities, entities + 2u);

  RD_ASSERT_FALSE(set.empty());
  RD_ASSERT_EQ(set.current(entities[2u]), traits_type::to_version(entities[2u]));
  RD_ASSERT_EQ(*set.begin(), entities[2u]);
}

RD_DEBUG_TEST(SparseSetDeathTest, Erase) {
  using traits_type = rendu::entity_traits<rendu::entity>;

  rendu::sparse_set set;
  rendu::entity entities[2u]{rendu::entity{42}, traits_type::construct(9, 3)};

  RD_ASSERT_DEATH(set.erase(std::begin(entities), std::end(entities)), "");
  RD_ASSERT_DEATH(set.erase(rendu::null), "");
}

RD_TEST(SparseSet, CrossErase) {
  rendu::sparse_set set;
  rendu::sparse_set other;
  rendu::entity entities[2u]{rendu::entity{3}, rendu::entity{42}};

  set.push(std::begin(entities), std::end(entities));
  other.push(entities[1u]);
  set.erase(other.begin(), other.end());

  RD_ASSERT_TRUE(set.contains(entities[0u]));
  RD_ASSERT_FALSE(set.contains(entities[1u]));
  RD_ASSERT_EQ(set.data()[0u], entities[0u]);
}

RD_TEST(SparseSet, StableErase) {
  using traits_type = rendu::entity_traits<rendu::entity>;

  rendu::sparse_set set{rendu::deletion_policy::in_place};
  rendu::entity entities[3u]{rendu::entity{3}, rendu::entity{42}, traits_type::construct(9, 3)};

  RD_ASSERT_EQ(set.policy(), rendu::deletion_policy::in_place);
  RD_ASSERT_TRUE(set.empty());
  RD_ASSERT_EQ(set.size(), 0u);

  set.push(entities[0u]);
  set.push(entities[1u]);
  set.push(entities[2u]);

  set.erase(set.begin(), set.end());

  RD_ASSERT_FALSE(set.empty());
  RD_ASSERT_EQ(set.size(), 3u);
  RD_ASSERT_EQ(set.current(entities[0u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[1u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[2u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_TRUE(set.at(0u) == rendu::tombstone);
  RD_ASSERT_TRUE(set.at(1u) == rendu::tombstone);
  RD_ASSERT_TRUE(set.at(2u) == rendu::tombstone);

  set.push(entities[0u]);
  set.push(entities[1u]);
  set.push(entities[2u]);

  set.erase(entities, entities + 2u);

  RD_ASSERT_FALSE(set.empty());
  RD_ASSERT_EQ(set.size(), 3u);
  RD_ASSERT_EQ(set.current(entities[0u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[1u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[2u]), traits_type::to_version(entities[2u]));
  RD_ASSERT_EQ(*set.begin(), entities[2u]);
  RD_ASSERT_TRUE(set.at(0u) == rendu::tombstone);
  RD_ASSERT_TRUE(set.at(1u) == rendu::tombstone);

  set.erase(entities[2u]);

  RD_ASSERT_FALSE(set.empty());
  RD_ASSERT_EQ(set.size(), 3u);
  RD_ASSERT_EQ(set.current(entities[2u]), traits_type::to_version(rendu::tombstone));

  set.push(entities[0u]);
  set.push(entities[1u]);
  set.push(entities[2u]);

  std::swap(entities[1u], entities[2u]);
  set.erase(entities, entities + 2u);

  RD_ASSERT_FALSE(set.empty());
  RD_ASSERT_EQ(set.size(), 3u);
  RD_ASSERT_EQ(set.current(entities[2u]), traits_type::to_version(entities[2u]));
  RD_ASSERT_TRUE(set.at(0u) == rendu::tombstone);
  RD_ASSERT_EQ(set.at(1u), entities[2u]);
  RD_ASSERT_TRUE(set.at(2u) == rendu::tombstone);
  RD_ASSERT_EQ(*++set.begin(), entities[2u]);

  set.compact();

  RD_ASSERT_FALSE(set.empty());
  RD_ASSERT_EQ(set.size(), 1u);
  RD_ASSERT_EQ(set.current(entities[0u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[1u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[2u]), traits_type::to_version(entities[2u]));
  RD_ASSERT_TRUE(set.at(0u) == entities[2u]);
  RD_ASSERT_EQ(*set.begin(), entities[2u]);

  set.clear();

  RD_ASSERT_EQ(set.size(), 0u);
  RD_ASSERT_EQ(set.current(entities[2u]), traits_type::to_version(rendu::tombstone));

  set.push(entities[0u]);
  set.push(entities[1u]);
  set.push(entities[2u]);

  set.erase(entities[2u]);

  RD_ASSERT_NE(set.current(entities[0u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_NE(set.current(entities[1u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[2u]), traits_type::to_version(rendu::tombstone));

  set.erase(entities[0u]);
  set.erase(entities[1u]);

  RD_ASSERT_EQ(set.size(), 3u);
  RD_ASSERT_EQ(set.current(entities[0u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[1u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[2u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_TRUE(*set.begin() == rendu::tombstone);

  set.push(entities[0u]);

  RD_ASSERT_EQ(*++set.begin(), entities[0u]);

  set.push(entities[1u]);
  set.push(entities[2u]);
  set.push(rendu::entity{0});

  RD_ASSERT_EQ(set.size(), 4u);
  RD_ASSERT_EQ(*set.begin(), rendu::entity{0});
  RD_ASSERT_EQ(set.at(0u), entities[1u]);
  RD_ASSERT_EQ(set.at(1u), entities[0u]);
  RD_ASSERT_EQ(set.at(2u), entities[2u]);

  RD_ASSERT_NE(set.current(entities[0u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_NE(set.current(entities[1u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_NE(set.current(entities[2u]), traits_type::to_version(rendu::tombstone));
}

RD_DEBUG_TEST(SparseSetDeathTest, StableErase) {
  using traits_type = rendu::entity_traits<rendu::entity>;

  rendu::sparse_set set{rendu::deletion_policy::in_place};
  rendu::entity entities[2u]{rendu::entity{42}, traits_type::construct(9, 3)};

  RD_ASSERT_DEATH(set.erase(std::begin(entities), std::end(entities)), "");
  RD_ASSERT_DEATH(set.erase(rendu::null), "");
}

RD_TEST(SparseSet, CrossStableErase) {
  rendu::sparse_set set{rendu::deletion_policy::in_place};
  rendu::sparse_set other{rendu::deletion_policy::in_place};
  rendu::entity entities[2u]{rendu::entity{3}, rendu::entity{42}};

  set.push(std::begin(entities), std::end(entities));
  other.push(entities[1u]);
  set.erase(other.begin(), other.end());

  RD_ASSERT_TRUE(set.contains(entities[0u]));
  RD_ASSERT_FALSE(set.contains(entities[1u]));
  RD_ASSERT_EQ(set.data()[0u], entities[0u]);
}

RD_TEST(SparseSet, Remove) {
  using traits_type = rendu::entity_traits<rendu::entity>;

  rendu::sparse_set set;
  rendu::entity entities[3u]{rendu::entity{3}, rendu::entity{42}, traits_type::construct(9, 3)};

  RD_ASSERT_EQ(set.policy(), rendu::deletion_policy::swap_and_pop);
  RD_ASSERT_TRUE(set.empty());

  RD_ASSERT_EQ(set.remove(std::begin(entities), std::end(entities)), 0u);
  RD_ASSERT_FALSE(set.remove(entities[1u]));

  RD_ASSERT_TRUE(set.empty());

  set.push(std::begin(entities), std::end(entities));

  RD_ASSERT_EQ(set.remove(set.begin(), set.end()), 3u);
  RD_ASSERT_TRUE(set.empty());
  RD_ASSERT_EQ(set.current(entities[0u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[1u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[2u]), traits_type::to_version(rendu::tombstone));

  set.push(std::begin(entities), std::end(entities));

  RD_ASSERT_EQ(set.remove(entities, entities + 2u), 2u);
  RD_ASSERT_FALSE(set.empty());
  RD_ASSERT_EQ(set.current(entities[0u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[1u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[2u]), traits_type::to_version(entities[2u]));
  RD_ASSERT_EQ(*set.begin(), entities[2u]);

  RD_ASSERT_TRUE(set.remove(entities[2u]));
  RD_ASSERT_FALSE(set.remove(entities[2u]));
  RD_ASSERT_TRUE(set.empty());
  RD_ASSERT_EQ(set.current(entities[2u]), traits_type::to_version(rendu::tombstone));

  set.push(entities, entities + 2u);

  RD_ASSERT_EQ(set.remove(std::begin(entities), std::end(entities)), 2u);
  RD_ASSERT_EQ(set.current(entities[0u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[1u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[2u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_TRUE(set.empty());

  set.push(std::begin(entities), std::end(entities));
  std::swap(entities[1u], entities[2u]);

  RD_ASSERT_EQ(set.remove(entities, entities + 2u), 2u);
  RD_ASSERT_EQ(set.current(entities[2u]), traits_type::to_version(entities[2u]));
  RD_ASSERT_FALSE(set.empty());
  RD_ASSERT_EQ(*set.begin(), entities[2u]);

  RD_ASSERT_FALSE(set.remove(traits_type::construct(9, 0)));
  RD_ASSERT_FALSE(set.remove(rendu::tombstone));
  RD_ASSERT_FALSE(set.remove(rendu::null));
}

RD_TEST(SparseSet, CrossRemove) {
  rendu::sparse_set set;
  rendu::sparse_set other;
  rendu::entity entities[2u]{rendu::entity{3}, rendu::entity{42}};

  set.push(std::begin(entities), std::end(entities));
  other.push(entities[1u]);
  set.remove(other.begin(), other.end());

  RD_ASSERT_TRUE(set.contains(entities[0u]));
  RD_ASSERT_FALSE(set.contains(entities[1u]));
  RD_ASSERT_EQ(set.data()[0u], entities[0u]);
}

RD_TEST(SparseSet, StableRemove) {
  using traits_type = rendu::entity_traits<rendu::entity>;

  rendu::sparse_set set{rendu::deletion_policy::in_place};
  rendu::entity entities[3u]{rendu::entity{3}, rendu::entity{42}, traits_type::construct(9, 3)};

  RD_ASSERT_EQ(set.policy(), rendu::deletion_policy::in_place);
  RD_ASSERT_TRUE(set.empty());
  RD_ASSERT_EQ(set.size(), 0u);

  RD_ASSERT_EQ(set.remove(std::begin(entities), std::end(entities)), 0u);
  RD_ASSERT_FALSE(set.remove(entities[1u]));

  RD_ASSERT_TRUE(set.empty());
  RD_ASSERT_EQ(set.size(), 0u);

  set.push(entities[0u]);
  set.push(entities[1u]);
  set.push(entities[2u]);

  RD_ASSERT_EQ(set.remove(set.begin(), set.end()), 3u);
  RD_ASSERT_EQ(set.remove(set.begin(), set.end()), 0u);

  RD_ASSERT_FALSE(set.empty());
  RD_ASSERT_EQ(set.size(), 3u);
  RD_ASSERT_EQ(set.current(entities[0u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[1u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[2u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_TRUE(set.at(0u) == rendu::tombstone);
  RD_ASSERT_TRUE(set.at(1u) == rendu::tombstone);
  RD_ASSERT_TRUE(set.at(2u) == rendu::tombstone);

  set.push(entities[0u]);
  set.push(entities[1u]);
  set.push(entities[2u]);

  RD_ASSERT_EQ(set.remove(entities, entities + 2u), 2u);
  RD_ASSERT_EQ(set.remove(entities, entities + 2u), 0u);

  RD_ASSERT_FALSE(set.empty());
  RD_ASSERT_EQ(set.size(), 3u);
  RD_ASSERT_EQ(set.current(entities[0u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[1u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[2u]), traits_type::to_version(entities[2u]));
  RD_ASSERT_EQ(*set.begin(), entities[2u]);
  RD_ASSERT_TRUE(set.at(0u) == rendu::tombstone);
  RD_ASSERT_TRUE(set.at(1u) == rendu::tombstone);

  RD_ASSERT_TRUE(set.remove(entities[2u]));
  RD_ASSERT_FALSE(set.remove(entities[2u]));

  RD_ASSERT_FALSE(set.empty());
  RD_ASSERT_EQ(set.size(), 3u);
  RD_ASSERT_EQ(set.current(entities[2u]), traits_type::to_version(rendu::tombstone));

  set.push(entities[0u]);
  set.push(entities[1u]);
  set.push(entities[2u]);

  std::swap(entities[1u], entities[2u]);

  RD_ASSERT_EQ(set.remove(entities, entities + 2u), 2u);
  RD_ASSERT_EQ(set.remove(entities, entities + 2u), 0u);

  RD_ASSERT_FALSE(set.empty());
  RD_ASSERT_EQ(set.size(), 3u);
  RD_ASSERT_EQ(set.current(entities[2u]), traits_type::to_version(entities[2u]));
  RD_ASSERT_TRUE(set.at(0u) == rendu::tombstone);
  RD_ASSERT_EQ(set.at(1u), entities[2u]);
  RD_ASSERT_TRUE(set.at(2u) == rendu::tombstone);
  RD_ASSERT_EQ(*++set.begin(), entities[2u]);

  set.compact();

  RD_ASSERT_FALSE(set.empty());
  RD_ASSERT_EQ(set.size(), 1u);
  RD_ASSERT_EQ(set.current(entities[0u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[1u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[2u]), traits_type::to_version(entities[2u]));
  RD_ASSERT_TRUE(set.at(0u) == entities[2u]);
  RD_ASSERT_EQ(*set.begin(), entities[2u]);

  set.clear();

  RD_ASSERT_EQ(set.size(), 0u);
  RD_ASSERT_EQ(set.current(entities[2u]), traits_type::to_version(rendu::tombstone));

  set.push(entities[0u]);
  set.push(entities[1u]);
  set.push(entities[2u]);

  RD_ASSERT_TRUE(set.remove(entities[2u]));
  RD_ASSERT_FALSE(set.remove(entities[2u]));

  RD_ASSERT_NE(set.current(entities[0u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_NE(set.current(entities[1u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[2u]), traits_type::to_version(rendu::tombstone));

  RD_ASSERT_TRUE(set.remove(entities[0u]));
  RD_ASSERT_TRUE(set.remove(entities[1u]));
  RD_ASSERT_EQ(set.remove(entities, entities + 2u), 0u);

  RD_ASSERT_EQ(set.size(), 3u);
  RD_ASSERT_EQ(set.current(entities[0u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[1u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_EQ(set.current(entities[2u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_TRUE(*set.begin() == rendu::tombstone);

  set.push(entities[0u]);

  RD_ASSERT_EQ(*++set.begin(), entities[0u]);

  set.push(entities[1u]);
  set.push(entities[2u]);
  set.push(rendu::entity{0});

  RD_ASSERT_EQ(set.size(), 4u);
  RD_ASSERT_EQ(*set.begin(), rendu::entity{0});
  RD_ASSERT_EQ(set.at(0u), entities[1u]);
  RD_ASSERT_EQ(set.at(1u), entities[0u]);
  RD_ASSERT_EQ(set.at(2u), entities[2u]);

  RD_ASSERT_NE(set.current(entities[0u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_NE(set.current(entities[1u]), traits_type::to_version(rendu::tombstone));
  RD_ASSERT_NE(set.current(entities[2u]), traits_type::to_version(rendu::tombstone));

  RD_ASSERT_FALSE(set.remove(traits_type::construct(9, 0)));
  RD_ASSERT_FALSE(set.remove(rendu::null));
}

RD_TEST(SparseSet, CrossStableRemove) {
  rendu::sparse_set set{rendu::deletion_policy::in_place};
  rendu::sparse_set other{rendu::deletion_policy::in_place};
  rendu::entity entities[2u]{rendu::entity{3}, rendu::entity{42}};

  set.push(std::begin(entities), std::end(entities));
  other.push(entities[1u]);
  set.remove(other.begin(), other.end());

  RD_ASSERT_TRUE(set.contains(entities[0u]));
  RD_ASSERT_FALSE(set.contains(entities[1u]));
  RD_ASSERT_EQ(set.data()[0u], entities[0u]);
}

RD_TEST(SparseSet, Compact) {
  rendu::sparse_set set{rendu::deletion_policy::in_place};

  RD_ASSERT_TRUE(set.empty());
  RD_ASSERT_EQ(set.size(), 0u);

  set.compact();

  RD_ASSERT_TRUE(set.empty());
  RD_ASSERT_EQ(set.size(), 0u);

  set.push(rendu::entity{0});
  set.compact();

  RD_ASSERT_FALSE(set.empty());
  RD_ASSERT_EQ(set.size(), 1u);

  set.push(rendu::entity{42});
  set.erase(rendu::entity{0});

  RD_ASSERT_EQ(set.size(), 2u);
  RD_ASSERT_EQ(set.index(rendu::entity{42}), 1u);

  set.compact();

  RD_ASSERT_EQ(set.size(), 1u);
  RD_ASSERT_EQ(set.index(rendu::entity{42}), 0u);

  set.push(rendu::entity{0});
  set.compact();

  RD_ASSERT_EQ(set.size(), 2u);
  RD_ASSERT_EQ(set.index(rendu::entity{42}), 0u);
  RD_ASSERT_EQ(set.index(rendu::entity{0}), 1u);

  set.erase(rendu::entity{0});
  set.erase(rendu::entity{42});
  set.compact();

  RD_ASSERT_TRUE(set.empty());
}

RD_TEST(SparseSet, SwapEntity) {
  using traits_type = rendu::entity_traits<rendu::entity>;

  rendu::sparse_set set;

  set.push(traits_type::construct(3, 5));
  set.push(traits_type::construct(42, 99));

  RD_ASSERT_EQ(set.index(traits_type::construct(3, 5)), 0u);
  RD_ASSERT_EQ(set.index(traits_type::construct(42, 99)), 1u);

  set.swap_elements(traits_type::construct(3, 5), traits_type::construct(42, 99));

  RD_ASSERT_EQ(set.index(traits_type::construct(3, 5)), 1u);
  RD_ASSERT_EQ(set.index(traits_type::construct(42, 99)), 0u);

  set.swap_elements(traits_type::construct(3, 5), traits_type::construct(42, 99));

  RD_ASSERT_EQ(set.index(traits_type::construct(3, 5)), 0u);
  RD_ASSERT_EQ(set.index(traits_type::construct(42, 99)), 1u);
}

RD_DEBUG_TEST(SparseSetDeathTest, SwapEntity) {
  rendu::sparse_set set;

  RD_ASSERT_TRUE(set.empty());
  RD_ASSERT_DEATH(set.swap_elements(rendu::entity{0}, rendu::entity{1}), "");
}

RD_TEST(SparseSet, Clear) {
  rendu::sparse_set set{rendu::deletion_policy::in_place};

  set.push(rendu::entity{3});
  set.push(rendu::entity{42});
  set.push(rendu::entity{9});
  set.erase(rendu::entity{42});

  RD_ASSERT_FALSE(set.empty());
  RD_ASSERT_EQ(set.size(), 3u);
  RD_ASSERT_EQ(*set.begin(), rendu::entity{9});

  set.clear();

  RD_ASSERT_TRUE(set.empty());
  RD_ASSERT_EQ(set.size(), 0u);

  RD_ASSERT_EQ(set.find(rendu::entity{3}), set.end());
  RD_ASSERT_EQ(set.find(rendu::entity{9}), set.end());

  set.push(rendu::entity{3});
  set.push(rendu::entity{42});
  set.push(rendu::entity{9});

  RD_ASSERT_FALSE(set.empty());
  RD_ASSERT_EQ(set.size(), 3u);
  RD_ASSERT_EQ(*set.begin(), rendu::entity{9});

  set.clear();

  RD_ASSERT_TRUE(set.empty());
  RD_ASSERT_EQ(set.size(), 0u);

  RD_ASSERT_EQ(set.find(rendu::entity{3}), set.end());
  RD_ASSERT_EQ(set.find(rendu::entity{42}), set.end());
  RD_ASSERT_EQ(set.find(rendu::entity{9}), set.end());
}

RD_TEST(SparseSet, Contiguous) {
  rendu::sparse_set swap_and_pop{rendu::deletion_policy::swap_and_pop};
  rendu::sparse_set in_place{rendu::deletion_policy::in_place};

  const rendu::entity entity{42};
  const rendu::entity other{3};

  RD_ASSERT_TRUE(swap_and_pop.contiguous());
  RD_ASSERT_TRUE(in_place.contiguous());

  swap_and_pop.push(entity);
  in_place.push(entity);

  swap_and_pop.push(other);
  in_place.push(other);

  RD_ASSERT_TRUE(swap_and_pop.contiguous());
  RD_ASSERT_TRUE(in_place.contiguous());

  swap_and_pop.erase(entity);
  in_place.erase(entity);

  RD_ASSERT_TRUE(swap_and_pop.contiguous());
  RD_ASSERT_FALSE(in_place.contiguous());

  swap_and_pop.push(entity);
  in_place.push(entity);

  RD_ASSERT_TRUE(swap_and_pop.contiguous());
  RD_ASSERT_TRUE(in_place.contiguous());

  swap_and_pop.erase(other);
  in_place.erase(other);

  RD_ASSERT_TRUE(swap_and_pop.contiguous());
  RD_ASSERT_FALSE(in_place.contiguous());

  in_place.compact();

  RD_ASSERT_TRUE(swap_and_pop.contiguous());
  RD_ASSERT_TRUE(in_place.contiguous());

  swap_and_pop.push(other);
  in_place.push(other);

  swap_and_pop.erase(entity);
  in_place.erase(entity);

  RD_ASSERT_TRUE(swap_and_pop.contiguous());
  RD_ASSERT_FALSE(in_place.contiguous());

  swap_and_pop.clear();
  in_place.clear();

  RD_ASSERT_TRUE(swap_and_pop.contiguous());
  RD_ASSERT_TRUE(in_place.contiguous());
}

RD_TEST(SparseSet, Iterator) {
  using iterator = typename rendu::sparse_set::iterator;

  static_assert(std::is_same_v<iterator::value_type, rendu::entity>);
  static_assert(std::is_same_v<iterator::pointer, const rendu::entity *>);
  static_assert(std::is_same_v<iterator::reference, const rendu::entity &>);

  rendu::sparse_set set;
  set.push(rendu::entity{3});

  iterator end{set.begin()};
  iterator begin{};

  RD_ASSERT_EQ(end.data(), set.data());
  RD_ASSERT_EQ(begin.data(), nullptr);

  begin = set.end();
  std::swap(begin, end);

  RD_ASSERT_EQ(end.data(), set.data());
  RD_ASSERT_EQ(begin.data(), set.data());

  RD_ASSERT_EQ(begin, set.cbegin());
  RD_ASSERT_EQ(end, set.cend());
  RD_ASSERT_NE(begin, end);

  RD_ASSERT_EQ(begin.index(), 0);
  RD_ASSERT_EQ(end.index(), -1);

  RD_ASSERT_EQ(begin++, set.begin());
  RD_ASSERT_EQ(begin--, set.end());

  RD_ASSERT_EQ(begin + 1, set.end());
  RD_ASSERT_EQ(end - 1, set.begin());

  RD_ASSERT_EQ(++begin, set.end());
  RD_ASSERT_EQ(--begin, set.begin());

  RD_ASSERT_EQ(begin += 1, set.end());
  RD_ASSERT_EQ(begin -= 1, set.begin());

  RD_ASSERT_EQ(begin + (end - begin), set.end());
  RD_ASSERT_EQ(begin - (begin - end), set.end());

  RD_ASSERT_EQ(end - (end - begin), set.begin());
  RD_ASSERT_EQ(end + (begin - end), set.begin());

  RD_ASSERT_EQ(begin[0u], *set.begin());

  RD_ASSERT_LT(begin, end);
  RD_ASSERT_LE(begin, set.begin());

  RD_ASSERT_GT(end, begin);
  RD_ASSERT_GE(end, set.end());

  RD_ASSERT_EQ(*begin, rendu::entity{3});
  RD_ASSERT_EQ(*begin.operator->(), rendu::entity{3});

  RD_ASSERT_EQ(begin.index(), 0);
  RD_ASSERT_EQ(end.index(), -1);

  set.push(rendu::entity{42});
  begin = set.begin();

  RD_ASSERT_EQ(begin.index(), 1);
  RD_ASSERT_EQ(end.index(), -1);

  RD_ASSERT_EQ(begin[0u], rendu::entity{42});
  RD_ASSERT_EQ(begin[1u], rendu::entity{3});
}

RD_TEST(SparseSet, ReverseIterator) {
  using reverse_iterator = typename rendu::sparse_set::reverse_iterator;

  static_assert(std::is_same_v<reverse_iterator::value_type, rendu::entity>);
  static_assert(std::is_same_v<reverse_iterator::pointer, const rendu::entity *>);
  static_assert(std::is_same_v<reverse_iterator::reference, const rendu::entity &>);

  rendu::sparse_set set;
  set.push(rendu::entity{3});

  reverse_iterator end{set.rbegin()};
  reverse_iterator begin{};
  begin = set.rend();
  std::swap(begin, end);

  RD_ASSERT_EQ(begin, set.crbegin());
  RD_ASSERT_EQ(end, set.crend());
  RD_ASSERT_NE(begin, end);

  RD_ASSERT_EQ(begin.base().index(), -1);
  RD_ASSERT_EQ(end.base().index(), 0);

  RD_ASSERT_EQ(begin++, set.rbegin());
  RD_ASSERT_EQ(begin--, set.rend());

  RD_ASSERT_EQ(begin + 1, set.rend());
  RD_ASSERT_EQ(end - 1, set.rbegin());

  RD_ASSERT_EQ(++begin, set.rend());
  RD_ASSERT_EQ(--begin, set.rbegin());

  RD_ASSERT_EQ(begin += 1, set.rend());
  RD_ASSERT_EQ(begin -= 1, set.rbegin());

  RD_ASSERT_EQ(begin + (end - begin), set.rend());
  RD_ASSERT_EQ(begin - (begin - end), set.rend());

  RD_ASSERT_EQ(end - (end - begin), set.rbegin());
  RD_ASSERT_EQ(end + (begin - end), set.rbegin());

  RD_ASSERT_EQ(begin[0u], *set.rbegin());

  RD_ASSERT_LT(begin, end);
  RD_ASSERT_LE(begin, set.rbegin());

  RD_ASSERT_GT(end, begin);
  RD_ASSERT_GE(end, set.rend());

  RD_ASSERT_EQ(*begin, rendu::entity{3});
  RD_ASSERT_EQ(*begin.operator->(), rendu::entity{3});

  RD_ASSERT_EQ(begin.base().index(), -1);
  RD_ASSERT_EQ(end.base().index(), 0);

  set.push(rendu::entity{42});
  end = set.rend();

  RD_ASSERT_EQ(begin.base().index(), -1);
  RD_ASSERT_EQ(end.base().index(), 1);

  RD_ASSERT_EQ(begin[0u], rendu::entity{3});
  RD_ASSERT_EQ(begin[1u], rendu::entity{42});
}

RD_TEST(SparseSet, Find) {
  using traits_type = rendu::entity_traits<rendu::entity>;

  rendu::sparse_set set;
  set.push(rendu::entity{3});
  set.push(rendu::entity{42});
  set.push(traits_type::construct(99, 1));

  RD_ASSERT_NE(set.find(rendu::entity{3}), set.end());
  RD_ASSERT_NE(set.find(rendu::entity{42}), set.end());
  RD_ASSERT_NE(set.find(traits_type::construct(99, 1)), set.end());
  RD_ASSERT_EQ(set.find(traits_type::construct(99, 5)), set.end());
  RD_ASSERT_EQ(set.find(rendu::entity{0}), set.end());
  RD_ASSERT_EQ(set.find(rendu::tombstone), set.end());
  RD_ASSERT_EQ(set.find(rendu::null), set.end());

  auto it = set.find(traits_type::construct(99, 1));

  RD_ASSERT_EQ(*it, traits_type::construct(99, 1));
  RD_ASSERT_EQ(*(++it), rendu::entity{42});
  RD_ASSERT_EQ(*(++it), rendu::entity{3});
  RD_ASSERT_EQ(++it, set.end());
  RD_ASSERT_EQ(++set.find(rendu::entity{3}), set.end());
}

RD_TEST(SparseSet, Data) {
  rendu::sparse_set set;

  set.push(rendu::entity{3});
  set.push(rendu::entity{12});
  set.push(rendu::entity{42});

  RD_ASSERT_EQ(set.index(rendu::entity{3}), 0u);
  RD_ASSERT_EQ(set.index(rendu::entity{12}), 1u);
  RD_ASSERT_EQ(set.index(rendu::entity{42}), 2u);

  RD_ASSERT_EQ(set.data()[0u], rendu::entity{3});
  RD_ASSERT_EQ(set.data()[1u], rendu::entity{12});
  RD_ASSERT_EQ(set.data()[2u], rendu::entity{42});
}

RD_TEST(SparseSet, SortOrdered) {
  rendu::sparse_set set;
  rendu::entity
      entities[5u]{rendu::entity{42}, rendu::entity{12}, rendu::entity{9}, rendu::entity{7}, rendu::entity{3}};

  set.push(std::begin(entities), std::end(entities));
  set.sort(std::less{});

  RD_ASSERT_TRUE(std::equal(std::rbegin(entities), std::rend(entities), set.begin(), set.end()));
}

RD_TEST(SparseSet, SortReverse) {
  rendu::sparse_set set;
  rendu::entity
      entities[5u]{rendu::entity{3}, rendu::entity{7}, rendu::entity{9}, rendu::entity{12}, rendu::entity{42}};

  set.push(std::begin(entities), std::end(entities));
  set.sort(std::less{});

  RD_ASSERT_TRUE(std::equal(std::begin(entities), std::end(entities), set.begin(), set.end()));
}

RD_TEST(SparseSet, SortUnordered) {
  rendu::sparse_set set;
  rendu::entity
      entities[5u]{rendu::entity{9}, rendu::entity{7}, rendu::entity{3}, rendu::entity{12}, rendu::entity{42}};

  set.push(std::begin(entities), std::end(entities));
  set.sort(std::less{});

  auto begin = set.begin();
  auto end = set.end();

  RD_ASSERT_EQ(*(begin++), entities[2u]);
  RD_ASSERT_EQ(*(begin++), entities[1u]);
  RD_ASSERT_EQ(*(begin++), entities[0u]);
  RD_ASSERT_EQ(*(begin++), entities[3u]);
  RD_ASSERT_EQ(*(begin++), entities[4u]);
  RD_ASSERT_EQ(begin, end);
}

RD_TEST(SparseSet, SortRange) {
  rendu::sparse_set set{rendu::deletion_policy::in_place};
  rendu::entity
      entities[5u]{rendu::entity{7}, rendu::entity{9}, rendu::entity{3}, rendu::entity{12}, rendu::entity{42}};

  set.push(std::begin(entities), std::end(entities));
  set.erase(entities[0u]);

  RD_ASSERT_EQ(set.size(), 5u);

  set.sort(std::less{});

  RD_ASSERT_EQ(set.size(), 4u);
  RD_ASSERT_EQ(set[0u], entities[4u]);
  RD_ASSERT_EQ(set[1u], entities[3u]);
  RD_ASSERT_EQ(set[2u], entities[1u]);
  RD_ASSERT_EQ(set[3u], entities[2u]);

  set.clear();
  set.compact();
  set.push(std::begin(entities), std::end(entities));
  set.sort_n(0u, std::less{});

  RD_ASSERT_TRUE(std::equal(std::rbegin(entities), std::rend(entities), set.begin(), set.end()));

  set.sort_n(2u, std::less{});

  RD_ASSERT_EQ(set.data()[0u], entities[1u]);
  RD_ASSERT_EQ(set.data()[1u], entities[0u]);
  RD_ASSERT_EQ(set.data()[2u], entities[2u]);

  set.sort_n(5u, std::less{});

  auto begin = set.begin();
  auto end = set.end();

  RD_ASSERT_EQ(*(begin++), entities[2u]);
  RD_ASSERT_EQ(*(begin++), entities[0u]);
  RD_ASSERT_EQ(*(begin++), entities[1u]);
  RD_ASSERT_EQ(*(begin++), entities[3u]);
  RD_ASSERT_EQ(*(begin++), entities[4u]);
  RD_ASSERT_EQ(begin, end);
}

RD_DEBUG_TEST(SparseSetDeathTest, SortRange) {
  rendu::sparse_set set{rendu::deletion_policy::in_place};
  rendu::entity entity{42};

  set.push(entity);
  set.erase(entity);

  RD_ASSERT_DEATH(set.sort_n(0u, std::less{});, "");
  RD_ASSERT_DEATH(set.sort_n(3u, std::less{});, "");
}

RD_TEST(SparseSet, RespectDisjoint) {
  rendu::sparse_set lhs;
  rendu::sparse_set rhs;

  rendu::entity lhs_entities[3u]{rendu::entity{3}, rendu::entity{12}, rendu::entity{42}};
  lhs.push(std::begin(lhs_entities), std::end(lhs_entities));

  RD_ASSERT_TRUE(std::equal(std::rbegin(lhs_entities), std::rend(lhs_entities), lhs.begin(), lhs.end()));

  lhs.respect(rhs);

  RD_ASSERT_TRUE(std::equal(std::rbegin(lhs_entities), std::rend(lhs_entities), lhs.begin(), lhs.end()));
}

RD_TEST(SparseSet, RespectOverlap) {
  rendu::sparse_set lhs;
  rendu::sparse_set rhs;

  rendu::entity lhs_entities[3u]{rendu::entity{3}, rendu::entity{12}, rendu::entity{42}};
  lhs.push(std::begin(lhs_entities), std::end(lhs_entities));

  rendu::entity rhs_entities[1u]{rendu::entity{12}};
  rhs.push(std::begin(rhs_entities), std::end(rhs_entities));

  RD_ASSERT_TRUE(std::equal(std::rbegin(lhs_entities), std::rend(lhs_entities), lhs.begin(), lhs.end()));
  RD_ASSERT_TRUE(std::equal(std::rbegin(rhs_entities), std::rend(rhs_entities), rhs.begin(), rhs.end()));

  lhs.respect(rhs);

  auto begin = lhs.begin();
  auto end = lhs.end();

  RD_ASSERT_EQ(*(begin++), lhs_entities[1u]);
  RD_ASSERT_EQ(*(begin++), lhs_entities[2u]);
  RD_ASSERT_EQ(*(begin++), lhs_entities[0u]);
  RD_ASSERT_EQ(begin, end);
}

RD_TEST(SparseSet, RespectOrdered) {
  rendu::sparse_set lhs;
  rendu::sparse_set rhs;

  rendu::entity
      lhs_entities[5u]{rendu::entity{1}, rendu::entity{2}, rendu::entity{3}, rendu::entity{4}, rendu::entity{5}};
  lhs.push(std::begin(lhs_entities), std::end(lhs_entities));

  rendu::entity rhs_entities[6u]
      {rendu::entity{6}, rendu::entity{1}, rendu::entity{2}, rendu::entity{3}, rendu::entity{4}, rendu::entity{5}};
  rhs.push(std::begin(rhs_entities), std::end(rhs_entities));

  RD_ASSERT_TRUE(std::equal(std::rbegin(lhs_entities), std::rend(lhs_entities), lhs.begin(), lhs.end()));
  RD_ASSERT_TRUE(std::equal(std::rbegin(rhs_entities), std::rend(rhs_entities), rhs.begin(), rhs.end()));

  rhs.respect(lhs);

  RD_ASSERT_TRUE(std::equal(std::rbegin(rhs_entities), std::rend(rhs_entities), rhs.begin(), rhs.end()));
}

RD_TEST(SparseSet, RespectReverse) {
  rendu::sparse_set lhs;
  rendu::sparse_set rhs;

  rendu::entity
      lhs_entities[5u]{rendu::entity{1}, rendu::entity{2}, rendu::entity{3}, rendu::entity{4}, rendu::entity{5}};
  lhs.push(std::begin(lhs_entities), std::end(lhs_entities));

  rendu::entity rhs_entities[6u]
      {rendu::entity{5}, rendu::entity{4}, rendu::entity{3}, rendu::entity{2}, rendu::entity{1}, rendu::entity{6}};
  rhs.push(std::begin(rhs_entities), std::end(rhs_entities));

  RD_ASSERT_TRUE(std::equal(std::rbegin(lhs_entities), std::rend(lhs_entities), lhs.begin(), lhs.end()));
  RD_ASSERT_TRUE(std::equal(std::rbegin(rhs_entities), std::rend(rhs_entities), rhs.begin(), rhs.end()));

  rhs.respect(lhs);

  auto begin = rhs.begin();
  auto end = rhs.end();

  RD_ASSERT_EQ(*(begin++), rhs_entities[0u]);
  RD_ASSERT_EQ(*(begin++), rhs_entities[1u]);
  RD_ASSERT_EQ(*(begin++), rhs_entities[2u]);
  RD_ASSERT_EQ(*(begin++), rhs_entities[3u]);
  RD_ASSERT_EQ(*(begin++), rhs_entities[4u]);
  RD_ASSERT_EQ(*(begin++), rhs_entities[5u]);
  RD_ASSERT_EQ(begin, end);
}

RD_TEST(SparseSet, RespectUnordered) {
  rendu::sparse_set lhs;
  rendu::sparse_set rhs;

  rendu::entity
      lhs_entities[5u]{rendu::entity{1}, rendu::entity{2}, rendu::entity{3}, rendu::entity{4}, rendu::entity{5}};
  lhs.push(std::begin(lhs_entities), std::end(lhs_entities));

  rendu::entity rhs_entities[6u]
      {rendu::entity{3}, rendu::entity{2}, rendu::entity{6}, rendu::entity{1}, rendu::entity{4}, rendu::entity{5}};
  rhs.push(std::begin(rhs_entities), std::end(rhs_entities));

  RD_ASSERT_TRUE(std::equal(std::rbegin(lhs_entities), std::rend(lhs_entities), lhs.begin(), lhs.end()));
  RD_ASSERT_TRUE(std::equal(std::rbegin(rhs_entities), std::rend(rhs_entities), rhs.begin(), rhs.end()));

  rhs.respect(lhs);

  auto begin = rhs.begin();
  auto end = rhs.end();

  RD_ASSERT_EQ(*(begin++), rhs_entities[5u]);
  RD_ASSERT_EQ(*(begin++), rhs_entities[4u]);
  RD_ASSERT_EQ(*(begin++), rhs_entities[0u]);
  RD_ASSERT_EQ(*(begin++), rhs_entities[1u]);
  RD_ASSERT_EQ(*(begin++), rhs_entities[3u]);
  RD_ASSERT_EQ(*(begin++), rhs_entities[2u]);
  RD_ASSERT_EQ(begin, end);
}

RD_TEST(SparseSet, RespectInvalid) {
  using traits_type = rendu::entity_traits<rendu::entity>;

  rendu::sparse_set lhs;
  rendu::sparse_set rhs;

  rendu::entity lhs_entities[3u]{rendu::entity{1}, rendu::entity{2}, traits_type::construct(3, 1)};
  lhs.push(std::begin(lhs_entities), std::end(lhs_entities));

  rendu::entity rhs_entities[3u]{rendu::entity{2}, rendu::entity{1}, traits_type::construct(3, 2)};
  rhs.push(std::begin(rhs_entities), std::end(rhs_entities));

  RD_ASSERT_TRUE(std::equal(std::rbegin(lhs_entities), std::rend(lhs_entities), lhs.begin(), lhs.end()));
  RD_ASSERT_TRUE(std::equal(std::rbegin(rhs_entities), std::rend(rhs_entities), rhs.begin(), rhs.end()));

  rhs.respect(lhs);

  auto begin = rhs.begin();
  auto end = rhs.end();

  RD_ASSERT_EQ(*(begin++), rhs_entities[0u]);
  RD_ASSERT_EQ(*(begin++), rhs_entities[1u]);
  RD_ASSERT_EQ(*(begin++), rhs_entities[2u]);
  RD_ASSERT_EQ(rhs.current(rhs_entities[0u]), 0u);
  RD_ASSERT_EQ(rhs.current(rhs_entities[1u]), 0u);
  RD_ASSERT_EQ(rhs.current(rhs_entities[2u]), 2u);
  RD_ASSERT_EQ(begin, end);
}

RD_TEST(SparseSet, CanModifyDuringIteration) {
  rendu::sparse_set set;
  set.push(rendu::entity{0});

  RD_ASSERT_EQ(set.capacity(), 1u);

  const auto it = set.begin();
  set.reserve(2u);

  RD_ASSERT_EQ(set.capacity(), 2u);

// this should crash with asan enabled if we break the constraint
  [[maybe_unused]] const auto entity = *it;
}

RD_TEST(SparseSet, CustomAllocator) {
  test::throwing_allocator<rendu::entity> allocator{};
  rendu::basic_sparse_set<rendu::entity, test::throwing_allocator<rendu::entity>> set{allocator};

  RD_ASSERT_EQ(set.get_allocator(), allocator);

  set.reserve(1u);

  RD_ASSERT_EQ(set.capacity(), 1u);

  set.push(rendu::entity{0});
  set.push(rendu::entity{1});

  rendu::basic_sparse_set<rendu::entity, test::throwing_allocator<rendu::entity>> other{std::move(set), allocator};

  RD_ASSERT_TRUE(set.empty());
  RD_ASSERT_FALSE(other.empty());
  RD_ASSERT_EQ(set.capacity(), 0u);
  RD_ASSERT_EQ(other.capacity(), 2u);
  RD_ASSERT_EQ(other.size(), 2u);

  set = std::move(other);

  RD_ASSERT_FALSE(set.empty());
  RD_ASSERT_TRUE(other.empty());
  RD_ASSERT_EQ(other.capacity(), 0u);
  RD_ASSERT_EQ(set.capacity(), 2u);
  RD_ASSERT_EQ(set.size(), 2u);

  set.swap(other);
  set = std::move(other);

  RD_ASSERT_FALSE(set.empty());
  RD_ASSERT_TRUE(other.empty());
  RD_ASSERT_EQ(other.capacity(), 0u);
  RD_ASSERT_EQ(set.capacity(), 2u);
  RD_ASSERT_EQ(set.size(), 2u);

  set.clear();

  RD_ASSERT_EQ(set.capacity(), 2u);
  RD_ASSERT_EQ(set.size(), 0u);

  set.shrink_to_fit();

  RD_ASSERT_EQ(set.capacity(), 0u);
}

RD_TEST(SparseSet, ThrowingAllocator) {
  using traits_type = rendu::entity_traits<rendu::entity>;

  rendu::basic_sparse_set<rendu::entity, test::throwing_allocator<rendu::entity>> set{};

  test::throwing_allocator<rendu::entity>::trigger_on_allocate = true;

  RD_ASSERT_THROW(set.reserve(1u), test::throwing_allocator<rendu::entity>::exception_type);
  RD_ASSERT_EQ(set.capacity(), 0u);
  RD_ASSERT_EQ(set.extent(), 0u);

  test::throwing_allocator<rendu::entity>::trigger_on_allocate = true;

  RD_ASSERT_THROW(set.push(rendu::entity{0}), test::throwing_allocator<rendu::entity>::exception_type);
  RD_ASSERT_EQ(set.extent(), traits_type::page_size);
  RD_ASSERT_EQ(set.capacity(), 0u);

  set.push(rendu::entity{0});
  test::throwing_allocator<rendu::entity>::trigger_on_allocate = true;

  RD_ASSERT_THROW(set.reserve(2u), test::throwing_allocator<rendu::entity>::exception_type);
  RD_ASSERT_EQ(set.extent(), traits_type::page_size);
  RD_ASSERT_TRUE(set.contains(rendu::entity{0}));
  RD_ASSERT_EQ(set.capacity(), 1u);

  test::throwing_allocator<rendu::entity>::trigger_on_allocate = true;

  RD_ASSERT_THROW(set.push(rendu::entity{1}), test::throwing_allocator<rendu::entity>::exception_type);
  RD_ASSERT_EQ(set.extent(), traits_type::page_size);
  RD_ASSERT_TRUE(set.contains(rendu::entity{0}));
  RD_ASSERT_FALSE(set.contains(rendu::entity{1}));
  RD_ASSERT_EQ(set.capacity(), 1u);

  rendu::entity entities[2u]{rendu::entity{1}, rendu::entity{traits_type::page_size}};
  test::throwing_allocator<rendu::entity>::trigger_after_allocate = true;

  RD_ASSERT_THROW(set.push(std::begin(entities), std::end(entities)),
                  test::throwing_allocator<rendu::entity>::exception_type);
  RD_ASSERT_EQ(set.extent(), 2 * traits_type::page_size);
  RD_ASSERT_TRUE(set.contains(rendu::entity{0}));
  RD_ASSERT_TRUE(set.contains(rendu::entity{1}));
  RD_ASSERT_FALSE(set.contains(rendu::entity{traits_type::page_size}));
  RD_ASSERT_EQ(set.capacity(), 2u);
  RD_ASSERT_EQ(set.size(), 2u);

  set.push(entities[1u]);

  RD_ASSERT_TRUE(set.contains(rendu::entity{traits_type::page_size}));
}
