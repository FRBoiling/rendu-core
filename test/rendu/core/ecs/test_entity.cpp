/*
* Created by boil on 2023/2/17.
*/

#include <test/rdtest.h>
#include <core/ecs/entity.h>
#include <core/ecs/registry.h>

RD_TEST(Entity, Traits) {
  using traits_type = rendu::entity_traits<rendu::entity>;
  constexpr rendu::entity tombstone = rendu::tombstone;
  constexpr rendu::entity null = rendu::null;
  rendu::registry registry{};

  registry.destroy(registry.create());
  const auto entity = registry.create();
  const auto other = registry.create();

  RD_ASSERT_EQ(rendu::to_integral(entity), rendu::to_integral(entity));
  RD_ASSERT_NE(rendu::to_integral(entity), rendu::to_integral<rendu::entity>(rendu::null));
  RD_ASSERT_NE(rendu::to_integral(entity), rendu::to_integral(rendu::entity{}));

  RD_ASSERT_EQ(rendu::to_entity(entity), 0u);
  RD_ASSERT_EQ(rendu::to_version(entity), 1u);
  RD_ASSERT_EQ(rendu::to_entity(other), 1u);
  RD_ASSERT_EQ(rendu::to_version(other), 0u);

  RD_ASSERT_EQ(traits_type::construct(rendu::to_entity(entity), rendu::to_version(entity)), entity);
  RD_ASSERT_EQ(traits_type::construct(rendu::to_entity(other), rendu::to_version(other)), other);
  RD_ASSERT_NE(traits_type::construct(rendu::to_entity(entity), {}), entity);

  RD_ASSERT_EQ(traits_type::construct(rendu::to_entity(other), rendu::to_version(entity)),
            traits_type::combine(rendu::to_integral(other), rendu::to_integral(entity)));

  RD_ASSERT_EQ(traits_type::combine(rendu::tombstone, rendu::null), tombstone);
  RD_ASSERT_EQ(traits_type::combine(rendu::null, rendu::tombstone), null);

  RD_ASSERT_EQ(traits_type::next(entity),
            traits_type::construct(rendu::to_integral(entity), rendu::to_version(entity) + 1u));
  RD_ASSERT_EQ(traits_type::next(other), traits_type::construct(rendu::to_integral(other), rendu::to_version(other) + 1u));

  RD_ASSERT_EQ(traits_type::next(rendu::tombstone), traits_type::construct(rendu::null, {}));
  RD_ASSERT_EQ(traits_type::next(rendu::null), traits_type::construct(rendu::null, {}));
}

RD_TEST(Entity, Null) {
  using traits_type = rendu::entity_traits<rendu::entity>;
  constexpr rendu::entity null = rendu::null;

  RD_ASSERT_FALSE(rendu::entity{} == rendu::null);
  RD_ASSERT_TRUE(rendu::null == rendu::null);
  RD_ASSERT_FALSE(rendu::null != rendu::null);

  rendu::registry registry{};
  const auto entity = registry.create();

  RD_ASSERT_EQ(traits_type::combine(rendu::null, rendu::to_integral(entity)),
            (traits_type::construct(rendu::to_entity(null), rendu::to_version(entity))));
  RD_ASSERT_EQ(traits_type::combine(rendu::null, rendu::to_integral(null)), null);
  RD_ASSERT_EQ(traits_type::combine(rendu::null, rendu::tombstone), null);

  registry.emplace<int>(entity, 42);

  RD_ASSERT_FALSE(entity == rendu::null);
  RD_ASSERT_FALSE(rendu::null == entity);

  RD_ASSERT_TRUE(entity != rendu::null);
  RD_ASSERT_TRUE(rendu::null != entity);

  const rendu::entity other = rendu::null;

  RD_ASSERT_FALSE(registry.valid(other));
  RD_ASSERT_NE(registry.create(other), other);
}

RD_TEST(Entity, Tombstone) {
  using traits_type = rendu::entity_traits<rendu::entity>;
  constexpr rendu::entity tombstone = rendu::tombstone;

  RD_ASSERT_FALSE(rendu::entity{} == rendu::tombstone);
  RD_ASSERT_TRUE(rendu::tombstone == rendu::tombstone);
  RD_ASSERT_FALSE(rendu::tombstone != rendu::tombstone);

  rendu::registry registry{};
  const auto entity = registry.create();

  RD_ASSERT_EQ(traits_type::combine(rendu::to_integral(entity), rendu::tombstone),
            (traits_type::construct(rendu::to_entity(entity), rendu::to_version(tombstone))));
  RD_ASSERT_EQ(traits_type::combine(rendu::tombstone, rendu::to_integral(tombstone)), tombstone);
  RD_ASSERT_EQ(traits_type::combine(rendu::tombstone, rendu::null), tombstone);

  registry.emplace<int>(entity, 42);

  RD_ASSERT_FALSE(entity == rendu::tombstone);
  RD_ASSERT_FALSE(rendu::tombstone == entity);

  RD_ASSERT_TRUE(entity != rendu::tombstone);
  RD_ASSERT_TRUE(rendu::tombstone != entity);

  constexpr auto vers = rendu::to_version(tombstone);
  const auto other = traits_type::construct(rendu::to_entity(entity), vers);

  RD_ASSERT_FALSE(registry.valid(rendu::tombstone));
  RD_ASSERT_NE(registry.destroy(entity, vers), vers);
  RD_ASSERT_NE(registry.create(other), other);
}
