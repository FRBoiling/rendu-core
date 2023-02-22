/*
* Created by boil on 2023/2/22.
*/

#include <test/rdtest.h>
#include <core/base/enum.h>


enum class detected {
  foo = 0x01,
  bar = 0x02,
  quux = 0x04,
  _rendu_enum_as_bitmask
};

// small type on purpose
enum class registered : std::uint8_t {
  foo = 0x01,
  bar = 0x02,
  quux = 0x04
};

template<>
struct rendu::enum_as_bitmask<registered>
    : std::true_type {};

template<typename Type>
struct Enum: testing::Test {
  using type = Type;
};

using EnumTypes = ::testing::Types<detected, registered>;

RD_TYPED_TEST_SUITE(Enum, EnumTypes, );

RD_TYPED_TEST(Enum, Functionalities) {
  using enum_type = typename TestFixture::type;

  RD_ASSERT_TRUE(!!((enum_type::foo | enum_type::bar) & enum_type::foo));
  RD_ASSERT_TRUE(!!((enum_type::foo | enum_type::bar) & enum_type::bar));
  RD_ASSERT_TRUE(!((enum_type::foo | enum_type::bar) & enum_type::quux));

  RD_ASSERT_TRUE(!!((enum_type::foo ^ enum_type::bar) & enum_type::foo));
  RD_ASSERT_TRUE(!((enum_type::foo ^ enum_type::foo) & enum_type::foo));

  RD_ASSERT_TRUE(!(~enum_type::foo & enum_type::foo));
  RD_ASSERT_TRUE(!!(~enum_type::foo & enum_type::bar));

  RD_ASSERT_TRUE(enum_type::foo == enum_type::foo);
  RD_ASSERT_TRUE(enum_type::foo != enum_type::bar);

  enum_type value = enum_type::foo;

  RD_ASSERT_TRUE(!!(value & enum_type::foo));
  RD_ASSERT_TRUE(!(value & enum_type::bar));
  RD_ASSERT_TRUE(!(value & enum_type::quux));

  value |= (enum_type::bar | enum_type::quux);

  RD_ASSERT_TRUE(!!(value & enum_type::foo));
  RD_ASSERT_TRUE(!!(value & enum_type::bar));
  RD_ASSERT_TRUE(!!(value & enum_type::quux));

  value &= (enum_type::bar | enum_type::quux);

  RD_ASSERT_TRUE(!(value & enum_type::foo));
  RD_ASSERT_TRUE(!!(value & enum_type::bar));
  RD_ASSERT_TRUE(!!(value & enum_type::quux));

  value ^= enum_type::bar;

  RD_ASSERT_TRUE(!(value & enum_type::foo));
  RD_ASSERT_TRUE(!(value & enum_type::bar));
  RD_ASSERT_TRUE(!!(value & enum_type::quux));
}

