/*
* Created by boil on 2023/2/27.
*/

#include <test/rdtest.h>
#include <core/base/hashed_string.h>
#include <core/meta/factory.h>
#include <core/meta/meta.h>
#include <core/meta/resolve.h>

namespace meta_any {

struct clazz_t {
  clazz_t()
      : value{0} {}

  void member(int i) {
    value = i;
  }

  static void func() {
    c = 'd';
  }

  operator int() const {
    return value;
  }

  static inline char c = 'c';
  int value;
};

struct empty_t {
  virtual ~empty_t() {
    ++destructor_counter;
  }

  static void destroy(empty_t &) {
    ++destroy_counter;
  }

  inline static int destroy_counter = 0;
  inline static int destructor_counter = 0;
};

struct fat_t : empty_t {
  fat_t()
      : value{.0, .0, .0, .0} {}

  fat_t(double v1, double v2, double v3, double v4)
      : value{v1, v2, v3, v4} {}

  bool operator==(const fat_t &other) const {
    return std::equal(std::begin(value), std::end(value), std::begin(other.value), std::end(other.value));
  }

  double value[4];
};

enum class enum_class : unsigned short int {
  foo = 0u,
  bar = 42u
};

struct not_comparable_t {
  bool operator==(const not_comparable_t &) const = delete;
};

struct unmanageable_t {
  unmanageable_t() = default;
  unmanageable_t(const unmanageable_t &) = delete;
  unmanageable_t(unmanageable_t &&) = delete;
  unmanageable_t &operator=(const unmanageable_t &) = delete;
  unmanageable_t &operator=(unmanageable_t &&) = delete;
};

struct MetaAny : ::testing::Test {
  void SetUp() override {
    using namespace rendu::literals;

    rendu::meta<double>()
        .type("double"_hs);

    rendu::meta<empty_t>()
        .type("empty"_hs)
        .dtor<empty_t::destroy>();

    rendu::meta<fat_t>()
        .type("fat"_hs)
        .base<empty_t>()
        .dtor<fat_t::destroy>();

    rendu::meta<clazz_t>()
        .type("clazz"_hs)
        .data<&clazz_t::value>("value"_hs)
        .func<&clazz_t::member>("member"_hs)
        .func<clazz_t::func>("func"_hs)
        .conv<int>();

    empty_t::destroy_counter = 0;
    empty_t::destructor_counter = 0;
  }

  void TearDown() override {
    rendu::meta_reset();
  }
};

using MetaAnyDeathTest = MetaAny;

RD_TEST_F(MetaAny, SBO) {
  rendu::meta_any any{'c'};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_FALSE(any.try_cast<std::size_t>());
  RD_ASSERT_EQ(any.cast<char>(), 'c');
  RD_ASSERT_NE(any.data(), nullptr);
  RD_ASSERT_EQ(any, rendu::meta_any{'c'});
  RD_ASSERT_NE(rendu::meta_any{'h'}, any);
}

RD_TEST_F(MetaAny, NoSBO) {
  fat_t instance{.1, .2, .3, .4};
  rendu::meta_any any{instance};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_FALSE(any.try_cast<std::size_t>());
  RD_ASSERT_EQ(any.cast<fat_t>(), instance);
  RD_ASSERT_NE(any.data(), nullptr);
  RD_ASSERT_EQ(any, rendu::meta_any{instance});
  RD_ASSERT_NE(any, fat_t{});
}

RD_TEST_F(MetaAny, Empty) {
  rendu::meta_any any{};

  RD_ASSERT_FALSE(any);
  RD_ASSERT_FALSE(any.type());
  RD_ASSERT_FALSE(any.try_cast<std::size_t>());
  RD_ASSERT_EQ(any.data(), nullptr);
  RD_ASSERT_EQ(any, rendu::meta_any{});
  RD_ASSERT_NE(rendu::meta_any{'c'}, any);

  RD_ASSERT_FALSE(any.as_ref());
  RD_ASSERT_FALSE(any.as_sequence_container());
  RD_ASSERT_FALSE(any.as_associative_container());

  RD_ASSERT_FALSE(std::as_const(any).as_ref());
  RD_ASSERT_FALSE(std::as_const(any).as_sequence_container());
  RD_ASSERT_FALSE(std::as_const(any).as_associative_container());
}

RD_TEST_F(MetaAny, SBOInPlaceTypeConstruction) {
  rendu::meta_any any{std::in_place_type<int>, 42};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_FALSE(any.try_cast<std::size_t>());
  RD_ASSERT_EQ(any.cast<int>(), 42);
  RD_ASSERT_NE(any.data(), nullptr);
  RD_ASSERT_EQ(any, (rendu::meta_any{std::in_place_type<int>, 42}));
  RD_ASSERT_EQ(any, rendu::meta_any{42});
  RD_ASSERT_NE(rendu::meta_any{3}, any);
}

RD_TEST_F(MetaAny, SBOAsRefConstruction) {
  int value = 3;
  int compare = 42;
  auto any = rendu::forward_as_meta(value);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_FALSE(any.owner());
  RD_ASSERT_EQ(any.type(), rendu::resolve<int>());

  RD_ASSERT_FALSE(any.try_cast<std::size_t>());
  RD_ASSERT_EQ(any.cast<int &>(), 3);
  RD_ASSERT_EQ(any.cast<const int &>(), 3);
  RD_ASSERT_EQ(any.cast<int>(), 3);
  RD_ASSERT_EQ(any.data(), &value);
  RD_ASSERT_EQ(std::as_const(any).data(), &value);

  RD_ASSERT_EQ(any, rendu::forward_as_meta(value));
  RD_ASSERT_NE(any, rendu::forward_as_meta(compare));

  RD_ASSERT_NE(any, rendu::meta_any{42});
  RD_ASSERT_EQ(rendu::meta_any{3}, any);

  any = rendu::forward_as_meta(value);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.type(), rendu::resolve<int>());
  RD_ASSERT_EQ(std::as_const(any).data(), &value);

  auto other = any.as_ref();

  RD_ASSERT_TRUE(other);
  RD_ASSERT_EQ(any.type(), rendu::resolve<int>());
  RD_ASSERT_EQ(any.cast<int>(), 3);
  RD_ASSERT_EQ(other.data(), any.data());
}

RD_TEST_F(MetaAny, SBOAsConstRefConstruction) {
  const int value = 3;
  int compare = 42;
  auto any = rendu::forward_as_meta(value);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_FALSE(any.owner());
  RD_ASSERT_EQ(any.type(), rendu::resolve<int>());

  RD_ASSERT_FALSE(any.try_cast<std::size_t>());
  RD_ASSERT_EQ(any.cast<const int &>(), 3);
  RD_ASSERT_EQ(any.cast<int>(), 3);
  RD_ASSERT_EQ(any.data(), nullptr);
  RD_ASSERT_EQ(std::as_const(any).data(), &value);

  RD_ASSERT_EQ(any, rendu::forward_as_meta(value));
  RD_ASSERT_NE(any, rendu::forward_as_meta(compare));

  RD_ASSERT_NE(any, rendu::meta_any{42});
  RD_ASSERT_EQ(rendu::meta_any{3}, any);

  any = rendu::forward_as_meta(std::as_const(value));

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.type(), rendu::resolve<int>());
  RD_ASSERT_EQ(std::as_const(any).data(), &value);

  auto other = any.as_ref();

  RD_ASSERT_TRUE(other);
  RD_ASSERT_EQ(any.type(), rendu::resolve<int>());
  RD_ASSERT_EQ(any.cast<int>(), 3);
  RD_ASSERT_EQ(other.data(), any.data());
}

RD_DEBUG_TEST_F(MetaAnyDeathTest, SBOAsConstRefConstruction) {
  const int value = 3;
  auto any = rendu::forward_as_meta(value);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_DEATH(any.cast<int &>() = 3, "");
}

RD_TEST_F(MetaAny, SBOCopyConstruction) {
  rendu::meta_any any{42};
  rendu::meta_any other{any};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(other);
  RD_ASSERT_FALSE(other.try_cast<std::size_t>());
  RD_ASSERT_EQ(other.cast<int>(), 42);
  RD_ASSERT_EQ(other, rendu::meta_any{42});
  RD_ASSERT_NE(other, rendu::meta_any{0});
}

RD_TEST_F(MetaAny, SBOCopyAssignment) {
  rendu::meta_any any{42};
  rendu::meta_any other{3};

  other = any;

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(other);
  RD_ASSERT_FALSE(other.try_cast<std::size_t>());
  RD_ASSERT_EQ(other.cast<int>(), 42);
  RD_ASSERT_EQ(other, rendu::meta_any{42});
  RD_ASSERT_NE(other, rendu::meta_any{0});
}

RD_TEST_F(MetaAny, SBOMoveConstruction) {
  rendu::meta_any any{42};
  rendu::meta_any other{std::move(any)};

  RD_ASSERT_FALSE(any);
  RD_ASSERT_TRUE(other);
  RD_ASSERT_FALSE(other.try_cast<std::size_t>());
  RD_ASSERT_EQ(other.cast<int>(), 42);
  RD_ASSERT_EQ(other, rendu::meta_any{42});
  RD_ASSERT_NE(other, rendu::meta_any{0});
}

RD_TEST_F(MetaAny, SBOMoveAssignment) {
  rendu::meta_any any{42};
  rendu::meta_any other{3};

  other = std::move(any);

  RD_ASSERT_FALSE(any);
  RD_ASSERT_TRUE(other);
  RD_ASSERT_FALSE(other.try_cast<std::size_t>());
  RD_ASSERT_EQ(other.cast<int>(), 42);
  RD_ASSERT_EQ(other, rendu::meta_any{42});
  RD_ASSERT_NE(other, rendu::meta_any{0});
}

RD_TEST_F(MetaAny, SBODirectAssignment) {
  rendu::meta_any any{};
  any = 42;

  RD_ASSERT_FALSE(any.try_cast<std::size_t>());
  RD_ASSERT_EQ(any.cast<int>(), 42);
  RD_ASSERT_EQ(any, rendu::meta_any{42});
  RD_ASSERT_NE(rendu::meta_any{0}, any);
}

RD_TEST_F(MetaAny, SBOAssignValue) {
  rendu::meta_any any{42};
  rendu::meta_any other{3};
  rendu::meta_any invalid{empty_t{}};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<int>(), 42);

  RD_ASSERT_TRUE(any.assign(other));
  RD_ASSERT_FALSE(any.assign(invalid));
  RD_ASSERT_EQ(any.cast<int>(), 3);
}

RD_TEST_F(MetaAny, SBOConvertAssignValue) {
  rendu::meta_any any{42};
  rendu::meta_any other{3.5};
  rendu::meta_any invalid{empty_t{}};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<int>(), 42);

  RD_ASSERT_TRUE(any.assign(other));
  RD_ASSERT_FALSE(any.assign(invalid));
  RD_ASSERT_EQ(any.cast<int>(), 3);
}

RD_TEST_F(MetaAny, SBOAsRefAssignValue) {
  int value = 42;
  rendu::meta_any any{rendu::forward_as_meta(value)};
  rendu::meta_any other{3};
  rendu::meta_any invalid{empty_t{}};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<int>(), 42);

  RD_ASSERT_TRUE(any.assign(other));
  RD_ASSERT_FALSE(any.assign(invalid));
  RD_ASSERT_EQ(any.cast<int>(), 3);
  RD_ASSERT_EQ(value, 3);
}

RD_TEST_F(MetaAny, SBOAsConstRefAssignValue) {
  const int value = 42;
  rendu::meta_any any{rendu::forward_as_meta(value)};
  rendu::meta_any other{3};
  rendu::meta_any invalid{empty_t{}};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<int>(), 42);

  RD_ASSERT_FALSE(any.assign(other));
  RD_ASSERT_FALSE(any.assign(invalid));
  RD_ASSERT_EQ(any.cast<int>(), 42);
  RD_ASSERT_EQ(value, 42);
}

RD_TEST_F(MetaAny, SBOTransferValue) {
  rendu::meta_any any{42};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<int>(), 42);

  RD_ASSERT_TRUE(any.assign(3));
  RD_ASSERT_FALSE(any.assign(empty_t{}));
  RD_ASSERT_EQ(any.cast<int>(), 3);
}

RD_TEST_F(MetaAny, SBOTransferConstValue) {
  const int value = 3;
  rendu::meta_any any{42};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<int>(), 42);

  RD_ASSERT_TRUE(any.assign(rendu::forward_as_meta(value)));
  RD_ASSERT_EQ(any.cast<int>(), 3);
}

RD_TEST_F(MetaAny, SBOConvertTransferValue) {
  rendu::meta_any any{42};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<int>(), 42);

  RD_ASSERT_TRUE(any.assign(3.5));
  RD_ASSERT_FALSE(any.assign(empty_t{}));
  RD_ASSERT_EQ(any.cast<int>(), 3);
}

RD_TEST_F(MetaAny, SBOAsRefTransferValue) {
  int value = 42;
  rendu::meta_any any{rendu::forward_as_meta(value)};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<int>(), 42);

  RD_ASSERT_TRUE(any.assign(3));
  RD_ASSERT_FALSE(any.assign(empty_t{}));
  RD_ASSERT_EQ(any.cast<int>(), 3);
  RD_ASSERT_EQ(value, 3);
}

RD_TEST_F(MetaAny, SBOAsConstRefTransferValue) {
  const int value = 42;
  rendu::meta_any any{rendu::forward_as_meta(value)};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<int>(), 42);

  RD_ASSERT_FALSE(any.assign(3));
  RD_ASSERT_FALSE(any.assign(empty_t{}));
  RD_ASSERT_EQ(any.cast<int>(), 42);
  RD_ASSERT_EQ(value, 42);
}

RD_TEST_F(MetaAny, NoSBOInPlaceTypeConstruction) {
  fat_t instance{.1, .2, .3, .4};
  rendu::meta_any any{std::in_place_type<fat_t>, instance};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_FALSE(any.try_cast<std::size_t>());
  RD_ASSERT_EQ(any.cast<fat_t>(), instance);
  RD_ASSERT_NE(any.data(), nullptr);
  RD_ASSERT_EQ(any, (rendu::meta_any{std::in_place_type<fat_t>, instance}));
  RD_ASSERT_EQ(any, rendu::meta_any{instance});
  RD_ASSERT_NE(rendu::meta_any{fat_t{}}, any);
}

RD_TEST_F(MetaAny, NoSBOAsRefConstruction) {
  fat_t instance{.1, .2, .3, .4};
  auto any = rendu::forward_as_meta(instance);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_FALSE(any.owner());
  RD_ASSERT_EQ(any.type(), rendu::resolve<fat_t>());

  RD_ASSERT_FALSE(any.try_cast<std::size_t>());
  RD_ASSERT_EQ(any.cast<fat_t &>(), instance);
  RD_ASSERT_EQ(any.cast<const fat_t &>(), instance);
  RD_ASSERT_EQ(any.cast<fat_t>(), instance);
  RD_ASSERT_EQ(any.data(), &instance);
  RD_ASSERT_EQ(std::as_const(any).data(), &instance);

  RD_ASSERT_EQ(any, rendu::forward_as_meta(instance));

  RD_ASSERT_EQ(any, rendu::meta_any{instance});
  RD_ASSERT_NE(rendu::meta_any{fat_t{}}, any);

  any = rendu::forward_as_meta(instance);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.type(), rendu::resolve<fat_t>());
  RD_ASSERT_EQ(std::as_const(any).data(), &instance);

  auto other = any.as_ref();

  RD_ASSERT_TRUE(other);
  RD_ASSERT_EQ(any.type(), rendu::resolve<fat_t>());
  RD_ASSERT_EQ(any, rendu::meta_any{instance});
  RD_ASSERT_EQ(other.data(), any.data());
}

RD_TEST_F(MetaAny, NoSBOAsConstRefConstruction) {
  const fat_t instance{.1, .2, .3, .4};
  auto any = rendu::forward_as_meta(instance);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_FALSE(any.owner());
  RD_ASSERT_EQ(any.type(), rendu::resolve<fat_t>());

  RD_ASSERT_FALSE(any.try_cast<std::size_t>());
  RD_ASSERT_EQ(any.cast<const fat_t &>(), instance);
  RD_ASSERT_EQ(any.cast<fat_t>(), instance);
  RD_ASSERT_EQ(any.data(), nullptr);
  RD_ASSERT_EQ(std::as_const(any).data(), &instance);

  RD_ASSERT_EQ(any, rendu::forward_as_meta(instance));

  RD_ASSERT_EQ(any, rendu::meta_any{instance});
  RD_ASSERT_NE(rendu::meta_any{fat_t{}}, any);

  any = rendu::forward_as_meta(std::as_const(instance));

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.type(), rendu::resolve<fat_t>());
  RD_ASSERT_EQ(std::as_const(any).data(), &instance);

  auto other = any.as_ref();

  RD_ASSERT_TRUE(other);
  RD_ASSERT_EQ(any.type(), rendu::resolve<fat_t>());
  RD_ASSERT_EQ(any, rendu::meta_any{instance});
  RD_ASSERT_EQ(other.data(), any.data());
}

RD_DEBUG_TEST_F(MetaAnyDeathTest, NoSBOAsConstRefConstruction) {
  const fat_t instance{.1, .2, .3, .4};
  auto any = rendu::forward_as_meta(instance);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_DEATH(any.cast<fat_t &>() = {}, "");
}

RD_TEST_F(MetaAny, NoSBOCopyConstruction) {
  fat_t instance{.1, .2, .3, .4};
  rendu::meta_any any{instance};
  rendu::meta_any other{any};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(other);
  RD_ASSERT_FALSE(other.try_cast<std::size_t>());
  RD_ASSERT_EQ(other.cast<fat_t>(), instance);
  RD_ASSERT_EQ(other, rendu::meta_any{instance});
  RD_ASSERT_NE(other, fat_t{});
}

RD_TEST_F(MetaAny, NoSBOCopyAssignment) {
  fat_t instance{.1, .2, .3, .4};
  rendu::meta_any any{instance};
  rendu::meta_any other{3};

  other = any;

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(other);
  RD_ASSERT_FALSE(other.try_cast<std::size_t>());
  RD_ASSERT_EQ(other.cast<fat_t>(), instance);
  RD_ASSERT_EQ(other, rendu::meta_any{instance});
  RD_ASSERT_NE(other, fat_t{});
}

RD_TEST_F(MetaAny, NoSBOMoveConstruction) {
  fat_t instance{.1, .2, .3, .4};
  rendu::meta_any any{instance};
  rendu::meta_any other{std::move(any)};

  RD_ASSERT_FALSE(any);
  RD_ASSERT_TRUE(other);
  RD_ASSERT_FALSE(other.try_cast<std::size_t>());
  RD_ASSERT_EQ(other.cast<fat_t>(), instance);
  RD_ASSERT_EQ(other, rendu::meta_any{instance});
  RD_ASSERT_NE(other, fat_t{});
}

RD_TEST_F(MetaAny, NoSBOMoveAssignment) {
  fat_t instance{.1, .2, .3, .4};
  rendu::meta_any any{instance};
  rendu::meta_any other{3};

  other = std::move(any);

  RD_ASSERT_FALSE(any);
  RD_ASSERT_TRUE(other);
  RD_ASSERT_FALSE(other.try_cast<std::size_t>());
  RD_ASSERT_EQ(other.cast<fat_t>(), instance);
  RD_ASSERT_EQ(other, rendu::meta_any{instance});
  RD_ASSERT_NE(other, fat_t{});
}

RD_TEST_F(MetaAny, NoSBODirectAssignment) {
  fat_t instance{.1, .2, .3, .4};
  rendu::meta_any any{};
  any = instance;

  RD_ASSERT_FALSE(any.try_cast<std::size_t>());
  RD_ASSERT_EQ(any.cast<fat_t>(), instance);
  RD_ASSERT_EQ(any, (rendu::meta_any{fat_t{.1, .2, .3, .4}}));
  RD_ASSERT_NE(any, fat_t{});
}

RD_TEST_F(MetaAny, NoSBOAssignValue) {
  rendu::meta_any any{fat_t{.1, .2, .3, .4}};
  rendu::meta_any other{fat_t{.0, .1, .2, .3}};
  rendu::meta_any invalid{'c'};

  const void *addr = std::as_const(any).data();

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<const fat_t &>(), (fat_t{.1, .2, .3, .4}));

  RD_ASSERT_TRUE(any.assign(other));
  RD_ASSERT_FALSE(any.assign(invalid));
  RD_ASSERT_EQ(any.cast<const fat_t &>(), (fat_t{.0, .1, .2, .3}));
  RD_ASSERT_EQ(addr, std::as_const(any).data());
}

RD_TEST_F(MetaAny, NoSBOConvertAssignValue) {
  rendu::meta_any any{empty_t{}};
  rendu::meta_any other{fat_t{.0, .1, .2, .3}};
  rendu::meta_any invalid{'c'};

  const void *addr = std::as_const(any).data();

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(any.assign(other));
  RD_ASSERT_FALSE(any.assign(invalid));
  RD_ASSERT_EQ(addr, std::as_const(any).data());
}

RD_TEST_F(MetaAny, NoSBOAsRefAssignValue) {
  fat_t instance{.1, .2, .3, .4};
  rendu::meta_any any{rendu::forward_as_meta(instance)};
  rendu::meta_any other{fat_t{.0, .1, .2, .3}};
  rendu::meta_any invalid{'c'};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<const fat_t &>(), (fat_t{.1, .2, .3, .4}));

  RD_ASSERT_TRUE(any.assign(other));
  RD_ASSERT_FALSE(any.assign(invalid));
  RD_ASSERT_EQ(any.cast<const fat_t &>(), (fat_t{.0, .1, .2, .3}));
  RD_ASSERT_EQ(instance, (fat_t{.0, .1, .2, .3}));
}

RD_TEST_F(MetaAny, NoSBOAsConstRefAssignValue) {
  const fat_t instance{.1, .2, .3, .4};
  rendu::meta_any any{rendu::forward_as_meta(instance)};
  rendu::meta_any other{fat_t{.0, .1, .2, .3}};
  rendu::meta_any invalid{'c'};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<const fat_t &>(), (fat_t{.1, .2, .3, .4}));

  RD_ASSERT_FALSE(any.assign(other));
  RD_ASSERT_FALSE(any.assign(invalid));
  RD_ASSERT_EQ(any.cast<const fat_t &>(), (fat_t{.1, .2, .3, .4}));
  RD_ASSERT_EQ(instance, (fat_t{.1, .2, .3, .4}));
}

RD_TEST_F(MetaAny, NoSBOTransferValue) {
  rendu::meta_any any{fat_t{.1, .2, .3, .4}};

  const void *addr = std::as_const(any).data();

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<const fat_t &>(), (fat_t{.1, .2, .3, .4}));

  RD_ASSERT_TRUE(any.assign(fat_t{.0, .1, .2, .3}));
  RD_ASSERT_FALSE(any.assign('c'));
  RD_ASSERT_EQ(any.cast<const fat_t &>(), (fat_t{.0, .1, .2, .3}));
  RD_ASSERT_EQ(addr, std::as_const(any).data());
}

RD_TEST_F(MetaAny, NoSBOTransferConstValue) {
  const fat_t instance{.0, .1, .2, .3};
  rendu::meta_any any{fat_t{.1, .2, .3, .4}};

  const void *addr = std::as_const(any).data();

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<const fat_t &>(), (fat_t{.1, .2, .3, .4}));

  RD_ASSERT_TRUE(any.assign(rendu::forward_as_meta(instance)));
  RD_ASSERT_EQ(any.cast<const fat_t &>(), (fat_t{.0, .1, .2, .3}));
  RD_ASSERT_EQ(addr, std::as_const(any).data());
}

RD_TEST_F(MetaAny, NoSBOConvertTransferValue) {
  rendu::meta_any any{empty_t{}};

  const void *addr = std::as_const(any).data();

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(any.assign(fat_t{.0, .1, .2, .3}));
  RD_ASSERT_FALSE(any.assign('c'));
  RD_ASSERT_EQ(addr, std::as_const(any).data());
}

RD_TEST_F(MetaAny, NoSBOAsRefTransferValue) {
  fat_t instance{.1, .2, .3, .4};
  rendu::meta_any any{rendu::forward_as_meta(instance)};

  const void *addr = std::as_const(any).data();

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<const fat_t &>(), (fat_t{.1, .2, .3, .4}));

  RD_ASSERT_TRUE(any.assign(fat_t{.0, .1, .2, .3}));
  RD_ASSERT_FALSE(any.assign('c'));
  RD_ASSERT_EQ(any.cast<const fat_t &>(), (fat_t{.0, .1, .2, .3}));
  RD_ASSERT_EQ(instance, (fat_t{.0, .1, .2, .3}));
  RD_ASSERT_EQ(addr, std::as_const(any).data());
}

RD_TEST_F(MetaAny, NoSBOAsConstRefTransferValue) {
  const fat_t instance{.1, .2, .3, .4};
  rendu::meta_any any{rendu::forward_as_meta(instance)};

  const void *addr = std::as_const(any).data();

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<const fat_t &>(), (fat_t{.1, .2, .3, .4}));

  RD_ASSERT_FALSE(any.assign(fat_t{.0, .1, .2, .3}));
  RD_ASSERT_FALSE(any.assign('c'));
  RD_ASSERT_EQ(any.cast<const fat_t &>(), (fat_t{.1, .2, .3, .4}));
  RD_ASSERT_EQ(instance, (fat_t{.1, .2, .3, .4}));
  RD_ASSERT_EQ(addr, std::as_const(any).data());
}

RD_TEST_F(MetaAny, VoidInPlaceTypeConstruction) {
  rendu::meta_any any{std::in_place_type<void>};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_FALSE(any.try_cast<char>());
  RD_ASSERT_EQ(any.data(), nullptr);
  RD_ASSERT_EQ(any.type(), rendu::resolve<void>());
  RD_ASSERT_EQ(any, rendu::meta_any{std::in_place_type<void>});
  RD_ASSERT_NE(rendu::meta_any{3}, any);
}

RD_TEST_F(MetaAny, VoidCopyConstruction) {
  rendu::meta_any any{std::in_place_type<void>};
  rendu::meta_any other{any};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(other);
  RD_ASSERT_EQ(any.type(), rendu::resolve<void>());
  RD_ASSERT_EQ(other, rendu::meta_any{std::in_place_type<void>});
}

RD_TEST_F(MetaAny, VoidCopyAssignment) {
  rendu::meta_any any{std::in_place_type<void>};
  rendu::meta_any other{std::in_place_type<void>};

  other = any;

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(other);
  RD_ASSERT_EQ(any.type(), rendu::resolve<void>());
  RD_ASSERT_EQ(other, rendu::meta_any{std::in_place_type<void>});
}

RD_TEST_F(MetaAny, VoidMoveConstruction) {
  rendu::meta_any any{std::in_place_type<void>};
  rendu::meta_any other{std::move(any)};

  RD_ASSERT_FALSE(any);
  RD_ASSERT_TRUE(other);
  RD_ASSERT_EQ(other.type(), rendu::resolve<void>());
  RD_ASSERT_EQ(other, rendu::meta_any{std::in_place_type<void>});
}

RD_TEST_F(MetaAny, VoidMoveAssignment) {
  rendu::meta_any any{std::in_place_type<void>};
  rendu::meta_any other{std::in_place_type<void>};

  other = std::move(any);

  RD_ASSERT_FALSE(any);
  RD_ASSERT_TRUE(other);
  RD_ASSERT_EQ(other.type(), rendu::resolve<void>());
  RD_ASSERT_EQ(other, rendu::meta_any{std::in_place_type<void>});
}

RD_TEST_F(MetaAny, SBOMoveInvalidate) {
  rendu::meta_any any{42};
  rendu::meta_any other{std::move(any)};
  rendu::meta_any valid = std::move(other);

  RD_ASSERT_FALSE(any);
  RD_ASSERT_FALSE(other);
  RD_ASSERT_TRUE(valid);
}

RD_TEST_F(MetaAny, NoSBOMoveInvalidate) {
  fat_t instance{.1, .2, .3, .4};
  rendu::meta_any any{instance};
  rendu::meta_any other{std::move(any)};
  rendu::meta_any valid = std::move(other);

  RD_ASSERT_FALSE(any);
  RD_ASSERT_FALSE(other);
  RD_ASSERT_TRUE(valid);
}

RD_TEST_F(MetaAny, VoidMoveInvalidate) {
  rendu::meta_any any{std::in_place_type<void>};
  rendu::meta_any other{std::move(any)};
  rendu::meta_any valid = std::move(other);

  RD_ASSERT_FALSE(any);
  RD_ASSERT_FALSE(other);
  RD_ASSERT_TRUE(valid);
}

RD_TEST_F(MetaAny, SBODestruction) {
  {
    rendu::meta_any any{std::in_place_type<empty_t>};
    any.emplace<empty_t>();
    any = empty_t{};
    rendu::meta_any other{std::move(any)};
    any = std::move(other);
  }

  RD_ASSERT_EQ(empty_t::destroy_counter, 3);
  RD_ASSERT_EQ(empty_t::destructor_counter, 6);
}

RD_TEST_F(MetaAny, NoSBODestruction) {
  {
    rendu::meta_any any{std::in_place_type<fat_t>, 1., 2., 3., 4.};
    any.emplace<fat_t>(1., 2., 3., 4.);
    any = fat_t{1., 2., 3., 4.};
    rendu::meta_any other{std::move(any)};
    any = std::move(other);
  }

  RD_ASSERT_EQ(fat_t::destroy_counter, 3);
  RD_ASSERT_EQ(empty_t::destructor_counter, 4);
}

RD_TEST_F(MetaAny, VoidDestruction) {
  // just let asan tell us if everything is ok here
  [[maybe_unused]] rendu::meta_any any{std::in_place_type<void>};
}

RD_TEST_F(MetaAny, Emplace) {
  rendu::meta_any any{};
  any.emplace<int>(42);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_FALSE(any.try_cast<std::size_t>());
  RD_ASSERT_EQ(any.cast<int>(), 42);
  RD_ASSERT_NE(any.data(), nullptr);
  RD_ASSERT_EQ(any, (rendu::meta_any{std::in_place_type<int>, 42}));
  RD_ASSERT_EQ(any, rendu::meta_any{42});
  RD_ASSERT_NE(rendu::meta_any{3}, any);
}

RD_TEST_F(MetaAny, EmplaceVoid) {
  rendu::meta_any any{};
  any.emplace<void>();

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.data(), nullptr);
  RD_ASSERT_EQ(any.type(), rendu::resolve<void>());
  RD_ASSERT_EQ(any, (rendu::meta_any{std::in_place_type<void>}));
}

RD_TEST_F(MetaAny, Reset) {
  rendu::meta_any any{42};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.type(), rendu::resolve<int>());

  any.reset();

  RD_ASSERT_FALSE(any);
  RD_ASSERT_EQ(any.type(), rendu::meta_type{});
}

RD_TEST_F(MetaAny, SBOSwap) {
  rendu::meta_any lhs{'c'};
  rendu::meta_any rhs{42};

  std::swap(lhs, rhs);

  RD_ASSERT_FALSE(lhs.try_cast<char>());
  RD_ASSERT_EQ(lhs.cast<int>(), 42);
  RD_ASSERT_FALSE(rhs.try_cast<int>());
  RD_ASSERT_EQ(rhs.cast<char>(), 'c');
}

RD_TEST_F(MetaAny, NoSBOSwap) {
  rendu::meta_any lhs{fat_t{.1, .2, .3, .4}};
  rendu::meta_any rhs{fat_t{.4, .3, .2, .1}};

  std::swap(lhs, rhs);

  RD_ASSERT_EQ(lhs.cast<fat_t>(), (fat_t{.4, .3, .2, .1}));
  RD_ASSERT_EQ(rhs.cast<fat_t>(), (fat_t{.1, .2, .3, .4}));
}

RD_TEST_F(MetaAny, VoidSwap) {
  rendu::meta_any lhs{std::in_place_type<void>};
  rendu::meta_any rhs{std::in_place_type<void>};
  const auto *pre = lhs.data();

  std::swap(lhs, rhs);

  RD_ASSERT_EQ(pre, lhs.data());
}

RD_TEST_F(MetaAny, SBOWithNoSBOSwap) {
  rendu::meta_any lhs{fat_t{.1, .2, .3, .4}};
  rendu::meta_any rhs{'c'};

  std::swap(lhs, rhs);

  RD_ASSERT_FALSE(lhs.try_cast<fat_t>());
  RD_ASSERT_EQ(lhs.cast<char>(), 'c');
  RD_ASSERT_FALSE(rhs.try_cast<char>());
  RD_ASSERT_EQ(rhs.cast<fat_t>(), (fat_t{.1, .2, .3, .4}));
}

RD_TEST_F(MetaAny, SBOWithEmptySwap) {
  rendu::meta_any lhs{'c'};
  rendu::meta_any rhs{};

  std::swap(lhs, rhs);

  RD_ASSERT_FALSE(lhs);
  RD_ASSERT_EQ(rhs.cast<char>(), 'c');

  std::swap(lhs, rhs);

  RD_ASSERT_FALSE(rhs);
  RD_ASSERT_EQ(lhs.cast<char>(), 'c');
}

RD_TEST_F(MetaAny, SBOWithVoidSwap) {
  rendu::meta_any lhs{'c'};
  rendu::meta_any rhs{std::in_place_type<void>};

  std::swap(lhs, rhs);

  RD_ASSERT_EQ(lhs.type(), rendu::resolve<void>());
  RD_ASSERT_EQ(rhs.cast<char>(), 'c');
}

RD_TEST_F(MetaAny, NoSBOWithEmptySwap) {
  rendu::meta_any lhs{fat_t{.1, .2, .3, .4}};
  rendu::meta_any rhs{};

  std::swap(lhs, rhs);

  RD_ASSERT_FALSE(lhs);
  RD_ASSERT_EQ(rhs.cast<fat_t>(), (fat_t{.1, .2, .3, .4}));

  std::swap(lhs, rhs);

  RD_ASSERT_FALSE(rhs);
  RD_ASSERT_EQ(lhs.cast<fat_t>(), (fat_t{.1, .2, .3, .4}));
}

RD_TEST_F(MetaAny, NoSBOWithVoidSwap) {
  rendu::meta_any lhs{fat_t{.1, .2, .3, .4}};
  rendu::meta_any rhs{std::in_place_type<void>};

  std::swap(lhs, rhs);

  RD_ASSERT_EQ(lhs.type(), rendu::resolve<void>());
  RD_ASSERT_EQ(rhs.cast<fat_t>(), (fat_t{.1, .2, .3, .4}));

  std::swap(lhs, rhs);

  RD_ASSERT_EQ(rhs.type(), rendu::resolve<void>());
  RD_ASSERT_EQ(lhs.cast<fat_t>(), (fat_t{.1, .2, .3, .4}));
}

RD_TEST_F(MetaAny, AsRef) {
  rendu::meta_any any{42};
  auto ref = any.as_ref();
  auto cref = std::as_const(any).as_ref();

  RD_ASSERT_EQ(any.try_cast<int>(), any.data());
  RD_ASSERT_EQ(ref.try_cast<int>(), any.data());
  RD_ASSERT_EQ(cref.try_cast<int>(), nullptr);

  RD_ASSERT_EQ(any.try_cast<const int>(), any.data());
  RD_ASSERT_EQ(ref.try_cast<const int>(), any.data());
  RD_ASSERT_EQ(cref.try_cast<const int>(), any.data());

  RD_ASSERT_EQ(any.cast<int>(), 42);
  RD_ASSERT_EQ(ref.cast<int>(), 42);
  RD_ASSERT_EQ(cref.cast<int>(), 42);

  RD_ASSERT_EQ(any.cast<const int>(), 42);
  RD_ASSERT_EQ(ref.cast<const int>(), 42);
  RD_ASSERT_EQ(cref.cast<const int>(), 42);

  RD_ASSERT_EQ(any.cast<int &>(), 42);
  RD_ASSERT_EQ(any.cast<const int &>(), 42);
  RD_ASSERT_EQ(ref.cast<int &>(), 42);
  RD_ASSERT_EQ(ref.cast<const int &>(), 42);
  RD_ASSERT_EQ(cref.cast<const int &>(), 42);

  any.cast<int &>() = 3;

  RD_ASSERT_EQ(any.cast<int>(), 3);
  RD_ASSERT_EQ(ref.cast<int>(), 3);
  RD_ASSERT_EQ(cref.cast<int>(), 3);

  std::swap(ref, cref);

  RD_ASSERT_EQ(ref.try_cast<int>(), nullptr);
  RD_ASSERT_EQ(cref.try_cast<int>(), any.data());

  ref = ref.as_ref();
  cref = std::as_const(cref).as_ref();

  RD_ASSERT_EQ(ref.try_cast<int>(), nullptr);
  RD_ASSERT_EQ(cref.try_cast<int>(), nullptr);
  RD_ASSERT_EQ(ref.try_cast<const int>(), any.data());
  RD_ASSERT_EQ(cref.try_cast<const int>(), any.data());

  RD_ASSERT_EQ(ref.cast<const int &>(), 3);
  RD_ASSERT_EQ(cref.cast<const int &>(), 3);

  ref = 42;
  cref = 42;

  RD_ASSERT_NE(ref.try_cast<int>(), nullptr);
  RD_ASSERT_NE(cref.try_cast<int>(), nullptr);
  RD_ASSERT_EQ(ref.cast<int &>(), 42);
  RD_ASSERT_EQ(cref.cast<int &>(), 42);
  RD_ASSERT_EQ(ref.cast<const int &>(), 42);
  RD_ASSERT_EQ(cref.cast<const int &>(), 42);
  RD_ASSERT_NE(ref.try_cast<int>(), any.data());
  RD_ASSERT_NE(cref.try_cast<int>(), any.data());

  any.emplace<void>();
  ref = any.as_ref();
  cref = std::as_const(any).as_ref();

  RD_ASSERT_TRUE(any);
  RD_ASSERT_FALSE(ref);
  RD_ASSERT_FALSE(cref);
}

RD_DEBUG_TEST_F(MetaAnyDeathTest, AsRef) {
  rendu::meta_any any{42};
  auto cref = std::as_const(any).as_ref();

  RD_ASSERT_TRUE(any);
  RD_ASSERT_DEATH(cref.cast<int &>() = 3, "");
}

RD_TEST_F(MetaAny, Comparable) {
  rendu::meta_any any{'c'};

  RD_ASSERT_EQ(any, any);
  RD_ASSERT_EQ(any, rendu::meta_any{'c'});
  RD_ASSERT_NE(rendu::meta_any{'a'}, any);
  RD_ASSERT_NE(any, rendu::meta_any{});

  RD_ASSERT_TRUE(any == any);
  RD_ASSERT_TRUE(any == rendu::meta_any{'c'});
  RD_ASSERT_FALSE(rendu::meta_any{'a'} == any);
  RD_ASSERT_TRUE(any != rendu::meta_any{'a'});
  RD_ASSERT_TRUE(rendu::meta_any{} != any);
}

RD_TEST_F(MetaAny, NotComparable) {
  rendu::meta_any any{not_comparable_t{}};

  RD_ASSERT_EQ(any, any);
  RD_ASSERT_NE(any, rendu::meta_any{not_comparable_t{}});
  RD_ASSERT_NE(rendu::meta_any{}, any);

  RD_ASSERT_TRUE(any == any);
  RD_ASSERT_FALSE(any == rendu::meta_any{not_comparable_t{}});
  RD_ASSERT_TRUE(rendu::meta_any{} != any);
}

RD_TEST_F(MetaAny, CompareVoid) {
  rendu::meta_any any{std::in_place_type<void>};

  RD_ASSERT_EQ(any, any);
  RD_ASSERT_EQ(any, rendu::meta_any{std::in_place_type<void>});
  RD_ASSERT_NE(rendu::meta_any{'a'}, any);
  RD_ASSERT_NE(any, rendu::meta_any{});

  RD_ASSERT_TRUE(any == any);
  RD_ASSERT_TRUE(any == rendu::meta_any{std::in_place_type<void>});
  RD_ASSERT_FALSE(rendu::meta_any{'a'} == any);
  RD_ASSERT_TRUE(any != rendu::meta_any{'a'});
  RD_ASSERT_TRUE(rendu::meta_any{} != any);
}

RD_TEST_F(MetaAny, TryCast) {
  rendu::meta_any any{fat_t{}};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.type(), rendu::resolve<fat_t>());
  RD_ASSERT_EQ(any.try_cast<void>(), nullptr);
  RD_ASSERT_NE(any.try_cast<empty_t>(), nullptr);
  RD_ASSERT_EQ(any.try_cast<fat_t>(), any.data());
  RD_ASSERT_EQ(std::as_const(any).try_cast<empty_t>(), any.try_cast<empty_t>());
  RD_ASSERT_EQ(std::as_const(any).try_cast<fat_t>(), any.data());
  RD_ASSERT_EQ(std::as_const(any).try_cast<int>(), nullptr);
}

RD_TEST_F(MetaAny, Cast) {
  rendu::meta_any any{fat_t{}};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.type(), rendu::resolve<fat_t>());
  RD_ASSERT_EQ(std::as_const(any).cast<const fat_t &>(), fat_t{});
  RD_ASSERT_EQ(any.cast<const fat_t>(), fat_t{});
  RD_ASSERT_EQ(any.cast<fat_t &>(), fat_t{});
  RD_ASSERT_EQ(any.cast<fat_t>(), fat_t{});

  RD_ASSERT_EQ(any.cast<fat_t>().value[0u], 0.);

  any.cast<fat_t &>().value[0u] = 3.;

  RD_ASSERT_EQ(any.cast<fat_t>().value[0u], 3.);
}

RD_TEST_F(MetaAny, AllowCast) {
  rendu::meta_any clazz{clazz_t{}};
  rendu::meta_any fat{fat_t{}};
  rendu::meta_any arithmetic{42};
  auto as_cref = rendu::forward_as_meta(arithmetic.cast<const int &>());

  RD_ASSERT_TRUE(clazz);
  RD_ASSERT_TRUE(fat);
  RD_ASSERT_TRUE(arithmetic);
  RD_ASSERT_TRUE(as_cref);

  RD_ASSERT_TRUE(clazz.allow_cast<clazz_t>());
  RD_ASSERT_TRUE(clazz.allow_cast<clazz_t &>());
  RD_ASSERT_TRUE(clazz.allow_cast<const clazz_t &>());
  RD_ASSERT_EQ(clazz.type(), rendu::resolve<clazz_t>());

  RD_ASSERT_TRUE(clazz.allow_cast<const int &>());
  RD_ASSERT_EQ(clazz.type(), rendu::resolve<int>());
  RD_ASSERT_TRUE(clazz.allow_cast<int>());
  RD_ASSERT_TRUE(clazz.allow_cast<int &>());
  RD_ASSERT_TRUE(clazz.allow_cast<const int &>());

  RD_ASSERT_TRUE(fat.allow_cast<fat_t>());
  RD_ASSERT_TRUE(fat.allow_cast<fat_t &>());
  RD_ASSERT_TRUE(fat.allow_cast<const empty_t &>());
  RD_ASSERT_EQ(fat.type(), rendu::resolve<fat_t>());
  RD_ASSERT_FALSE(fat.allow_cast<int>());

  RD_ASSERT_TRUE(std::as_const(fat).allow_cast<fat_t>());
  RD_ASSERT_FALSE(std::as_const(fat).allow_cast<fat_t &>());
  RD_ASSERT_TRUE(std::as_const(fat).allow_cast<const empty_t &>());
  RD_ASSERT_EQ(fat.type(), rendu::resolve<fat_t>());
  RD_ASSERT_FALSE(fat.allow_cast<int>());

  RD_ASSERT_TRUE(arithmetic.allow_cast<int>());
  RD_ASSERT_TRUE(arithmetic.allow_cast<int &>());
  RD_ASSERT_TRUE(arithmetic.allow_cast<const int &>());
  RD_ASSERT_EQ(arithmetic.type(), rendu::resolve<int>());
  RD_ASSERT_FALSE(arithmetic.allow_cast<fat_t>());

  RD_ASSERT_TRUE(arithmetic.allow_cast<double &>());
  RD_ASSERT_EQ(arithmetic.type(), rendu::resolve<double>());
  RD_ASSERT_EQ(arithmetic.cast<double &>(), 42.);

  RD_ASSERT_TRUE(arithmetic.allow_cast<const float &>());
  RD_ASSERT_EQ(arithmetic.type(), rendu::resolve<float>());
  RD_ASSERT_EQ(arithmetic.cast<float &>(), 42.f);

  RD_ASSERT_TRUE(as_cref.allow_cast<int>());
  RD_ASSERT_FALSE(as_cref.allow_cast<int &>());
  RD_ASSERT_TRUE(as_cref.allow_cast<const int &>());
  RD_ASSERT_EQ(as_cref.type(), rendu::resolve<int>());
  RD_ASSERT_FALSE(as_cref.allow_cast<fat_t>());

  RD_ASSERT_TRUE(as_cref.allow_cast<double &>());
  RD_ASSERT_EQ(as_cref.type(), rendu::resolve<double>());
}

RD_TEST_F(MetaAny, OpaqueAllowCast) {
  rendu::meta_any clazz{clazz_t{}};
  rendu::meta_any fat{fat_t{}};
  rendu::meta_any arithmetic{42};
  auto as_cref = rendu::forward_as_meta(arithmetic.cast<const int &>());

  RD_ASSERT_TRUE(clazz);
  RD_ASSERT_TRUE(fat);
  RD_ASSERT_TRUE(arithmetic);
  RD_ASSERT_TRUE(as_cref);

  RD_ASSERT_TRUE(clazz.allow_cast(rendu::resolve<clazz_t>()));
  RD_ASSERT_EQ(clazz.type(), rendu::resolve<clazz_t>());

  RD_ASSERT_TRUE(clazz.allow_cast(rendu::resolve<int>()));
  RD_ASSERT_EQ(clazz.type(), rendu::resolve<int>());
  RD_ASSERT_TRUE(clazz.allow_cast(rendu::resolve<int>()));

  RD_ASSERT_TRUE(fat.allow_cast(rendu::resolve<fat_t>()));
  RD_ASSERT_TRUE(fat.allow_cast(rendu::resolve<empty_t>()));
  RD_ASSERT_EQ(fat.type(), rendu::resolve<fat_t>());
  RD_ASSERT_FALSE(fat.allow_cast(rendu::resolve<int>()));

  RD_ASSERT_TRUE(std::as_const(fat).allow_cast(rendu::resolve<fat_t>()));
  RD_ASSERT_TRUE(std::as_const(fat).allow_cast(rendu::resolve<empty_t>()));
  RD_ASSERT_EQ(fat.type(), rendu::resolve<fat_t>());
  RD_ASSERT_FALSE(fat.allow_cast(rendu::resolve<int>()));

  RD_ASSERT_TRUE(arithmetic.allow_cast(rendu::resolve<int>()));
  RD_ASSERT_EQ(arithmetic.type(), rendu::resolve<int>());
  RD_ASSERT_FALSE(arithmetic.allow_cast(rendu::resolve<fat_t>()));

  RD_ASSERT_TRUE(arithmetic.allow_cast(rendu::resolve<double>()));
  RD_ASSERT_EQ(arithmetic.type(), rendu::resolve<double>());
  RD_ASSERT_EQ(arithmetic.cast<double &>(), 42.);

  RD_ASSERT_TRUE(arithmetic.allow_cast(rendu::resolve<float>()));
  RD_ASSERT_EQ(arithmetic.type(), rendu::resolve<float>());
  RD_ASSERT_EQ(arithmetic.cast<float &>(), 42.f);

  RD_ASSERT_TRUE(as_cref.allow_cast(rendu::resolve<int>()));
  RD_ASSERT_EQ(as_cref.type(), rendu::resolve<int>());
  RD_ASSERT_FALSE(as_cref.allow_cast(rendu::resolve<fat_t>()));

  RD_ASSERT_TRUE(as_cref.allow_cast(rendu::resolve<double>()));
  RD_ASSERT_EQ(as_cref.type(), rendu::resolve<double>());
}

RD_TEST_F(MetaAny, Convert) {
  rendu::meta_any any{clazz_t{}};
  any.cast<clazz_t &>().value = 42;
  auto as_int = std::as_const(any).allow_cast<int>();

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.type(), rendu::resolve<clazz_t>());
  RD_ASSERT_TRUE(any.allow_cast<clazz_t>());
  RD_ASSERT_EQ(any.type(), rendu::resolve<clazz_t>());
  RD_ASSERT_TRUE(any.allow_cast<int>());
  RD_ASSERT_EQ(any.type(), rendu::resolve<int>());
  RD_ASSERT_EQ(any.cast<int>(), 42);

  RD_ASSERT_TRUE(as_int);
  RD_ASSERT_EQ(as_int.type(), rendu::resolve<int>());
  RD_ASSERT_EQ(as_int.cast<int>(), 42);

  RD_ASSERT_TRUE(as_int.allow_cast<char>());
  RD_ASSERT_EQ(as_int.type(), rendu::resolve<char>());
  RD_ASSERT_EQ(as_int.cast<char>(), char{42});
}

RD_TEST_F(MetaAny, ArithmeticConversion) {
  rendu::meta_any any{'c'};

  RD_ASSERT_EQ(any.type(), rendu::resolve<char>());
  RD_ASSERT_EQ(any.cast<char>(), 'c');

  RD_ASSERT_TRUE(any.allow_cast<double>());
  RD_ASSERT_EQ(any.type(), rendu::resolve<double>());
  RD_ASSERT_EQ(any.cast<double>(), static_cast<double>('c'));

  any = 3.1;

  RD_ASSERT_TRUE(any.allow_cast(rendu::resolve<int>()));
  RD_ASSERT_EQ(any.type(), rendu::resolve<int>());
  RD_ASSERT_EQ(any.cast<int>(), 3);

  RD_ASSERT_TRUE(any.allow_cast<float>());
  RD_ASSERT_EQ(any.type(), rendu::resolve<float>());
  RD_ASSERT_EQ(any.cast<float>(), 3.f);

  any = static_cast<float>('c');

  RD_ASSERT_TRUE(any.allow_cast<char>());
  RD_ASSERT_EQ(any.type(), rendu::resolve<char>());
  RD_ASSERT_EQ(any.cast<char>(), 'c');
}

RD_TEST_F(MetaAny, EnumConversion) {
  rendu::meta_any any{enum_class::foo};

  RD_ASSERT_EQ(any.type(), rendu::resolve<enum_class>());
  RD_ASSERT_EQ(any.cast<enum_class>(), enum_class::foo);

  RD_ASSERT_TRUE(any.allow_cast<double>());
  RD_ASSERT_EQ(any.type(), rendu::resolve<double>());
  RD_ASSERT_EQ(any.cast<double>(), 0.);

  any = enum_class::bar;

  RD_ASSERT_TRUE(any.allow_cast(rendu::resolve<int>()));
  RD_ASSERT_EQ(any.type(), rendu::resolve<int>());
  RD_ASSERT_EQ(any.cast<int>(), 42);

  RD_ASSERT_TRUE(any.allow_cast<enum_class>());
  RD_ASSERT_EQ(any.type(), rendu::resolve<enum_class>());
  RD_ASSERT_EQ(any.cast<enum_class>(), enum_class::bar);

  any = 0;

  RD_ASSERT_TRUE(any.allow_cast(rendu::resolve<enum_class>()));
  RD_ASSERT_EQ(any.type(), rendu::resolve<enum_class>());
  RD_ASSERT_EQ(any.cast<enum_class>(), enum_class::foo);
}

RD_TEST_F(MetaAny, UnmanageableType) {
  unmanageable_t instance;
  auto any = rendu::forward_as_meta(instance);
  rendu::meta_any other = any.as_ref();

  std::swap(any, other);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(other);

  RD_ASSERT_EQ(any.type(), rendu::resolve<unmanageable_t>());
  RD_ASSERT_NE(any.data(), nullptr);
  RD_ASSERT_EQ(any.try_cast<int>(), nullptr);
  RD_ASSERT_NE(any.try_cast<unmanageable_t>(), nullptr);

  RD_ASSERT_TRUE(any.allow_cast<unmanageable_t>());
  RD_ASSERT_FALSE(any.allow_cast<int>());

  RD_ASSERT_TRUE(std::as_const(any).allow_cast<unmanageable_t>());
  RD_ASSERT_FALSE(std::as_const(any).allow_cast<int>());
}

RD_TEST_F(MetaAny, Invoke) {
  using namespace rendu::literals;

  clazz_t instance;
  auto any = rendu::forward_as_meta(instance);

  RD_ASSERT_TRUE(any.invoke("func"_hs));
  RD_ASSERT_TRUE(any.invoke("member"_hs, 42));
  RD_ASSERT_FALSE(std::as_const(any).invoke("member"_hs, 42));
  RD_ASSERT_FALSE(std::as_const(any).as_ref().invoke("member"_hs, 42));
  RD_ASSERT_FALSE(any.invoke("non_existent"_hs, 42));

  RD_ASSERT_EQ(clazz_t::c, 'd');
  RD_ASSERT_EQ(instance.value, 42);
}

RD_TEST_F(MetaAny, SetGet) {
  using namespace rendu::literals;

  clazz_t instance;
  auto any = rendu::forward_as_meta(instance);

  RD_ASSERT_TRUE(any.set("value"_hs, 42));

  const auto value = std::as_const(any).get("value"_hs);

  RD_ASSERT_TRUE(value);
  RD_ASSERT_EQ(value, any.get("value"_hs));
  RD_ASSERT_EQ(value, std::as_const(any).as_ref().get("value"_hs));
  RD_ASSERT_NE(value.try_cast<int>(), nullptr);
  RD_ASSERT_EQ(value.cast<int>(), 42);
  RD_ASSERT_EQ(instance.value, 42);

  RD_ASSERT_FALSE(any.set("non_existent"_hs, 42));
  RD_ASSERT_FALSE(any.get("non_existent"_hs));
}

RD_TEST_F(MetaAny, ForwardAsMeta) {
  int value = 42;
  auto ref = rendu::forward_as_meta(value);
  auto cref = rendu::forward_as_meta(std::as_const(value));
  auto any = rendu::forward_as_meta(std::move(value));

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(ref);
  RD_ASSERT_TRUE(cref);

  RD_ASSERT_NE(any.try_cast<int>(), nullptr);
  RD_ASSERT_NE(ref.try_cast<int>(), nullptr);
  RD_ASSERT_EQ(cref.try_cast<int>(), nullptr);

  RD_ASSERT_EQ(any.cast<const int &>(), 42);
  RD_ASSERT_EQ(ref.cast<const int &>(), 42);
  RD_ASSERT_EQ(cref.cast<const int &>(), 42);

  RD_ASSERT_NE(any.data(), &value);
  RD_ASSERT_EQ(ref.data(), &value);
}
}
