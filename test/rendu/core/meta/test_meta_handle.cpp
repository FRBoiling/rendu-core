/*
* Created by boil on 2023/2/27.
*/
#include <test/rdtest.h>
#include <core/base/hashed_string.h>
#include <core/meta/factory.h>
#include <core/meta/meta.h>

namespace meta_handle{

struct clazz_t {
  clazz_t()
      : value{} {}

  void incr() {
    ++value;
  }

  void decr() {
    --value;
  }

  int value;
};

struct MetaHandle: ::testing::Test {
  void SetUp() override {
    using namespace rendu::literals;

    rendu::meta<clazz_t>()
        .type("clazz"_hs)
        .func<&clazz_t::incr>("incr"_hs)
        .func<&clazz_t::decr>("decr"_hs);
  }

  void TearDown() override {
    rendu::meta_reset();
  }
};

RD_TEST_F(MetaHandle, Functionalities) {
  using namespace rendu::literals;

  clazz_t instance{};
  rendu::meta_handle handle{};
  rendu::meta_handle chandle{};

  RD_ASSERT_FALSE(handle);
  RD_ASSERT_FALSE(chandle);

  handle = rendu::meta_handle{instance};
  chandle = rendu::meta_handle{std::as_const(instance)};

  RD_ASSERT_TRUE(handle);
  RD_ASSERT_TRUE(chandle);

  RD_ASSERT_TRUE(handle->invoke("incr"_hs));
  RD_ASSERT_FALSE(chandle->invoke("incr"_hs));
  RD_ASSERT_FALSE(std::as_const(handle)->invoke("incr"_hs));
  RD_ASSERT_EQ(instance.value, 1);

  auto any = rendu::forward_as_meta(instance);
  handle = rendu::meta_handle{any};
  chandle = rendu::meta_handle{std::as_const(any)};

  RD_ASSERT_TRUE(handle->invoke("decr"_hs));
  RD_ASSERT_FALSE(chandle->invoke("decr"_hs));
  RD_ASSERT_FALSE(std::as_const(handle)->invoke("decr"_hs));
  RD_ASSERT_EQ(instance.value, 0);
}
}
