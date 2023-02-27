/*
* Created by boil on 2023/2/27.
*/
#include <utility>
#include <test/rdtest.h>
#include <core/base/hashed_string.h>
#include <core/base/utility.h>
#include <core/ecs/registry.h>
#include <core/locator/locator.h>
#include <core/meta/factory.h>
#include <core/meta/meta.h>
#include <core/meta/resolve.h>

namespace meta_ctor{

struct base_t {
  base_t()
      : value{'c'} {}

  char value;
};

struct derived_t: base_t {
  derived_t()
      : base_t{} {}
};

struct clazz_t {
  clazz_t(const base_t &other, int &iv)
      : clazz_t{iv, other.value} {}

  clazz_t(const int &iv, char cv)
      : i{iv}, c{cv} {}

  explicit operator int() const {
    return i;
  }

  static clazz_t factory(int value) {
    return {value, 'c'};
  }

  static clazz_t factory(base_t other, int value, int mul) {
    return {value * mul, other.value};
  }

  int i{};
  char c{};
};

double double_factory() {
  return 42.;
}

struct MetaCtor: ::testing::Test {
  void SetUp() override {
    using namespace rendu::literals;

    rendu::meta<double>()
        .type("double"_hs)
        .ctor<double_factory>();

    rendu::meta<derived_t>()
        .type("derived"_hs)
        .base<base_t>();

    rendu::meta<clazz_t>()
        .type("clazz"_hs)
        .ctor<&rendu::registry::emplace_or_replace<clazz_t, const int &, const char &>, rendu::as_ref_t>()
        .ctor<const base_t &, int &>()
        .ctor<const int &, char>()
        .ctor<rendu::overload<clazz_t(int)>(clazz_t::factory)>()
        .ctor<rendu::overload<clazz_t(base_t, int, int)>(clazz_t::factory)>()
        .conv<int>();
  }

  void TearDown() override {
    rendu::meta_reset();
  }
};

RD_TEST_F(MetaCtor, Functionalities) {
  auto any = rendu::resolve<clazz_t>().construct(42, 'c');

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<clazz_t>().i, 42);
  RD_ASSERT_EQ(any.cast<clazz_t>().c, 'c');
}

RD_TEST_F(MetaCtor, Func) {
  auto any = rendu::resolve<clazz_t>().construct(42);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<clazz_t>().i, 42);
  RD_ASSERT_EQ(any.cast<clazz_t>().c, 'c');
}

RD_TEST_F(MetaCtor, MetaAnyArgs) {
  auto any = rendu::resolve<clazz_t>().construct(rendu::meta_any{42}, rendu::meta_any{'c'});

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<clazz_t>().i, 42);
  RD_ASSERT_EQ(any.cast<clazz_t>().c, 'c');
}

RD_TEST_F(MetaCtor, InvalidArgs) {
  RD_ASSERT_FALSE(rendu::resolve<clazz_t>().construct(rendu::meta_any{}, derived_t{}));
}

RD_TEST_F(MetaCtor, CastAndConvert) {
  auto any = rendu::resolve<clazz_t>().construct(derived_t{}, clazz_t{42, 'd'});

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<clazz_t>().i, 42);
  RD_ASSERT_EQ(any.cast<clazz_t>().c, 'c');
}

RD_TEST_F(MetaCtor, ArithmeticConversion) {
  auto any = rendu::resolve<clazz_t>().construct(true, 4.2);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<clazz_t>().i, 1);
  RD_ASSERT_EQ(any.cast<clazz_t>().c, char{4});
}

RD_TEST_F(MetaCtor, ConstNonConstRefArgs) {
  int ivalue = 42;
  const char cvalue = 'c';
  auto any = rendu::resolve<clazz_t>().construct(rendu::forward_as_meta(ivalue), rendu::forward_as_meta(cvalue));

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<clazz_t>().i, 42);
  RD_ASSERT_EQ(any.cast<clazz_t>().c, 'c');
}

RD_TEST_F(MetaCtor, WrongConstness) {
  int value = 42;
  auto any = rendu::resolve<clazz_t>().construct(derived_t{}, rendu::forward_as_meta(value));
  auto other = rendu::resolve<clazz_t>().construct(derived_t{}, rendu::forward_as_meta(std::as_const(value)));

  RD_ASSERT_TRUE(any);
  RD_ASSERT_FALSE(other);
  RD_ASSERT_EQ(any.cast<clazz_t>().i, 42);
  RD_ASSERT_EQ(any.cast<clazz_t>().c, 'c');
}

RD_TEST_F(MetaCtor, FuncMetaAnyArgs) {
  auto any = rendu::resolve<clazz_t>().construct(rendu::meta_any{42});

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<clazz_t>().i, 42);
  RD_ASSERT_EQ(any.cast<clazz_t>().c, 'c');
}

RD_TEST_F(MetaCtor, FuncCastAndConvert) {
  auto any = rendu::resolve<clazz_t>().construct(derived_t{}, 3., clazz_t{3, 'd'});

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<clazz_t>().i, 9);
  RD_ASSERT_EQ(any.cast<clazz_t>().c, 'c');
}

RD_TEST_F(MetaCtor, FuncArithmeticConversion) {
  auto any = rendu::resolve<clazz_t>().construct(4.2);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<clazz_t>().i, 4);
  RD_ASSERT_EQ(any.cast<clazz_t>().c, 'c');
}

RD_TEST_F(MetaCtor, FuncConstNonConstRefArgs) {
  int ivalue = 42;
  auto any = rendu::resolve<clazz_t>().construct(rendu::forward_as_meta(ivalue));
  auto other = rendu::resolve<clazz_t>().construct(rendu::forward_as_meta(std::as_const(ivalue)));

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(other);
  RD_ASSERT_EQ(any.cast<clazz_t>().i, 42);
  RD_ASSERT_EQ(other.cast<clazz_t>().i, 42);
}

RD_TEST_F(MetaCtor, ExternalMemberFunction) {
  rendu::registry registry;
  const auto entity = registry.create();

  RD_ASSERT_FALSE(registry.all_of<clazz_t>(entity));

  const auto any = rendu::resolve<clazz_t>().construct(rendu::forward_as_meta(registry), entity, 3, 'c');

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(registry.all_of<clazz_t>(entity));
  RD_ASSERT_EQ(registry.get<clazz_t>(entity).i, 3);
  RD_ASSERT_EQ(registry.get<clazz_t>(entity).c, 'c');
}

RD_TEST_F(MetaCtor, OverrideImplicitlyGeneratedDefaultConstructor) {
  auto type = rendu::resolve<double>();
  auto any = type.construct();

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.type(), rendu::resolve<double>());
  RD_ASSERT_EQ(any.cast<double>(), 42.);
}

RD_TEST_F(MetaCtor, NonDefaultConstructibleType) {
  auto type = rendu::resolve<clazz_t>();
  // no implicitly generated default constructor
  RD_ASSERT_FALSE(type.construct());
}

RD_TEST_F(MetaCtor, ReRegistration) {
  SetUp();

  auto &&node = rendu::internal::resolve<double>(rendu::internal::meta_context::from(rendu::locator<rendu::meta_ctx>::value_or()));

  RD_ASSERT_TRUE(node.details);
  RD_ASSERT_FALSE(node.details->ctor.empty());
  // implicitly generated default constructor is not cleared
  RD_ASSERT_NE(node.default_constructor, nullptr);
}
}
