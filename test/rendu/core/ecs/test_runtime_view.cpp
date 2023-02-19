/*
* Created by boil on 2023/2/19.
*/
#include <test/rdtest.h>
#include <core/ecs/entity.h>
#include <core/ecs/registry.h>
#include <core/ecs/runtime_view.h>

struct stable_type {
  static constexpr auto in_place_delete = true;
  int value;
};

template<typename Type>
struct RuntimeView: testing::Test {
  using type = Type;
};

using RuntimeViewTypes = ::testing::Types<rendu::runtime_view, rendu::const_runtime_view>;

TYPED_TEST_SUITE(RuntimeView, RuntimeViewTypes, );

TYPED_TEST(RuntimeView, Functionalities) {
  using runtime_view_type = typename TestFixture::type;

  rendu::registry registry;
  runtime_view_type view{};

  const auto e0 = registry.create();
  const auto e1 = registry.create();

  RD_ASSERT_EQ(view.size_hint(), 0u);
  RD_ASSERT_EQ(view.begin(), view.end());
  RD_ASSERT_FALSE(view.contains(e0));
  RD_ASSERT_FALSE(view.contains(e1));

  // forces the creation of the pools
  static_cast<void>(registry.storage<int>());
  static_cast<void>(registry.storage<char>());

  view.iterate(registry.storage<int>()).iterate(registry.storage<char>());

  RD_ASSERT_EQ(view.size_hint(), 0u);

  registry.emplace<char>(e0);
  registry.emplace<int>(e1);

  RD_ASSERT_NE(view.size_hint(), 0u);

  registry.emplace<char>(e1);

  RD_ASSERT_EQ(view.size_hint(), 1u);

  auto it = view.begin();

  RD_ASSERT_EQ(*it, e1);
  RD_ASSERT_EQ(++it, (view.end()));

  RD_ASSERT_NO_FATAL_FAILURE((view.begin()++));
  RD_ASSERT_NO_FATAL_FAILURE((++view.begin()));

  RD_ASSERT_NE(view.begin(), view.end());
  RD_ASSERT_EQ(view.size_hint(), 1u);

  registry.get<char>(e0) = '1';
  registry.get<char>(e1) = '2';
  registry.get<int>(e1) = 42;

  for(auto entity: view) {
    RD_ASSERT_EQ(registry.get<int>(entity), 42);
    RD_ASSERT_EQ(registry.get<char>(entity), '2');
  }

  view.clear();

  RD_ASSERT_EQ(view.size_hint(), 0u);
  RD_ASSERT_EQ(view.begin(), view.end());
}

TYPED_TEST(RuntimeView, Constructors) {
  using runtime_view_type = typename TestFixture::type;

  rendu::registry registry;
  runtime_view_type view{};

  const auto entity = registry.create();
  registry.emplace<int>(entity);

  view = runtime_view_type{std::allocator<int>{}};
  view.iterate(registry.storage<int>());

  RD_ASSERT_TRUE(view.contains(entity));

  runtime_view_type temp{view, view.get_allocator()};
  runtime_view_type other{std::move(temp), view.get_allocator()};

  RD_ASSERT_TRUE(view.contains(entity));
  RD_ASSERT_FALSE(temp.contains(entity));
  RD_ASSERT_TRUE(other.contains(entity));
}

TYPED_TEST(RuntimeView, Copy) {
  using runtime_view_type = typename TestFixture::type;

  rendu::registry registry;
  runtime_view_type view{};

  const auto entity = registry.create();
  registry.emplace<int>(entity);
  registry.emplace<char>(entity);

  view.iterate(registry.storage<int>());

  RD_ASSERT_TRUE(view.contains(entity));

  runtime_view_type other{view};

  RD_ASSERT_TRUE(view.contains(entity));
  RD_ASSERT_TRUE(other.contains(entity));

  other.iterate(registry.storage<int>()).exclude(registry.storage<char>());

  RD_ASSERT_TRUE(view.contains(entity));
  RD_ASSERT_FALSE(other.contains(entity));

  other = view;

  RD_ASSERT_TRUE(view.contains(entity));
  RD_ASSERT_TRUE(other.contains(entity));
}

TYPED_TEST(RuntimeView, Move) {
  using runtime_view_type = typename TestFixture::type;

  rendu::registry registry;
  runtime_view_type view{};

  const auto entity = registry.create();
  registry.emplace<int>(entity);
  registry.emplace<char>(entity);

  view.iterate(registry.storage<int>());

  RD_ASSERT_TRUE(view.contains(entity));

  runtime_view_type other{std::move(view)};

  RD_ASSERT_FALSE(view.contains(entity));
  RD_ASSERT_TRUE(other.contains(entity));

  view = other;
  other.iterate(registry.storage<int>()).exclude(registry.storage<char>());

  RD_ASSERT_TRUE(view.contains(entity));
  RD_ASSERT_FALSE(other.contains(entity));

  other = std::move(view);

  RD_ASSERT_FALSE(view.contains(entity));
  RD_ASSERT_TRUE(other.contains(entity));
}

TYPED_TEST(RuntimeView, Swap) {
  using runtime_view_type = typename TestFixture::type;

  rendu::registry registry;
  runtime_view_type view{};
  runtime_view_type other{};

  const auto entity = registry.create();

  registry.emplace<int>(entity);
  view.iterate(registry.storage<int>());

  RD_ASSERT_EQ(view.size_hint(), 1u);
  RD_ASSERT_EQ(other.size_hint(), 0u);
  RD_ASSERT_TRUE(view.contains(entity));
  RD_ASSERT_FALSE(other.contains(entity));
  RD_ASSERT_NE(view.begin(), view.end());
  RD_ASSERT_EQ(other.begin(), other.end());

  view.swap(other);

  RD_ASSERT_EQ(view.size_hint(), 0u);
  RD_ASSERT_EQ(other.size_hint(), 1u);
  RD_ASSERT_FALSE(view.contains(entity));
  RD_ASSERT_TRUE(other.contains(entity));
  RD_ASSERT_EQ(view.begin(), view.end());
  RD_ASSERT_NE(other.begin(), other.end());
}

TYPED_TEST(RuntimeView, Iterator) {
  using runtime_view_type = typename TestFixture::type;
  using iterator = typename runtime_view_type::iterator;

  rendu::registry registry;
  runtime_view_type view{};

  const auto entity = registry.create();

  registry.emplace<int>(entity);
  view.iterate(registry.storage<int>());

  iterator end{view.begin()};
  iterator begin{};
  begin = view.end();
  std::swap(begin, end);

  RD_ASSERT_EQ(begin, view.begin());
  RD_ASSERT_EQ(end, view.end());
  RD_ASSERT_NE(begin, end);

  RD_ASSERT_EQ(begin++, view.begin());
  RD_ASSERT_EQ(begin--, view.end());

  RD_ASSERT_EQ(++begin, view.end());
  RD_ASSERT_EQ(--begin, view.begin());

  RD_ASSERT_EQ(*begin, entity);
  RD_ASSERT_EQ(*begin.operator->(), entity);
}

TYPED_TEST(RuntimeView, Contains) {
  using runtime_view_type = typename TestFixture::type;

  rendu::registry registry;
  runtime_view_type view{};

  const auto entity = registry.create();
  const auto other = registry.create();

  registry.emplace<int>(entity);
  registry.emplace<int>(other);

  registry.destroy(entity);

  view.iterate(registry.storage<int>());

  RD_ASSERT_FALSE(view.contains(entity));
  RD_ASSERT_TRUE(view.contains(other));
}

TYPED_TEST(RuntimeView, Empty) {
  using runtime_view_type = typename TestFixture::type;

  rendu::registry registry;
  runtime_view_type view{};

  const auto entity = registry.create();
  const auto other = registry.create();

  registry.emplace<double>(entity);
  registry.emplace<float>(other);

  view.iterate(registry.storage<int>());

  RD_ASSERT_FALSE(view.contains(entity));
  RD_ASSERT_FALSE(view.contains(other));
  RD_ASSERT_EQ(view.begin(), view.end());
  RD_ASSERT_EQ((std::find(view.begin(), view.end(), entity)), view.end());
  RD_ASSERT_EQ((std::find(view.begin(), view.end(), other)), view.end());
}

TYPED_TEST(RuntimeView, Each) {
  using runtime_view_type = typename TestFixture::type;

  rendu::registry registry;
  runtime_view_type view{};

  const auto entity = registry.create();
  const auto other = registry.create();

  registry.emplace<int>(entity);
  registry.emplace<char>(entity);
  registry.emplace<char>(other);

  view.iterate(registry.storage<int>()).iterate(registry.storage<char>());

  view.each([entity](const auto rendu) {
    RD_ASSERT_EQ(rendu, entity);
  });
}

TYPED_TEST(RuntimeView, EachWithHoles) {
  using runtime_view_type = typename TestFixture::type;

  rendu::registry registry;
  runtime_view_type view{};

  const auto e0 = registry.create();
  const auto e1 = registry.create();
  const auto e2 = registry.create();

  registry.emplace<char>(e0, '0');
  registry.emplace<char>(e1, '1');

  registry.emplace<int>(e0, 0);
  registry.emplace<int>(e2, 2);

  view.iterate(registry.storage<int>()).iterate(registry.storage<char>());

  view.each([e0](auto rendu) {
    RD_ASSERT_EQ(e0, rendu);
  });
}

TYPED_TEST(RuntimeView, ExcludedComponents) {
  using runtime_view_type = typename TestFixture::type;

  rendu::registry registry;
  runtime_view_type view{};

  const auto e0 = registry.create();
  registry.emplace<int>(e0);

  const auto e1 = registry.create();
  registry.emplace<int>(e1);
  registry.emplace<char>(e1);

  view.iterate(registry.storage<int>())
      .exclude(registry.storage<char>());

  RD_ASSERT_TRUE(view.contains(e0));
  RD_ASSERT_FALSE(view.contains(e1));

  view.each([e0](auto rendu) {
    RD_ASSERT_EQ(e0, rendu);
  });
}

TYPED_TEST(RuntimeView, StableType) {
  using runtime_view_type = typename TestFixture::type;

  rendu::registry registry;
  runtime_view_type view{};

  const auto e0 = registry.create();
  const auto e1 = registry.create();
  const auto e2 = registry.create();

  registry.emplace<int>(e0);
  registry.emplace<int>(e1);
  registry.emplace<int>(e2);

  registry.emplace<stable_type>(e0);
  registry.emplace<stable_type>(e1);

  registry.remove<stable_type>(e1);

  view.iterate(registry.storage<int>()).iterate(registry.storage<stable_type>());

  RD_ASSERT_EQ(view.size_hint(), 2u);
  RD_ASSERT_TRUE(view.contains(e0));
  RD_ASSERT_FALSE(view.contains(e1));

  RD_ASSERT_EQ(*view.begin(), e0);
  RD_ASSERT_EQ(++view.begin(), view.end());

  view.each([e0](const auto rendu) {
    RD_ASSERT_EQ(e0, rendu);
  });

  for(auto rendu: view) {
    static_assert(std::is_same_v<decltype(rendu), rendu::entity>);
    RD_ASSERT_EQ(e0, rendu);
  }

  registry.compact();

  RD_ASSERT_EQ(view.size_hint(), 1u);
}

TYPED_TEST(RuntimeView, StableTypeWithExcludedComponent) {
  using runtime_view_type = typename TestFixture::type;

  rendu::registry registry;
  runtime_view_type view{};

  const auto entity = registry.create();
  const auto other = registry.create();

  registry.emplace<stable_type>(entity, 0);
  registry.emplace<stable_type>(other, 42);
  registry.emplace<int>(entity);

  view.iterate(registry.storage<stable_type>()).exclude(registry.storage<int>());

  RD_ASSERT_EQ(view.size_hint(), 2u);
  RD_ASSERT_FALSE(view.contains(entity));
  RD_ASSERT_TRUE(view.contains(other));

  registry.destroy(entity);

  RD_ASSERT_EQ(view.size_hint(), 2u);
  RD_ASSERT_FALSE(view.contains(entity));
  RD_ASSERT_TRUE(view.contains(other));

  for(auto rendu: view) {
    constexpr rendu::entity tombstone = rendu::tombstone;
    RD_ASSERT_NE(rendu, tombstone);
    RD_ASSERT_EQ(rendu, other);
  }

  view.each([other](const auto rendu) {
    constexpr rendu::entity tombstone = rendu::tombstone;
    RD_ASSERT_NE(rendu, tombstone);
    RD_ASSERT_EQ(rendu, other);
  });
}
