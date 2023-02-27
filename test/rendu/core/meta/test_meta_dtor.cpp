/*
* Created by boil on 2023/2/27.
*/
#include <test/rdtest.h>
#include <core/base/hashed_string.h>
#include <core/locator/locator.h>
#include <core/meta/factory.h>
#include <core/meta/meta.h>
#include <core/meta/node.h>
#include <core/meta/resolve.h>

namespace meta_dtor {

struct clazz_t {
  clazz_t() {
    ++counter;
  }

  static void destroy_decr(clazz_t &) {
    --counter;
  }

  void destroy_incr() const {
    ++counter;
  }

  inline static int counter = 0;
};

struct MetaDtor : ::testing::Test {
  void SetUp() override {
    using namespace rendu::literals;

    rendu::meta<clazz_t>()
        .type("clazz"_hs)
        .dtor<clazz_t::destroy_decr>();

    clazz_t::counter = 0;
  }

  void TearDown() override {
    rendu::meta_reset();
  }
};

RD_TEST_F(MetaDtor, Functionalities) {
  RD_ASSERT_EQ(clazz_t::counter, 0);

  auto any = rendu::resolve<clazz_t>().construct();
  auto cref = std::as_const(any).as_ref();
  auto ref = any.as_ref();

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(cref);
  RD_ASSERT_TRUE(ref);

  RD_ASSERT_EQ(clazz_t::counter, 1);

  cref.reset();
  ref.reset();

  RD_ASSERT_TRUE(any);
  RD_ASSERT_FALSE(cref);
  RD_ASSERT_FALSE(ref);

  RD_ASSERT_EQ(clazz_t::counter, 1);

  any.reset();

  RD_ASSERT_FALSE(any);
  RD_ASSERT_FALSE(cref);
  RD_ASSERT_FALSE(ref);

  RD_ASSERT_EQ(clazz_t::counter, 0);
}

RD_TEST_F(MetaDtor, AsRefConstruction) {
  RD_ASSERT_EQ(clazz_t::counter, 0);

  clazz_t instance{};
  auto any = rendu::forward_as_meta(instance);
  auto cany = rendu::forward_as_meta(std::as_const(instance));
  auto cref = cany.as_ref();
  auto ref = any.as_ref();

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(cany);
  RD_ASSERT_TRUE(cref);
  RD_ASSERT_TRUE(ref);

  RD_ASSERT_EQ(clazz_t::counter, 1);

  any.reset();
  cany.reset();
  cref.reset();
  ref.reset();

  RD_ASSERT_FALSE(any);
  RD_ASSERT_FALSE(cany);
  RD_ASSERT_FALSE(cref);
  RD_ASSERT_FALSE(ref);

  RD_ASSERT_EQ(clazz_t::counter, 1);
}

RD_TEST_F(MetaDtor, ReRegistration) {
  SetUp();

  auto &&node =
      rendu::internal::resolve<clazz_t>(rendu::internal::meta_context::from(rendu::locator<rendu::meta_ctx>::value_or()));

  RD_ASSERT_NE(node.dtor.dtor, nullptr);

  rendu::meta<clazz_t>().dtor<&clazz_t::destroy_incr>();
  rendu::resolve<clazz_t>().construct().reset();

  RD_ASSERT_EQ(clazz_t::counter, 2);
}
}
