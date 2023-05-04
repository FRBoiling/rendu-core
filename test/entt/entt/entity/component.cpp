#include <gtest/gtest.h>
#include <entt/entt.hpp>

struct empty {};

struct non_empty {
  int value;
};

struct non_movable {
  non_movable() = default;
  non_movable(const non_movable &) = delete;
  non_movable &operator=(const non_movable &) = delete;
  int value;
};

struct self_contained {
  static constexpr auto in_place_delete = true;
  static constexpr auto page_size = 4u;
};

struct traits_based {};

template<>
struct entt::component_traits<traits_based> {
  using type = traits_based;
  static constexpr auto in_place_delete = false;
  static constexpr auto page_size = 8u;
};

TEST(Component, VoidType) {
  using traits_type = entt::component_traits<void>;

  EXPECT_TRUE(!traits_type::in_place_delete);
  EXPECT_TRUE(traits_type::page_size == 0u);
}

TEST(Component, Empty) {
  using traits_type = entt::component_traits<empty>;

  EXPECT_TRUE(!traits_type::in_place_delete);
  EXPECT_TRUE(traits_type::page_size == 0u);
}

TEST(Component, NonEmpty) {
  using traits_type = entt::component_traits<non_empty>;

  EXPECT_TRUE(!traits_type::in_place_delete);
  EXPECT_TRUE(traits_type::page_size == ENTT_PACKED_PAGE);
}

TEST(Component, NonMovable) {
  using traits_type = entt::component_traits<non_movable>;

  EXPECT_TRUE(traits_type::in_place_delete);
  EXPECT_TRUE(traits_type::page_size == ENTT_PACKED_PAGE);
}

TEST(Component, SelfContained) {
  using traits_type = entt::component_traits<self_contained>;

  EXPECT_TRUE(traits_type::in_place_delete);
  EXPECT_TRUE(traits_type::page_size == 4u);
}

TEST(Component, TraitsBased) {
  using traits_type = entt::component_traits<traits_based>;

  EXPECT_TRUE(!traits_type::in_place_delete);
  EXPECT_TRUE(traits_type::page_size == 8u);
}
