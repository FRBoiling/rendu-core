/*
* Created by boil on 2023/2/27.
*/
#include <test/rdtest.h>
#include <core/base/hashed_string.h>
#include <core/base/type_traits.h>
#include <core/locator/locator.h>
#include <core/meta/factory.h>
#include <core/meta/meta.h>
#include <core/meta/node.h>
#include <core/meta/resolve.h>
namespace meta_data {

struct base_t {
  virtual ~base_t() = default;

  static void destroy(base_t &) {
    ++counter;
  }

  inline static int counter = 0;
  int value{3};
};

struct derived_t : base_t {
  derived_t() {}
};

struct clazz_t {
  clazz_t()
      : i{0},
        j{1},
        base{} {}

  operator int() const {
    return h;
  }

  int i{0};
  const int j{1};
  base_t base{};
  inline static int h{2};
  inline static const int k{3};
};

struct setter_getter_t {
  setter_getter_t()
      : value{0} {}

  int setter(double val) {
    return value = static_cast<int>(val);
  }

  int getter() {
    return value;
  }

  int setter_with_ref(const int &val) {
    return value = val;
  }

  const int &getter_with_ref() {
    return value;
  }

  static int static_setter(setter_getter_t &type, int value) {
    return type.value = value;
  }

  static int static_getter(const setter_getter_t &type) {
    return type.value;
  }

  int value;
};

struct multi_setter_t {
  multi_setter_t()
      : value{0} {}

  void from_double(double val) {
    value = static_cast<int>(val);
  }

  void from_string(const char *val) {
    value = std::atoi(val);
  }

  int value;
};

struct array_t {
  static inline int global[3];
  int local[5];
};

enum class property_t : rendu::id_type {
  random,
  value
};

struct MetaData : ::testing::Test {
  void SetUp() override {
    using namespace rendu::literals;

    rendu::meta<double>()
        .type("double"_hs);

    rendu::meta<base_t>()
        .type("base"_hs)
        .dtor<base_t::destroy>()
        .data<&base_t::value>("value"_hs);

    rendu::meta<derived_t>()
        .type("derived"_hs)
        .base<base_t>()
        .dtor<derived_t::destroy>()
        .data<&base_t::value>("value_from_base"_hs);

    rendu::meta<clazz_t>()
        .type("clazz"_hs)
        .data<&clazz_t::i, rendu::as_ref_t>("i"_hs)
        .prop(3u, 0)
        .data<&clazz_t::i, rendu::as_cref_t>("ci"_hs)
        .data<&clazz_t::j>("j"_hs)
        .prop("true"_hs, 1)
        .data<&clazz_t::h>("h"_hs)
        .prop(static_cast<rendu::id_type>(property_t::random), 2)
        .data<&clazz_t::k>("k"_hs)
        .prop(static_cast<rendu::id_type>(property_t::value), 3)
        .data<&clazz_t::base>("base"_hs)
        .data<&clazz_t::i, rendu::as_void_t>("void"_hs)
        .conv<int>();

    rendu::meta<setter_getter_t>()
        .type("setter_getter"_hs)
        .data<&setter_getter_t::static_setter, &setter_getter_t::static_getter>("x"_hs)
        .data<&setter_getter_t::setter, &setter_getter_t::getter>("y"_hs)
        .data<&setter_getter_t::static_setter, &setter_getter_t::getter>("z"_hs)
        .data<&setter_getter_t::setter_with_ref, &setter_getter_t::getter_with_ref>("w"_hs)
        .data<nullptr, &setter_getter_t::getter>("z_ro"_hs)
        .data<nullptr, &setter_getter_t::value>("value"_hs);

    rendu::meta<multi_setter_t>()
        .type("multi_setter"_hs)
        .data<rendu::value_list<&multi_setter_t::from_double, &multi_setter_t::from_string>, &multi_setter_t::value>(
            "value"_hs);

    rendu::meta<array_t>()
        .type("array"_hs)
        .data<&array_t::global>("global"_hs)
        .data<&array_t::local>("local"_hs);

    base_t::counter = 0;
  }

  void TearDown() override {
    rendu::meta_reset();
  }
};

using MetaDataDeathTest = MetaData;

RD_TEST_F(MetaData, Functionalities) {
  using namespace rendu::literals;

  auto data = rendu::resolve<clazz_t>().data("i"_hs);
  clazz_t instance{};

  RD_ASSERT_TRUE(data);
  RD_ASSERT_EQ(data.arity(), 1u);
  RD_ASSERT_EQ(data.type(), rendu::resolve<int>());
  RD_ASSERT_EQ(data.arg(0u), rendu::resolve<int>());
  RD_ASSERT_FALSE(data.is_const());
  RD_ASSERT_FALSE(data.is_static());
  RD_ASSERT_EQ(data.get(instance).cast<int>(), 0);
  RD_ASSERT_TRUE(data.set(instance, 42));
  RD_ASSERT_EQ(data.get(instance).cast<int>(), 42);

  for (auto curr : data.prop()) {
    RD_ASSERT_EQ(curr.first, 3u);
    RD_ASSERT_EQ(curr.second.value(), 0);
  }

  RD_ASSERT_FALSE(data.prop(2));
  RD_ASSERT_FALSE(data.prop('c'));

  auto prop = data.prop(3u);

  RD_ASSERT_TRUE(prop);
  RD_ASSERT_EQ(prop.value(), 0);
}

RD_TEST_F(MetaData, Const) {
  using namespace rendu::literals;

  auto data = rendu::resolve<clazz_t>().data("j"_hs);
  clazz_t instance{};

  RD_ASSERT_TRUE(data);
  RD_ASSERT_EQ(data.arity(), 1u);
  RD_ASSERT_EQ(data.type(), rendu::resolve<int>());
  RD_ASSERT_EQ(data.arg(0u), rendu::resolve<int>());
  RD_ASSERT_TRUE(data.is_const());
  RD_ASSERT_FALSE(data.is_static());
  RD_ASSERT_EQ(data.get(instance).cast<int>(), 1);
  RD_ASSERT_FALSE(data.set(instance, 42));
  RD_ASSERT_EQ(data.get(instance).cast<int>(), 1);

  for (auto curr : data.prop()) {
    RD_ASSERT_EQ(curr.first, "true"_hs);
    RD_ASSERT_EQ(curr.second.value(), 1);
  }

  RD_ASSERT_FALSE(data.prop(false));
  RD_ASSERT_FALSE(data.prop('c'));

  auto prop = data.prop("true"_hs);

  RD_ASSERT_TRUE(prop);
  RD_ASSERT_EQ(prop.value(), 1);
}

RD_TEST_F(MetaData, Static) {
  using namespace rendu::literals;

  auto data = rendu::resolve<clazz_t>().data("h"_hs);

  RD_ASSERT_TRUE(data);
  RD_ASSERT_EQ(data.arity(), 1u);
  RD_ASSERT_EQ(data.type(), rendu::resolve<int>());
  RD_ASSERT_EQ(data.arg(0u), rendu::resolve<int>());
  RD_ASSERT_FALSE(data.is_const());
  RD_ASSERT_TRUE(data.is_static());
  RD_ASSERT_EQ(data.get({}).cast<int>(), 2);
  RD_ASSERT_TRUE(data.set({}, 42));
  RD_ASSERT_EQ(data.get({}).cast<int>(), 42);

  for (auto curr : data.prop()) {
    RD_ASSERT_EQ(curr.first, static_cast<rendu::id_type>(property_t::random));
    RD_ASSERT_EQ(curr.second.value(), 2);
  }

  RD_ASSERT_FALSE(data.prop(static_cast<rendu::id_type>(property_t::value)));
  RD_ASSERT_FALSE(data.prop('c'));

  auto prop = data.prop(static_cast<rendu::id_type>(property_t::random));

  RD_ASSERT_TRUE(prop);
  RD_ASSERT_EQ(prop.value(), 2);
}

RD_TEST_F(MetaData, ConstStatic) {
  using namespace rendu::literals;

  auto data = rendu::resolve<clazz_t>().data("k"_hs);

  RD_ASSERT_TRUE(data);
  RD_ASSERT_EQ(data.arity(), 1u);
  RD_ASSERT_EQ(data.type(), rendu::resolve<int>());
  RD_ASSERT_EQ(data.arg(0u), rendu::resolve<int>());
  RD_ASSERT_TRUE(data.is_const());
  RD_ASSERT_TRUE(data.is_static());
  RD_ASSERT_EQ(data.get({}).cast<int>(), 3);
  RD_ASSERT_FALSE(data.set({}, 42));
  RD_ASSERT_EQ(data.get({}).cast<int>(), 3);

  for (auto curr : data.prop()) {
    RD_ASSERT_EQ(curr.first, static_cast<rendu::id_type>(property_t::value));
    RD_ASSERT_EQ(curr.second.value(), 3);
  }

  RD_ASSERT_FALSE(data.prop(static_cast<rendu::id_type>(property_t::random)));
  RD_ASSERT_FALSE(data.prop('c'));

  auto prop = data.prop(static_cast<rendu::id_type>(property_t::value));

  RD_ASSERT_TRUE(prop);
  RD_ASSERT_EQ(prop.value(), 3);
}

RD_TEST_F(MetaData, GetMetaAnyArg) {
  using namespace rendu::literals;

  rendu::meta_any any{clazz_t{}};
  any.cast<clazz_t &>().i = 99;
  const auto value = rendu::resolve<clazz_t>().data("i"_hs).get(any);

  RD_ASSERT_TRUE(value);
  RD_ASSERT_TRUE(static_cast<bool>(value.cast<int>()));
  RD_ASSERT_EQ(value.cast<int>(), 99);
}

RD_TEST_F(MetaData, GetInvalidArg) {
  using namespace rendu::literals;

  auto instance = 0;
  RD_ASSERT_FALSE(rendu::resolve<clazz_t>().data("i"_hs).get(instance));
}

RD_TEST_F(MetaData, SetMetaAnyArg) {
  using namespace rendu::literals;

  rendu::meta_any any{clazz_t{}};
  rendu::meta_any value{42};

  RD_ASSERT_EQ(any.cast<clazz_t>().i, 0);
  RD_ASSERT_TRUE(rendu::resolve<clazz_t>().data("i"_hs).set(any, value));
  RD_ASSERT_EQ(any.cast<clazz_t>().i, 42);
}

RD_TEST_F(MetaData, SetInvalidArg) {
  using namespace rendu::literals;

  RD_ASSERT_FALSE(rendu::resolve<clazz_t>().data("i"_hs).set({}, 'c'));
}

RD_TEST_F(MetaData, SetCast) {
  using namespace rendu::literals;

  clazz_t instance{};

  RD_ASSERT_EQ(base_t::counter, 0);
  RD_ASSERT_TRUE(rendu::resolve<clazz_t>().data("base"_hs).set(instance, derived_t{}));
  RD_ASSERT_EQ(base_t::counter, 1);
}

RD_TEST_F(MetaData, SetConvert) {
  using namespace rendu::literals;

  clazz_t instance{};
  instance.h = 42;

  RD_ASSERT_EQ(instance.i, 0);
  RD_ASSERT_TRUE(rendu::resolve<clazz_t>().data("i"_hs).set(instance, instance));
  RD_ASSERT_EQ(instance.i, 42);
}

RD_TEST_F(MetaData, SetByRef) {
  using namespace rendu::literals;

  rendu::meta_any any{clazz_t{}};
  int value{42};

  RD_ASSERT_EQ(any.cast<clazz_t>().i, 0);
  RD_ASSERT_TRUE(rendu::resolve<clazz_t>().data("i"_hs).set(any, rendu::forward_as_meta(value)));
  RD_ASSERT_EQ(any.cast<clazz_t>().i, 42);

  value = 3;
  auto wrapper = rendu::forward_as_meta(value);

  RD_ASSERT_TRUE(rendu::resolve<clazz_t>().data("i"_hs).set(any, wrapper.as_ref()));
  RD_ASSERT_EQ(any.cast<clazz_t>().i, 3);
}

RD_TEST_F(MetaData, SetByConstRef) {
  using namespace rendu::literals;

  rendu::meta_any any{clazz_t{}};
  int value{42};

  RD_ASSERT_EQ(any.cast<clazz_t>().i, 0);
  RD_ASSERT_TRUE(rendu::resolve<clazz_t>().data("i"_hs).set(any, rendu::forward_as_meta(std::as_const(value))));
  RD_ASSERT_EQ(any.cast<clazz_t>().i, 42);

  value = 3;
  auto wrapper = rendu::forward_as_meta(std::as_const(value));

  RD_ASSERT_TRUE(rendu::resolve<clazz_t>().data("i"_hs).set(any, wrapper.as_ref()));
  RD_ASSERT_EQ(any.cast<clazz_t>().i, 3);
}

RD_TEST_F(MetaData, SetterGetterAsFreeFunctions) {
  using namespace rendu::literals;

  auto data = rendu::resolve<setter_getter_t>().data("x"_hs);
  setter_getter_t instance{};

  RD_ASSERT_TRUE(data);
  RD_ASSERT_EQ(data.arity(), 1u);
  RD_ASSERT_EQ(data.type(), rendu::resolve<int>());
  RD_ASSERT_EQ(data.arg(0u), rendu::resolve<int>());
  RD_ASSERT_FALSE(data.is_const());
  RD_ASSERT_FALSE(data.is_static());
  RD_ASSERT_EQ(data.get(instance).cast<int>(), 0);
  RD_ASSERT_TRUE(data.set(instance, 42));
  RD_ASSERT_EQ(data.get(instance).cast<int>(), 42);
}

RD_TEST_F(MetaData, SetterGetterAsMemberFunctions) {
  using namespace rendu::literals;

  auto data = rendu::resolve<setter_getter_t>().data("y"_hs);
  setter_getter_t instance{};

  RD_ASSERT_TRUE(data);
  RD_ASSERT_EQ(data.arity(), 1u);
  RD_ASSERT_EQ(data.type(), rendu::resolve<int>());
  RD_ASSERT_EQ(data.arg(0u), rendu::resolve<double>());
  RD_ASSERT_FALSE(data.is_const());
  RD_ASSERT_FALSE(data.is_static());
  RD_ASSERT_EQ(data.get(instance).cast<int>(), 0);
  RD_ASSERT_TRUE(data.set(instance, 42.));
  RD_ASSERT_EQ(data.get(instance).cast<int>(), 42);
  RD_ASSERT_TRUE(data.set(instance, 3));
  RD_ASSERT_EQ(data.get(instance).cast<int>(), 3);
}

RD_TEST_F(MetaData, SetterGetterWithRefAsMemberFunctions) {
  using namespace rendu::literals;

  auto data = rendu::resolve<setter_getter_t>().data("w"_hs);
  setter_getter_t instance{};

  RD_ASSERT_TRUE(data);
  RD_ASSERT_EQ(data.arity(), 1u);
  RD_ASSERT_EQ(data.type(), rendu::resolve<int>());
  RD_ASSERT_EQ(data.arg(0u), rendu::resolve<int>());
  RD_ASSERT_FALSE(data.is_const());
  RD_ASSERT_FALSE(data.is_static());
  RD_ASSERT_EQ(data.get(instance).cast<int>(), 0);
  RD_ASSERT_TRUE(data.set(instance, 42));
  RD_ASSERT_EQ(data.get(instance).cast<int>(), 42);
}

RD_TEST_F(MetaData, SetterGetterMixed) {
  using namespace rendu::literals;

  auto data = rendu::resolve<setter_getter_t>().data("z"_hs);
  setter_getter_t instance{};

  RD_ASSERT_TRUE(data);
  RD_ASSERT_EQ(data.arity(), 1u);
  RD_ASSERT_EQ(data.type(), rendu::resolve<int>());
  RD_ASSERT_EQ(data.arg(0u), rendu::resolve<int>());
  RD_ASSERT_FALSE(data.is_const());
  RD_ASSERT_FALSE(data.is_static());
  RD_ASSERT_EQ(data.get(instance).cast<int>(), 0);
  RD_ASSERT_TRUE(data.set(instance, 42));
  RD_ASSERT_EQ(data.get(instance).cast<int>(), 42);
}

RD_TEST_F(MetaData, SetterGetterReadOnly) {
  using namespace rendu::literals;

  auto data = rendu::resolve<setter_getter_t>().data("z_ro"_hs);
  setter_getter_t instance{};

  RD_ASSERT_TRUE(data);
  RD_ASSERT_EQ(data.arity(), 0u);
  RD_ASSERT_EQ(data.type(), rendu::resolve<int>());
  RD_ASSERT_EQ(data.arg(0u), rendu::meta_type{});
  RD_ASSERT_TRUE(data.is_const());
  RD_ASSERT_FALSE(data.is_static());
  RD_ASSERT_EQ(data.get(instance).cast<int>(), 0);
  RD_ASSERT_FALSE(data.set(instance, 42));
  RD_ASSERT_EQ(data.get(instance).cast<int>(), 0);
}

RD_TEST_F(MetaData, SetterGetterReadOnlyDataMember) {
  using namespace rendu::literals;

  auto data = rendu::resolve<setter_getter_t>().data("value"_hs);
  setter_getter_t instance{};

  RD_ASSERT_TRUE(data);
  RD_ASSERT_EQ(data.arity(), 0u);
  RD_ASSERT_EQ(data.type(), rendu::resolve<int>());
  RD_ASSERT_EQ(data.arg(0u), rendu::meta_type{});
  RD_ASSERT_TRUE(data.is_const());
  RD_ASSERT_FALSE(data.is_static());
  RD_ASSERT_EQ(data.get(instance).cast<int>(), 0);
  RD_ASSERT_FALSE(data.set(instance, 42));
  RD_ASSERT_EQ(data.get(instance).cast<int>(), 0);
}

RD_TEST_F(MetaData, MultiSetter) {
  using namespace rendu::literals;

  auto data = rendu::resolve<multi_setter_t>().data("value"_hs);
  multi_setter_t instance{};

  RD_ASSERT_TRUE(data);
  RD_ASSERT_EQ(data.arity(), 2u);
  RD_ASSERT_EQ(data.type(), rendu::resolve<int>());
  RD_ASSERT_EQ(data.arg(0u), rendu::resolve<double>());
  RD_ASSERT_EQ(data.arg(1u), rendu::resolve<const char *>());
  RD_ASSERT_EQ(data.arg(2u), rendu::meta_type{});
  RD_ASSERT_FALSE(data.is_const());
  RD_ASSERT_FALSE(data.is_static());
  RD_ASSERT_EQ(data.get(instance).cast<int>(), 0);
  RD_ASSERT_TRUE(data.set(instance, 42));
  RD_ASSERT_EQ(data.get(instance).cast<int>(), 42);
  RD_ASSERT_TRUE(data.set(instance, 3.));
  RD_ASSERT_EQ(data.get(instance).cast<int>(), 3);
  RD_ASSERT_FALSE(data.set(instance, std::string{"99"}));
  RD_ASSERT_TRUE(data.set(instance, std::string{"99"}.c_str()));
  RD_ASSERT_EQ(data.get(instance).cast<int>(), 99);
}

RD_TEST_F(MetaData, ConstInstance) {
  using namespace rendu::literals;

  clazz_t instance{};

  RD_ASSERT_NE(rendu::resolve<clazz_t>().data("i"_hs).get(instance).try_cast<int>(), nullptr);
  RD_ASSERT_NE(rendu::resolve<clazz_t>().data("i"_hs).get(instance).try_cast<const int>(), nullptr);
  RD_ASSERT_EQ(rendu::resolve<clazz_t>().data("i"_hs).get(std::as_const(instance)).try_cast<int>(), nullptr);
  // as_ref_t adapts to the constness of the passed object and returns const references in case
  RD_ASSERT_NE(rendu::resolve<clazz_t>().data("i"_hs).get(std::as_const(instance)).try_cast<const int>(), nullptr);

  RD_ASSERT_TRUE(rendu::resolve<clazz_t>().data("i"_hs).get(instance));
  RD_ASSERT_TRUE(rendu::resolve<clazz_t>().data("i"_hs).set(instance, 3));
  RD_ASSERT_TRUE(rendu::resolve<clazz_t>().data("i"_hs).get(std::as_const(instance)));
  RD_ASSERT_FALSE(rendu::resolve<clazz_t>().data("i"_hs).set(std::as_const(instance), 3));

  RD_ASSERT_TRUE(rendu::resolve<clazz_t>().data("ci"_hs).get(instance));
  RD_ASSERT_TRUE(rendu::resolve<clazz_t>().data("ci"_hs).set(instance, 3));
  RD_ASSERT_TRUE(rendu::resolve<clazz_t>().data("ci"_hs).get(std::as_const(instance)));
  RD_ASSERT_FALSE(rendu::resolve<clazz_t>().data("ci"_hs).set(std::as_const(instance), 3));

  RD_ASSERT_TRUE(rendu::resolve<clazz_t>().data("j"_hs).get(instance));
  RD_ASSERT_FALSE(rendu::resolve<clazz_t>().data("j"_hs).set(instance, 3));
  RD_ASSERT_TRUE(rendu::resolve<clazz_t>().data("j"_hs).get(std::as_const(instance)));
  RD_ASSERT_FALSE(rendu::resolve<clazz_t>().data("j"_hs).set(std::as_const(instance), 3));
}

RD_TEST_F(MetaData, ArrayStatic) {
  using namespace rendu::literals;

  auto data = rendu::resolve<array_t>().data("global"_hs);

  RD_ASSERT_TRUE(data);
  RD_ASSERT_EQ(data.arity(), 1u);
  RD_ASSERT_EQ(data.type(), rendu::resolve<int[3]>());
  RD_ASSERT_EQ(data.arg(0u), rendu::resolve<int[3]>());
  RD_ASSERT_FALSE(data.is_const());
  RD_ASSERT_TRUE(data.is_static());
  RD_ASSERT_TRUE(data.type().is_array());
  RD_ASSERT_FALSE(data.get({}));
}

RD_TEST_F(MetaData, Array) {
  using namespace rendu::literals;

  auto data = rendu::resolve<array_t>().data("local"_hs);
  array_t instance{};

  RD_ASSERT_TRUE(data);
  RD_ASSERT_EQ(data.arity(), 1u);
  RD_ASSERT_EQ(data.type(), rendu::resolve<int[5]>());
  RD_ASSERT_EQ(data.arg(0u), rendu::resolve<int[5]>());
  RD_ASSERT_FALSE(data.is_const());
  RD_ASSERT_FALSE(data.is_static());
  RD_ASSERT_TRUE(data.type().is_array());
  RD_ASSERT_FALSE(data.get(instance));
}

RD_TEST_F(MetaData, AsVoid) {
  using namespace rendu::literals;

  auto data = rendu::resolve<clazz_t>().data("void"_hs);
  clazz_t instance{};

  RD_ASSERT_TRUE(data);
  RD_ASSERT_EQ(data.arity(), 1u);
  RD_ASSERT_EQ(data.type(), rendu::resolve<int>());
  RD_ASSERT_EQ(data.arg(0u), rendu::resolve<int>());
  RD_ASSERT_TRUE(data.set(instance, 42));
  RD_ASSERT_EQ(instance.i, 42);
  RD_ASSERT_EQ(data.get(instance), rendu::meta_any{std::in_place_type<void>});
}

RD_TEST_F(MetaData, AsRef) {
  using namespace rendu::literals;

  clazz_t instance{};
  auto data = rendu::resolve<clazz_t>().data("i"_hs);

  RD_ASSERT_TRUE(data);
  RD_ASSERT_EQ(data.arity(), 1u);
  RD_ASSERT_EQ(data.type(), rendu::resolve<int>());
  RD_ASSERT_EQ(data.arg(0u), rendu::resolve<int>());
  RD_ASSERT_NE(data.prop().cbegin(), data.prop().cend());
  RD_ASSERT_EQ(instance.i, 0);

  data.get(instance).cast<int &>() = 3;

  RD_ASSERT_EQ(instance.i, 3);
}

RD_TEST_F(MetaData, AsConstRef) {
  using namespace rendu::literals;

  clazz_t instance{};
  auto data = rendu::resolve<clazz_t>().data("ci"_hs);

  RD_ASSERT_EQ(instance.i, 0);
  RD_ASSERT_EQ(data.arity(), 1u);
  RD_ASSERT_EQ(data.type(), rendu::resolve<int>());
  RD_ASSERT_EQ(data.arg(0u), rendu::resolve<int>());
  RD_ASSERT_EQ(data.get(instance).cast<const int &>(), 0);
  RD_ASSERT_EQ(data.get(instance).cast<int>(), 0);
  RD_ASSERT_EQ(data.prop().cbegin(), data.prop().cend());
  RD_ASSERT_EQ(instance.i, 0);
}

RD_DEBUG_TEST_F(MetaDataDeathTest, AsConstRef) {
  using namespace rendu::literals;

  clazz_t instance{};
  auto data = rendu::resolve<clazz_t>().data("ci"_hs);

  RD_ASSERT_DEATH(data.get(instance).cast<int &>() = 3, "");
}

RD_TEST_F(MetaData, SetGetBaseData) {
  using namespace rendu::literals;

  auto type = rendu::resolve<derived_t>();
  derived_t instance{};

  RD_ASSERT_TRUE(type.data("value"_hs));

  RD_ASSERT_EQ(instance.value, 3);
  RD_ASSERT_TRUE(type.data("value"_hs).set(instance, 42));
  RD_ASSERT_EQ(type.data("value"_hs).get(instance).cast<int>(), 42);
  RD_ASSERT_EQ(instance.value, 42);
}

RD_TEST_F(MetaData, SetGetFromBase) {
  using namespace rendu::literals;

  auto type = rendu::resolve<derived_t>();
  derived_t instance{};

  RD_ASSERT_TRUE(type.data("value_from_base"_hs));

  RD_ASSERT_EQ(instance.value, 3);
  RD_ASSERT_TRUE(type.data("value_from_base"_hs).set(instance, 42));
  RD_ASSERT_EQ(type.data("value_from_base"_hs).get(instance).cast<int>(), 42);
  RD_ASSERT_EQ(instance.value, 42);
}

RD_TEST_F(MetaData, ReRegistration) {
  using namespace rendu::literals;

  SetUp();

  auto &&node =
      rendu::internal::resolve<base_t>(rendu::internal::meta_context::from(rendu::locator<rendu::meta_ctx>::value_or()));
  auto type = rendu::resolve<base_t>();

  RD_ASSERT_TRUE(node.details);
  RD_ASSERT_FALSE(node.details->data.empty());
  RD_ASSERT_EQ(node.details->data.size(), 1u);
  RD_ASSERT_TRUE(type.data("value"_hs));

  rendu::meta<base_t>().data<&base_t::value>("field"_hs);

  RD_ASSERT_TRUE(node.details);
  RD_ASSERT_EQ(node.details->data.size(), 2u);
  RD_ASSERT_TRUE(type.data("value"_hs));
  RD_ASSERT_TRUE(type.data("field"_hs));
}

RD_TEST_F(MetaData, CollisionAndReuse) {
  using namespace rendu::literals;

  RD_ASSERT_TRUE(rendu::resolve<clazz_t>().data("j"_hs));
  RD_ASSERT_FALSE(rendu::resolve<clazz_t>().data("cj"_hs));
  RD_ASSERT_TRUE(rendu::resolve<clazz_t>().data("j"_hs).is_const());

  RD_ASSERT_NO_FATAL_FAILURE(rendu::meta<clazz_t>().data<&clazz_t::i>("j"_hs));
  RD_ASSERT_NO_FATAL_FAILURE(rendu::meta<clazz_t>().data<&clazz_t::j>("cj"_hs));

  RD_ASSERT_TRUE(rendu::resolve<clazz_t>().data("j"_hs));
  RD_ASSERT_TRUE(rendu::resolve<clazz_t>().data("cj"_hs));
  RD_ASSERT_FALSE(rendu::resolve<clazz_t>().data("j"_hs).is_const());
}
}
