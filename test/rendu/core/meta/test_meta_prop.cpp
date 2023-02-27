/*
* Created by boil on 2023/2/27.
*/
#include <test/rdtest.h>
#include <core/base/hashed_string.h>
#include <core/locator/locator.h>
#include <core/meta/factory.h>
#include <core/meta/meta.h>
#include <core/meta/resolve.h>

namespace meta_prop {

struct base_1_t {};
struct base_2_t {};
struct base_3_t {};
struct derived_t : base_1_t, base_2_t, base_3_t {};

struct MetaProp : ::testing::Test {
  void SetUp() override {
    using namespace rendu::literals;

    rendu::meta<base_1_t>()
        .type("base_1"_hs)
        .prop("int"_hs, 42);

    rendu::meta<base_2_t>()
        .type("base_2"_hs)
        .prop("bool"_hs, false)
        .prop("char[]"_hs, "char[]");

    rendu::meta<base_3_t>()
        .type("base_3"_hs)
        .prop("key_only"_hs)
        .prop("key"_hs, 42);

    rendu::meta<derived_t>()
        .type("derived"_hs)
        .base<base_1_t>()
        .base<base_2_t>()
        .base<base_3_t>();
  }

  void TearDown() override {
    rendu::meta_reset();
  }
};

RD_TEST_F(MetaProp, Functionalities) {
  using namespace rendu::literals;

  auto prop = rendu::resolve<base_1_t>().prop("int"_hs);

  RD_ASSERT_TRUE(prop);
  RD_ASSERT_EQ(prop.value(), 42);
}

RD_TEST_F(MetaProp, FromBase) {
  using namespace rendu::literals;

  auto type = rendu::resolve<derived_t>();
  auto prop_bool = type.prop("bool"_hs);
  auto prop_int = type.prop("int"_hs);
  auto key_only = type.prop("key_only"_hs);
  auto key_value = type.prop("key"_hs);

  RD_ASSERT_TRUE(prop_bool);
  RD_ASSERT_TRUE(prop_int);
  RD_ASSERT_TRUE(key_only);
  RD_ASSERT_TRUE(key_value);

  RD_ASSERT_FALSE(prop_bool.value().cast<bool>());
  RD_ASSERT_EQ(prop_int.value().cast<int>(), 42);
  RD_ASSERT_FALSE(key_only.value());
  RD_ASSERT_EQ(key_value.value().cast<int>(), 42);
}

RD_TEST_F(MetaProp, DeducedArrayType) {
  using namespace rendu::literals;

  auto prop = rendu::resolve<base_2_t>().prop("char[]"_hs);

  RD_ASSERT_TRUE(prop);
  RD_ASSERT_EQ(prop.value().type(), rendu::resolve<const char *>());
  RD_ASSERT_EQ(strcmp(prop.value().cast<const char *>(), "char[]"), 0);
}

RD_TEST_F(MetaProp, ReRegistration) {
  using namespace rendu::literals;

  SetUp();

  auto &&node =
      rendu::internal::resolve<base_1_t>(rendu::internal::meta_context::from(rendu::locator<rendu::meta_ctx>::value_or()));
  auto type = rendu::resolve<base_1_t>();

  RD_ASSERT_TRUE(node.details);
  RD_ASSERT_FALSE(node.details->prop.empty());
  RD_ASSERT_EQ(node.details->prop.size(), 1u);

  RD_ASSERT_TRUE(type.prop("int"_hs));
  RD_ASSERT_EQ(type.prop("int"_hs).value().cast<int>(), 42);

  rendu::meta<base_1_t>().prop("int"_hs, 0);
  rendu::meta<base_1_t>().prop("double"_hs, 3.);

  RD_ASSERT_TRUE(node.details);
  RD_ASSERT_FALSE(node.details->prop.empty());
  RD_ASSERT_EQ(node.details->prop.size(), 2u);

  RD_ASSERT_TRUE(type.prop("int"_hs));
  RD_ASSERT_TRUE(type.prop("double"_hs));
  RD_ASSERT_EQ(type.prop("int"_hs).value().cast<int>(), 0);
  RD_ASSERT_EQ(type.prop("double"_hs).value().cast<double>(), 3.);
}
}
