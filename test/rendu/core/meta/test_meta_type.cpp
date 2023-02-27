/*
* Created by boil on 2023/2/27.
*/

#include <test/rdtest.h>
#include <core/meta/container.h>
#include <core/meta/factory.h>
#include <core/meta/pointer.h>
#include <core/meta/template.h>

namespace test_meta_type {

template<typename Type>
void set(Type &prop, Type value) {
  prop = value;
}

template<typename Type>
Type get(Type &prop) {
  return prop;
}

struct base_t {
  base_t()
      : value{'c'} {};

  char value;
};

struct derived_t : base_t {
  derived_t()
      : base_t{} {}
};

struct abstract_t {
  virtual ~abstract_t() = default;

  virtual void func(int) {}
  void base_only(int) {}
};

struct concrete_t : base_t, abstract_t {
  void func(int v) override {
    abstract_t::func(v);
    value = v;
  }

  int value{3};
};

struct clazz_t {
  clazz_t() = default;

  clazz_t(const base_t &, int v)
      : value{v} {}

  void member() {}
  static void func() {}

  operator int() const {
    return value;
  }

  int value{};
};

struct overloaded_func_t {
  int e(int v) const {
    return v + v;
  }

  int f(const base_t &, int a, int b) {
    return f(a, b);
  }

  int f(int a, int b) {
    value = a;
    return g(b);
  }

  int f(int v) {
    return 2 * std::as_const(*this).f(v);
  }

  int f(int v) const {
    return g(v);
  }

  float f(int a, float b) {
    value = a;
    return static_cast<float>(e(static_cast<int>(b)));
  }

  int g(int v) const {
    return v * v;
  }

  inline static int value = 0;
};

enum class property_t : rendu::id_type {
  random,
  value,
  key_only,
  list
};

struct MetaType : ::testing::Test {
  void SetUp() override {
    using namespace rendu::literals;

    rendu::meta<double>()
        .type("double"_hs)
        .data<set<double>, get<double>>("var"_hs);

    rendu::meta<unsigned int>()
        .type("unsigned int"_hs)
        .data<0u>("min"_hs)
        .data<100u>("max"_hs);

    rendu::meta<base_t>()
        .type("base"_hs)
        .data<&base_t::value>("value"_hs);

    rendu::meta<derived_t>()
        .type("derived"_hs)
        .base<base_t>();

    rendu::meta<abstract_t>()
        .type("abstract"_hs)
        .func<&abstract_t::func>("func"_hs)
        .func<&abstract_t::base_only>("base_only"_hs);

    rendu::meta<concrete_t>()
        .type("concrete"_hs)
        .base<base_t>()
        .base<abstract_t>();

    rendu::meta<overloaded_func_t>()
        .type("overloaded_func"_hs)
        .func<&overloaded_func_t::e>("e"_hs)
        .func<rendu::overload<int(const base_t &, int, int)>(&overloaded_func_t::f)>("f"_hs)
        .func<rendu::overload<int(int, int)>(&overloaded_func_t::f)>("f"_hs)
        .func<rendu::overload<int(int)>(&overloaded_func_t::f)>("f"_hs)
        .func<rendu::overload<int(int) const>(&overloaded_func_t::f)>("f"_hs)
        .func<rendu::overload<float(int, float)>(&overloaded_func_t::f)>("f"_hs)
        .func<&overloaded_func_t::g>("g"_hs);

    rendu::meta<property_t>()
        .type("property"_hs)
        .data<property_t::random>("random"_hs)
        .prop(static_cast<rendu::id_type>(property_t::random), 0)
        .prop(static_cast<rendu::id_type>(property_t::value), 3)
        .data<property_t::value>("value"_hs)
        .prop(static_cast<rendu::id_type>(property_t::random), true)
        .prop(static_cast<rendu::id_type>(property_t::value), 0)
        .prop(static_cast<rendu::id_type>(property_t::key_only))
        .prop(static_cast<rendu::id_type>(property_t::list))
        .data<property_t::key_only>("key_only"_hs)
        .prop(static_cast<rendu::id_type>(property_t::key_only))
        .data<property_t::list>("list"_hs)
        .prop(static_cast<rendu::id_type>(property_t::random), false)
        .prop(static_cast<rendu::id_type>(property_t::value), 0)
        .prop(static_cast<rendu::id_type>(property_t::key_only))
        .data<set<property_t>, get<property_t>>("var"_hs);

    rendu::meta<clazz_t>()
        .type("clazz"_hs)
        .prop(static_cast<rendu::id_type>(property_t::value), 42)
        .ctor<const base_t &, int>()
        .data<&clazz_t::value>("value"_hs)
        .func<&clazz_t::member>("member"_hs)
        .func<clazz_t::func>("func"_hs)
        .conv<int>();
  }

  void TearDown() override {
    rendu::meta_reset();
  }
};

RD_TEST_F(MetaType, Resolve) {
  using namespace rendu::literals;

  RD_ASSERT_EQ(rendu::resolve<double>(), rendu::resolve("double"_hs));
  RD_ASSERT_EQ(rendu::resolve<double>(), rendu::resolve(rendu::type_id<double>()));
  RD_ASSERT_FALSE(rendu::resolve(rendu::type_id<void>()));

  auto range = rendu::resolve();
  // it could be "char"_hs rather than rendu::hashed_string::value("char") if it weren't for a bug in VS2017
  const auto it = std::find_if(range.begin(),
                               range.end(),
                               [](auto curr) { return curr.second.id() == rendu::hashed_string::value("clazz"); });

  RD_ASSERT_NE(it, range.end());
  RD_ASSERT_EQ(it->second, rendu::resolve<clazz_t>());

  bool found = false;

  for (auto curr : rendu::resolve()) {
    found = found || curr.second == rendu::resolve<double>();
  }

  RD_ASSERT_TRUE(found);
}

RD_TEST_F(MetaType, Functionalities) {
  using namespace rendu::literals;

  auto type = rendu::resolve<clazz_t>();

  RD_ASSERT_TRUE(type);
  RD_ASSERT_NE(type, rendu::meta_type{});
  RD_ASSERT_EQ(type.id(), "clazz"_hs);
  RD_ASSERT_EQ(type.info(), rendu::type_id<clazz_t>());

  for (auto curr : type.prop()) {
    RD_ASSERT_EQ(curr.first, static_cast<rendu::id_type>(property_t::value));
    RD_ASSERT_EQ(curr.second.value(), 42);
  }

  RD_ASSERT_FALSE(type.prop(static_cast<rendu::id_type>(property_t::key_only)));
  RD_ASSERT_FALSE(type.prop("property"_hs));

  auto prop = type.prop(static_cast<rendu::id_type>(property_t::value));

  RD_ASSERT_TRUE(prop);
  RD_ASSERT_EQ(prop.value(), 42);
}

RD_TEST_F(MetaType, SizeOf) {
  RD_ASSERT_EQ(rendu::resolve<void>().size_of(), 0u);
  RD_ASSERT_EQ(rendu::resolve<int>().size_of(), sizeof(int));
  RD_ASSERT_EQ(rendu::resolve<int[]>().size_of(), 0u);
  RD_ASSERT_EQ(rendu::resolve<int[3]>().size_of(), sizeof(int[3]));
}

RD_TEST_F(MetaType, Traits) {
  RD_ASSERT_TRUE(rendu::resolve<bool>().is_arithmetic());
  RD_ASSERT_TRUE(rendu::resolve<double>().is_arithmetic());
  RD_ASSERT_FALSE(rendu::resolve<clazz_t>().is_arithmetic());

  RD_ASSERT_TRUE(rendu::resolve<int>().is_integral());
  RD_ASSERT_FALSE(rendu::resolve<double>().is_integral());
  RD_ASSERT_FALSE(rendu::resolve<clazz_t>().is_integral());

  RD_ASSERT_TRUE(rendu::resolve<long>().is_signed());
  RD_ASSERT_FALSE(rendu::resolve<unsigned int>().is_signed());
  RD_ASSERT_FALSE(rendu::resolve<clazz_t>().is_signed());

  RD_ASSERT_TRUE(rendu::resolve<int[5]>().is_array());
  RD_ASSERT_TRUE(rendu::resolve<int[5][3]>().is_array());
  RD_ASSERT_FALSE(rendu::resolve<int>().is_array());

  RD_ASSERT_TRUE(rendu::resolve<property_t>().is_enum());
  RD_ASSERT_FALSE(rendu::resolve<char>().is_enum());

  RD_ASSERT_TRUE(rendu::resolve<derived_t>().is_class());
  RD_ASSERT_FALSE(rendu::resolve<double>().is_class());

  RD_ASSERT_TRUE(rendu::resolve<int *>().is_pointer());
  RD_ASSERT_FALSE(rendu::resolve<int>().is_pointer());

  RD_ASSERT_TRUE(rendu::resolve<int *>().is_pointer_like());
  RD_ASSERT_TRUE(rendu::resolve<std::shared_ptr<int>>().is_pointer_like());
  RD_ASSERT_FALSE(rendu::resolve<int>().is_pointer_like());

  RD_ASSERT_FALSE((rendu::resolve<int>().is_sequence_container()));
  RD_ASSERT_TRUE(rendu::resolve<std::vector<int>>().is_sequence_container());
  RD_ASSERT_FALSE((rendu::resolve<std::map<int, char>>().is_sequence_container()));

  RD_ASSERT_FALSE((rendu::resolve<int>().is_associative_container()));
  RD_ASSERT_TRUE((rendu::resolve<std::map<int, char>>().is_associative_container()));
  RD_ASSERT_FALSE(rendu::resolve<std::vector<int>>().is_associative_container());
}

RD_TEST_F(MetaType, RemovePointer) {
  RD_ASSERT_EQ(rendu::resolve<void *>().remove_pointer(), rendu::resolve<void>());
  RD_ASSERT_EQ(rendu::resolve<char **>().remove_pointer(), rendu::resolve<char *>());
  RD_ASSERT_EQ(rendu::resolve<int (*)(char, double)>().remove_pointer(), rendu::resolve<int(char, double)>());
  RD_ASSERT_EQ(rendu::resolve<derived_t>().remove_pointer(), rendu::resolve<derived_t>());
}

RD_TEST_F(MetaType, TemplateInfo) {
  RD_ASSERT_FALSE(rendu::resolve<int>().is_template_specialization());
  RD_ASSERT_EQ(rendu::resolve<int>().template_arity(), 0u);
  RD_ASSERT_EQ(rendu::resolve<int>().template_type(), rendu::meta_type{});
  RD_ASSERT_EQ(rendu::resolve<int>().template_arg(0u), rendu::meta_type{});

  RD_ASSERT_TRUE(rendu::resolve<std::shared_ptr<int>>().is_template_specialization());
  RD_ASSERT_EQ(rendu::resolve<std::shared_ptr<int>>().template_arity(), 1u);
  RD_ASSERT_EQ(rendu::resolve<std::shared_ptr<int>>().template_type(),
               rendu::resolve<rendu::meta_class_template_tag<std::shared_ptr>>());
  RD_ASSERT_EQ(rendu::resolve<std::shared_ptr<int>>().template_arg(0u), rendu::resolve<int>());
  RD_ASSERT_EQ(rendu::resolve<std::shared_ptr<int>>().template_arg(1u), rendu::meta_type{});
}

RD_TEST_F(MetaType, Base) {
  using namespace rendu::literals;
  auto type = rendu::resolve<derived_t>();

  RD_ASSERT_NE(type.base().cbegin(), type.base().cend());

  for (auto curr : type.base()) {
    RD_ASSERT_EQ(curr.first, rendu::type_id<base_t>().hash());
    RD_ASSERT_EQ(curr.second, rendu::resolve<base_t>());
  }
}

RD_TEST_F(MetaType, Ctor) {
  derived_t derived;
  base_t &base = derived;
  auto type = rendu::resolve<clazz_t>();

  RD_ASSERT_TRUE((type.construct(rendu::forward_as_meta(derived), 42)));
  RD_ASSERT_TRUE((type.construct(rendu::forward_as_meta(base), 42)));

  // use the implicitly generated default constructor
  auto any = type.construct();

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.type(), rendu::resolve<clazz_t>());
}

RD_TEST_F(MetaType, Data) {
  using namespace rendu::literals;

  auto type = rendu::resolve<clazz_t>();
  int counter{};

  for ([[maybe_unused]] auto curr : type.data()) {
    ++counter;
  }

  RD_ASSERT_EQ(counter, 1);
  RD_ASSERT_TRUE(type.data("value"_hs));

  type = rendu::resolve<void>();

  RD_ASSERT_TRUE(type);
  RD_ASSERT_EQ(type.data().cbegin(), type.data().cend());
}

RD_TEST_F(MetaType, Func) {
  using namespace rendu::literals;

  auto type = rendu::resolve<clazz_t>();
  clazz_t instance{};
  int counter{};

  for ([[maybe_unused]] auto curr : type.func()) {
    ++counter;
  }

  RD_ASSERT_EQ(counter, 2);
  RD_ASSERT_TRUE(type.func("member"_hs));
  RD_ASSERT_TRUE(type.func("func"_hs));
  RD_ASSERT_TRUE(type.func("member"_hs).invoke(instance));
  RD_ASSERT_TRUE(type.func("func"_hs).invoke({}));

  type = rendu::resolve<void>();

  RD_ASSERT_TRUE(type);
  RD_ASSERT_EQ(type.func().cbegin(), type.func().cend());
}

RD_TEST_F(MetaType, Invoke) {
  using namespace rendu::literals;

  auto type = rendu::resolve<clazz_t>();
  clazz_t instance{};

  RD_ASSERT_TRUE(type.invoke("member"_hs, instance));
  RD_ASSERT_FALSE(type.invoke("rebmem"_hs, {}));
}

RD_TEST_F(MetaType, InvokeFromBase) {
  using namespace rendu::literals;

  auto type = rendu::resolve<concrete_t>();
  concrete_t instance{};

  RD_ASSERT_TRUE(type.invoke("base_only"_hs, instance, 42));
  RD_ASSERT_FALSE(type.invoke("ylno_esab"_hs, {}, 'c'));
}

RD_TEST_F(MetaType, OverloadedFunc) {
  using namespace rendu::literals;

  const auto type = rendu::resolve<overloaded_func_t>();
  overloaded_func_t instance{};
  rendu::meta_any res{};

  RD_ASSERT_TRUE(type.func("f"_hs));
  RD_ASSERT_TRUE(type.func("e"_hs));
  RD_ASSERT_TRUE(type.func("g"_hs));

  res = type.invoke("f"_hs, instance, base_t{}, 1, 2);

  RD_ASSERT_TRUE(res);
  RD_ASSERT_EQ(overloaded_func_t::value, 1);
  RD_ASSERT_NE(res.try_cast<int>(), nullptr);
  RD_ASSERT_EQ(res.cast<int>(), 4);

  res = type.invoke("f"_hs, instance, 3, 4);

  RD_ASSERT_TRUE(res);
  RD_ASSERT_EQ(overloaded_func_t::value, 3);
  RD_ASSERT_NE(res.try_cast<int>(), nullptr);
  RD_ASSERT_EQ(res.cast<int>(), 16);

  res = type.invoke("f"_hs, instance, 5);

  RD_ASSERT_TRUE(res);
  RD_ASSERT_EQ(overloaded_func_t::value, 3);
  RD_ASSERT_NE(res.try_cast<int>(), nullptr);
  RD_ASSERT_EQ(res.cast<int>(), 50);

  res = type.invoke("f"_hs, std::as_const(instance), 5);

  RD_ASSERT_TRUE(res);
  RD_ASSERT_EQ(overloaded_func_t::value, 3);
  RD_ASSERT_NE(res.try_cast<int>(), nullptr);
  RD_ASSERT_EQ(res.cast<int>(), 25);

  res = type.invoke("f"_hs, instance, 6, 7.f);

  RD_ASSERT_TRUE(res);
  RD_ASSERT_EQ(overloaded_func_t::value, 6);
  RD_ASSERT_NE(res.try_cast<float>(), nullptr);
  RD_ASSERT_EQ(res.cast<float>(), 14.f);

  res = type.invoke("f"_hs, instance, 8, 9.f);

  RD_ASSERT_TRUE(res);
  RD_ASSERT_EQ(overloaded_func_t::value, 8);
  RD_ASSERT_NE(res.try_cast<float>(), nullptr);
  RD_ASSERT_EQ(res.cast<float>(), 18.f);

  // it fails as an ambiguous call
  RD_ASSERT_FALSE(type.invoke("f"_hs, instance, 8, 9.));
}

RD_TEST_F(MetaType, Construct) {
  auto any = rendu::resolve<clazz_t>().construct(base_t{}, 42);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<clazz_t>().value, 42);
}

RD_TEST_F(MetaType, ConstructNoArgs) {
  // this should work, no other tests required
  auto any = rendu::resolve<clazz_t>().construct();

  RD_ASSERT_TRUE(any);
}

RD_TEST_F(MetaType, ConstructMetaAnyArgs) {
  auto any = rendu::resolve<clazz_t>().construct(rendu::meta_any{base_t{}}, rendu::meta_any{42});

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<clazz_t>().value, 42);
}

RD_TEST_F(MetaType, ConstructInvalidArgs) {
  RD_ASSERT_FALSE(rendu::resolve<clazz_t>().construct('c', base_t{}));
}

RD_TEST_F(MetaType, LessArgs) {
  RD_ASSERT_FALSE(rendu::resolve<clazz_t>().construct(base_t{}));
}

RD_TEST_F(MetaType, ConstructCastAndConvert) {
  auto any = rendu::resolve<clazz_t>().construct(derived_t{}, clazz_t{derived_t{}, 42});

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<clazz_t>().value, 42);
}

RD_TEST_F(MetaType, ConstructArithmeticConversion) {
  auto any = rendu::resolve<clazz_t>().construct(derived_t{}, clazz_t{derived_t{}, true});

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.cast<clazz_t>().value, 1);
}

RD_TEST_F(MetaType, FromVoid) {
  using namespace rendu::literals;

  RD_ASSERT_FALSE(rendu::resolve<double>().from_void(static_cast<double *>(nullptr)));
  RD_ASSERT_FALSE(rendu::resolve<double>().from_void(static_cast<const double *>(nullptr)));

  auto type = rendu::resolve<double>();
  double value = 4.2;

  RD_ASSERT_FALSE(rendu::resolve<void>().from_void(static_cast<void *>(&value)));
  RD_ASSERT_FALSE(rendu::resolve<void>().from_void(static_cast<const void *>(&value)));

  auto as_void = type.from_void(static_cast<void *>(&value));
  auto as_const_void = type.from_void(static_cast<const void *>(&value));

  RD_ASSERT_TRUE(as_void);
  RD_ASSERT_TRUE(as_const_void);

  RD_ASSERT_EQ(as_void.type(), rendu::resolve<double>());
  RD_ASSERT_NE(as_void.try_cast<double>(), nullptr);

  RD_ASSERT_EQ(as_const_void.type(), rendu::resolve<double>());
  RD_ASSERT_EQ(as_const_void.try_cast<double>(), nullptr);
  RD_ASSERT_NE(as_const_void.try_cast<const double>(), nullptr);

  value = 1.2;

  RD_ASSERT_EQ(as_void.cast<double>(), as_const_void.cast<double>());
  RD_ASSERT_EQ(as_void.cast<double>(), 1.2);
}

RD_TEST_F(MetaType, Reset) {
  using namespace rendu::literals;

  RD_ASSERT_TRUE(rendu::resolve("clazz"_hs));
  RD_ASSERT_EQ(rendu::resolve<clazz_t>().id(), "clazz"_hs);
  RD_ASSERT_TRUE(rendu::resolve<clazz_t>().prop(static_cast<rendu::id_type>(property_t::value)));
  RD_ASSERT_TRUE(rendu::resolve<clazz_t>().data("value"_hs));
  RD_ASSERT_TRUE((rendu::resolve<clazz_t>().construct(derived_t{}, clazz_t{})));
  // implicitly generated default constructor
  RD_ASSERT_TRUE(rendu::resolve<clazz_t>().construct());

  rendu::meta_reset("clazz"_hs);

  RD_ASSERT_FALSE(rendu::resolve("clazz"_hs));
  RD_ASSERT_NE(rendu::resolve<clazz_t>().id(), "clazz"_hs);
  RD_ASSERT_FALSE(rendu::resolve<clazz_t>().prop(static_cast<rendu::id_type>(property_t::value)));
  RD_ASSERT_FALSE(rendu::resolve<clazz_t>().data("value"_hs));
  RD_ASSERT_FALSE((rendu::resolve<clazz_t>().construct(derived_t{}, clazz_t{})));
  // implicitly generated default constructor is not cleared
  RD_ASSERT_TRUE(rendu::resolve<clazz_t>().construct());

  rendu::meta<clazz_t>().type("clazz"_hs);

  RD_ASSERT_TRUE(rendu::resolve("clazz"_hs));
}

RD_TEST_F(MetaType, ResetLast) {
  auto id = (rendu::resolve().cend() - 1u)->second.id();

  RD_ASSERT_TRUE(rendu::resolve(id));

  rendu::meta_reset(id);

  RD_ASSERT_FALSE(rendu::resolve(id));
}

RD_TEST_F(MetaType, ResetAll) {
  using namespace rendu::literals;

  RD_ASSERT_NE(rendu::resolve().begin(), rendu::resolve().end());

  RD_ASSERT_TRUE(rendu::resolve("clazz"_hs));
  RD_ASSERT_TRUE(rendu::resolve("overloaded_func"_hs));
  RD_ASSERT_TRUE(rendu::resolve("double"_hs));

  rendu::meta_reset();

  RD_ASSERT_FALSE(rendu::resolve("clazz"_hs));
  RD_ASSERT_FALSE(rendu::resolve("overloaded_func"_hs));
  RD_ASSERT_FALSE(rendu::resolve("double"_hs));

  RD_ASSERT_EQ(rendu::resolve().begin(), rendu::resolve().end());
}

RD_TEST_F(MetaType, AbstractClass) {
  using namespace rendu::literals;

  auto type = rendu::resolve<abstract_t>();
  concrete_t instance;

  RD_ASSERT_EQ(type.info(), rendu::type_id<abstract_t>());
  RD_ASSERT_EQ(instance.base_t::value, 'c');
  RD_ASSERT_EQ(instance.value, 3);

  type.func("func"_hs).invoke(instance, 42);

  RD_ASSERT_EQ(instance.base_t::value, 'c');
  RD_ASSERT_EQ(instance.value, 42);
}

RD_TEST_F(MetaType, EnumAndNamedConstants) {
  using namespace rendu::literals;

  auto type = rendu::resolve<property_t>();

  RD_ASSERT_TRUE(type.data("random"_hs));
  RD_ASSERT_TRUE(type.data("value"_hs));

  RD_ASSERT_EQ(type.data("random"_hs).type(), type);
  RD_ASSERT_EQ(type.data("value"_hs).type(), type);

  RD_ASSERT_FALSE(type.data("random"_hs).set({}, property_t::value));
  RD_ASSERT_FALSE(type.data("value"_hs).set({}, property_t::random));

  RD_ASSERT_EQ(type.data("random"_hs).get({}).cast<property_t>(), property_t::random);
  RD_ASSERT_EQ(type.data("value"_hs).get({}).cast<property_t>(), property_t::value);
}

RD_TEST_F(MetaType, ArithmeticTypeAndNamedConstants) {
  using namespace rendu::literals;

  auto type = rendu::resolve<unsigned int>();

  RD_ASSERT_TRUE(type.data("min"_hs));
  RD_ASSERT_TRUE(type.data("max"_hs));

  RD_ASSERT_EQ(type.data("min"_hs).type(), type);
  RD_ASSERT_EQ(type.data("max"_hs).type(), type);

  RD_ASSERT_FALSE(type.data("min"_hs).set({}, 100u));
  RD_ASSERT_FALSE(type.data("max"_hs).set({}, 0u));

  RD_ASSERT_EQ(type.data("min"_hs).get({}).cast<unsigned int>(), 0u);
  RD_ASSERT_EQ(type.data("max"_hs).get({}).cast<unsigned int>(), 100u);
}

RD_TEST_F(MetaType, Variables) {
  using namespace rendu::literals;

  auto p_data = rendu::resolve<property_t>().data("var"_hs);
  auto d_data = rendu::resolve("double"_hs).data("var"_hs);

  property_t prop{property_t::key_only};
  double d = 3.;

  p_data.set(prop, property_t::random);
  d_data.set(d, 42.);

  RD_ASSERT_EQ(p_data.get(prop).cast<property_t>(), property_t::random);
  RD_ASSERT_EQ(d_data.get(d).cast<double>(), 42.);
  RD_ASSERT_EQ(prop, property_t::random);
  RD_ASSERT_EQ(d, 42.);
}

RD_TEST_F(MetaType, PropertiesAndCornerCases) {
  using namespace rendu::literals;

  auto type = rendu::resolve<property_t>();

  RD_ASSERT_EQ(type.prop().cbegin(), type.prop().cend());

  RD_ASSERT_EQ(type.data("random"_hs).prop(static_cast<rendu::id_type>(property_t::random)).value().cast<int>(), 0);
  RD_ASSERT_EQ(type.data("random"_hs).prop(static_cast<rendu::id_type>(property_t::value)).value().cast<int>(), 3);

  RD_ASSERT_EQ(type.data("value"_hs).prop(static_cast<rendu::id_type>(property_t::random)).value().cast<bool>(), true);
  RD_ASSERT_EQ(type.data("value"_hs).prop(static_cast<rendu::id_type>(property_t::value)).value().cast<int>(), 0);
  RD_ASSERT_TRUE(type.data("value"_hs).prop(static_cast<rendu::id_type>(property_t::key_only)));
  RD_ASSERT_FALSE(type.data("value"_hs).prop(static_cast<rendu::id_type>(property_t::key_only)).value());

  RD_ASSERT_TRUE(type.data("key_only"_hs).prop(static_cast<rendu::id_type>(property_t::key_only)));
  RD_ASSERT_FALSE(type.data("key_only"_hs).prop(static_cast<rendu::id_type>(property_t::key_only)).value());

  RD_ASSERT_EQ(type.data("list"_hs).prop(static_cast<rendu::id_type>(property_t::random)).value().cast<bool>(), false);
  RD_ASSERT_EQ(type.data("list"_hs).prop(static_cast<rendu::id_type>(property_t::value)).value().cast<int>(), 0);
  RD_ASSERT_TRUE(type.data("list"_hs).prop(static_cast<rendu::id_type>(property_t::key_only)));
  RD_ASSERT_FALSE(type.data("list"_hs).prop(static_cast<rendu::id_type>(property_t::key_only)).value());

  type = rendu::resolve<void>();

  RD_ASSERT_EQ(type.prop().cbegin(), type.prop().cend());
}

RD_TEST_F(MetaType, ResetAndReRegistrationAfterReset) {
  using namespace rendu::literals;

  RD_ASSERT_FALSE(rendu::internal::meta_context::from(rendu::locator<rendu::meta_ctx>::value_or()).value.empty());

  rendu::meta_reset<double>();
  rendu::meta_reset<unsigned int>();
  rendu::meta_reset<base_t>();
  rendu::meta_reset<derived_t>();
  rendu::meta_reset<abstract_t>();
  rendu::meta_reset<concrete_t>();
  rendu::meta_reset<overloaded_func_t>();
  rendu::meta_reset<property_t>();
  rendu::meta_reset<clazz_t>();

  RD_ASSERT_FALSE(rendu::resolve("double"_hs));
  RD_ASSERT_FALSE(rendu::resolve("base"_hs));
  RD_ASSERT_FALSE(rendu::resolve("derived"_hs));
  RD_ASSERT_FALSE(rendu::resolve("clazz"_hs));

  RD_ASSERT_TRUE(rendu::internal::meta_context::from(rendu::locator<rendu::meta_ctx>::value_or()).value.empty());

  RD_ASSERT_FALSE(rendu::resolve<clazz_t>().prop(static_cast<rendu::id_type>(property_t::value)));
  // implicitly generated default constructor is not cleared
  RD_ASSERT_TRUE(rendu::resolve<clazz_t>().construct());
  RD_ASSERT_FALSE(rendu::resolve<clazz_t>().data("value"_hs));
  RD_ASSERT_FALSE(rendu::resolve<clazz_t>().func("member"_hs));

  rendu::meta<double>().type("double"_hs);
  rendu::meta_any any{42.};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(any.allow_cast<int>());
  RD_ASSERT_TRUE(any.allow_cast<float>());

  RD_ASSERT_FALSE(rendu::resolve("derived"_hs));
  RD_ASSERT_TRUE(rendu::resolve("double"_hs));

  rendu::meta<property_t>()
      .type("property"_hs)
      .data<property_t::random>("rand"_hs)
      .prop(static_cast<rendu::id_type>(property_t::value), 42)
      .prop(static_cast<rendu::id_type>(property_t::random), 3);

  RD_ASSERT_TRUE(rendu::resolve<property_t>().data("rand"_hs).prop(static_cast<rendu::id_type>(property_t::value)));
  RD_ASSERT_TRUE(rendu::resolve<property_t>().data("rand"_hs).prop(static_cast<rendu::id_type>(property_t::random)));
}

RD_TEST_F(MetaType, ReRegistration) {
  using namespace rendu::literals;

  int count = 0;

  for ([[maybe_unused]] auto type : rendu::resolve()) {
    ++count;
  }

  SetUp();

  for ([[maybe_unused]] auto type : rendu::resolve()) {
    --count;
  }

  RD_ASSERT_EQ(count, 0);
  RD_ASSERT_TRUE(rendu::resolve("double"_hs));

  rendu::meta<double>().type("real"_hs);

  RD_ASSERT_FALSE(rendu::resolve("double"_hs));
  RD_ASSERT_TRUE(rendu::resolve("real"_hs));
  RD_ASSERT_TRUE(rendu::resolve("real"_hs).data("var"_hs));
}

RD_TEST_F(MetaType, NameCollision) {
  using namespace rendu::literals;

  RD_ASSERT_NO_FATAL_FAILURE(rendu::meta<clazz_t>().type("clazz"_hs));
  RD_ASSERT_TRUE(rendu::resolve("clazz"_hs));

  RD_ASSERT_NO_FATAL_FAILURE(rendu::meta<clazz_t>().type("quux"_hs));
  RD_ASSERT_FALSE(rendu::resolve("clazz"_hs));
  RD_ASSERT_TRUE(rendu::resolve("quux"_hs));
}

using MetaTypeDeathTest = MetaType;
RD_DEBUG_TEST_F(MetaTypeDeathTest, NameCollision) {
  using namespace rendu::literals;

  RD_ASSERT_DEATH(rendu::meta<clazz_t>().type("abstract"_hs), "");
}
}
