/*
* Created by boil on 2023/2/25.
*/

#include <unordered_map>
#include <vector>
#include <test/rdtest.h>
#include <core/meta/context.h>
#include <core/meta/pointer.h>
#include <core/meta/template.h>
#include <core/meta/container.h>
#include <core/meta/factory.h>
#include <core/base/hashed_string.h>

namespace meta_context {

struct base {
  base() = default;

  base(char v)
      : value{v} {}

  char get() const {
    return value;
  }

  char value;
};

struct clazz : base {
  clazz() = default;

  clazz(int v)
      : base{},
        value{v} {}

  clazz(char c, int v)
      : base{c},
        value{v} {}

  int func(int v) {
    return (value = v);
  }

  int cfunc(int v) const {
    return v;
  }

  static void move_to_bucket(const clazz &instance) {
    bucket = instance.value;
  }

  int value{};
  static inline int bucket{};
};

struct local_only {};

struct argument {
  argument(int val)
      : value{val} {}

  int get() const {
    return value;
  }

  int get_mul() const {
    return value * 2;
  }

 private:
  int value{};
};

template<typename...>
struct template_clazz {};

class MetaContext : public ::testing::Test {
  void init_global_context() {
    using namespace rendu::literals;

    rendu::meta<int>()
        .data<global_marker>("marker"_hs);

    rendu::meta<argument>()
        .conv<&argument::get>();

    rendu::meta<clazz>()
        .type("foo"_hs)
        .prop("prop"_hs, prop_value)
        .ctor<int>()
        .data<&clazz::value>("value"_hs)
        .data<&clazz::value>("rw"_hs)
        .func<&clazz::func>("func"_hs);

    rendu::meta<template_clazz<int>>()
        .type("template"_hs);
  }

  void init_local_context() {
    using namespace rendu::literals;

    rendu::meta<int>(context)
        .data<local_marker>("marker"_hs);

    rendu::meta<local_only>(context)
        .type("quux"_hs);

    rendu::meta<argument>(context)
        .conv<&argument::get_mul>();

    rendu::meta<base>(context)
        .data<&base::value>("char"_hs)
        .func<&base::get>("get"_hs);

    rendu::meta<clazz>(context)
        .type("bar"_hs)
        .prop("prop"_hs, prop_value)
        .base<base>()
        .ctor<char, int>()
        .dtor<&clazz::move_to_bucket>()
        .data<nullptr, &clazz::value>("value"_hs)
        .data<&clazz::value>("rw"_hs)
        .func<&clazz::cfunc>("func"_hs);

    rendu::meta<template_clazz<int, char>>(context)
        .type("template"_hs);
  }

 public:
  void SetUp() override {
    init_global_context();
    init_local_context();

    clazz::bucket = bucket_value;
  }

  void TearDown() override {
    rendu::meta_reset(context);
    rendu::meta_reset();
  }

 protected:
  static constexpr int global_marker = 1;
  static constexpr int local_marker = 42;
  static constexpr int bucket_value = 99;
  static constexpr int prop_value = 3;
  rendu::meta_ctx context{};
};

RD_TEST_F(MetaContext, Resolve) {
  using namespace rendu::literals;

  RD_ASSERT_TRUE(rendu::resolve<clazz>());
  RD_ASSERT_TRUE(rendu::resolve<clazz>(context));

  RD_ASSERT_TRUE(rendu::resolve<local_only>());
  RD_ASSERT_TRUE(rendu::resolve<local_only>(context));

  RD_ASSERT_TRUE(rendu::resolve(rendu::type_id<clazz>()));
  RD_ASSERT_TRUE(rendu::resolve(context, rendu::type_id<clazz>()));

  RD_ASSERT_FALSE(rendu::resolve(rendu::type_id<local_only>()));
  RD_ASSERT_TRUE(rendu::resolve(context, rendu::type_id<local_only>()));

  RD_ASSERT_TRUE(rendu::resolve("foo"_hs));
  RD_ASSERT_FALSE(rendu::resolve(context, "foo"_hs));

  RD_ASSERT_FALSE(rendu::resolve("bar"_hs));
  RD_ASSERT_TRUE(rendu::resolve(context, "bar"_hs));

  RD_ASSERT_FALSE(rendu::resolve("quux"_hs));
  RD_ASSERT_TRUE(rendu::resolve(context, "quux"_hs));

  RD_ASSERT_EQ((std::distance(rendu::resolve().cbegin(), rendu::resolve().cend())), 4);
  RD_ASSERT_EQ((std::distance(rendu::resolve(context).cbegin(), rendu::resolve(context).cend())), 6);
}

RD_TEST_F(MetaContext, MetaType) {
  using namespace rendu::literals;

  const auto global = rendu::resolve<clazz>();
  const auto local = rendu::resolve<clazz>(context);

  RD_ASSERT_TRUE(global);
  RD_ASSERT_TRUE(local);

  RD_ASSERT_NE(global, local);

  RD_ASSERT_EQ(global, rendu::resolve("foo"_hs));
  RD_ASSERT_EQ(local, rendu::resolve(context, "bar"_hs));

  RD_ASSERT_EQ(global.id(), "foo"_hs);
  RD_ASSERT_EQ(local.id(), "bar"_hs);

  clazz instance{'c', 99};
  const argument value{2};

  RD_ASSERT_NE(instance.value, value.get());
  RD_ASSERT_EQ(global.invoke("func"_hs, instance, value).cast<int>(), value.get());
  RD_ASSERT_EQ(instance.value, value.get());

  RD_ASSERT_NE(instance.value, value.get_mul());
  RD_ASSERT_EQ(local.invoke("func"_hs, instance, value).cast<int>(), value.get_mul());
  RD_ASSERT_NE(instance.value, value.get_mul());

  RD_ASSERT_FALSE(global.invoke("get"_hs, instance));
  RD_ASSERT_EQ(local.invoke("get"_hs, instance).cast<char>(), 'c');
}

RD_TEST_F(MetaContext, MetaBase) {
  const auto global = rendu::resolve<clazz>();
  const auto local = rendu::resolve<clazz>(context);

  RD_ASSERT_EQ((std::distance(global.base().cbegin(), global.base().cend())), 0);
  RD_ASSERT_EQ((std::distance(local.base().cbegin(), local.base().cend())), 1);

  RD_ASSERT_EQ(local.base().cbegin()->second.info(), rendu::type_id<base>());

  RD_ASSERT_FALSE(rendu::resolve(rendu::type_id<base>()));
  RD_ASSERT_TRUE(rendu::resolve(context, rendu::type_id<base>()));
}

RD_TEST_F(MetaContext, MetaData) {
  using namespace rendu::literals;

  const auto global = rendu::resolve<clazz>();
  const auto local = rendu::resolve<clazz>(context);

  RD_ASSERT_TRUE(global.data("value"_hs));
  RD_ASSERT_TRUE(local.data("value"_hs));

  RD_ASSERT_FALSE(global.data("value"_hs).is_const());
  RD_ASSERT_TRUE(local.data("value"_hs).is_const());

  RD_ASSERT_EQ(global.data("value"_hs).type().data("marker"_hs).get({}).cast<int>(), global_marker);
  RD_ASSERT_EQ(local.data("value"_hs).type().data("marker"_hs).get({}).cast<int>(), local_marker);

  RD_ASSERT_EQ(global.data("rw"_hs).arg(0u).data("marker"_hs).get({}).cast<int>(), global_marker);
  RD_ASSERT_EQ(local.data("rw"_hs).arg(0u).data("marker"_hs).get({}).cast<int>(), local_marker);

  clazz instance{'c', 99};
  const argument value{2};

  RD_ASSERT_NE(instance.value, value.get());
  RD_ASSERT_TRUE(global.data("rw"_hs).set(instance, value));
  RD_ASSERT_EQ(instance.value, value.get());

  RD_ASSERT_NE(instance.value, value.get_mul());
  RD_ASSERT_TRUE(local.data("rw"_hs).set(instance, value));
  RD_ASSERT_EQ(instance.value, value.get_mul());

  RD_ASSERT_FALSE(global.data("char"_hs));
  RD_ASSERT_EQ(local.data("char"_hs).get(instance).cast<char>(), 'c');
  RD_ASSERT_TRUE(local.data("char"_hs).set(instance, 'x'));
  RD_ASSERT_EQ(instance.base::value, 'x');
}

RD_TEST_F(MetaContext, MetaFunc) {
  using namespace rendu::literals;

  const auto global = rendu::resolve<clazz>();
  const auto local = rendu::resolve<clazz>(context);

  RD_ASSERT_TRUE(global.func("func"_hs));
  RD_ASSERT_TRUE(local.func("func"_hs));

  RD_ASSERT_FALSE(global.func("func"_hs).is_const());
  RD_ASSERT_TRUE(local.func("func"_hs).is_const());

  RD_ASSERT_EQ(global.func("func"_hs).arg(0u).data("marker"_hs).get({}).cast<int>(), global_marker);
  RD_ASSERT_EQ(local.func("func"_hs).arg(0u).data("marker"_hs).get({}).cast<int>(), local_marker);

  RD_ASSERT_EQ(global.func("func"_hs).ret().data("marker"_hs).get({}).cast<int>(), global_marker);
  RD_ASSERT_EQ(local.func("func"_hs).ret().data("marker"_hs).get({}).cast<int>(), local_marker);

  clazz instance{'c', 99};
  const argument value{2};

  RD_ASSERT_NE(instance.value, value.get());
  RD_ASSERT_EQ(global.func("func"_hs).invoke(instance, value).cast<int>(), value.get());
  RD_ASSERT_EQ(instance.value, value.get());

  RD_ASSERT_NE(instance.value, value.get_mul());
  RD_ASSERT_EQ(local.func("func"_hs).invoke(instance, value).cast<int>(), value.get_mul());
  RD_ASSERT_NE(instance.value, value.get_mul());

  RD_ASSERT_FALSE(global.func("get"_hs));
  RD_ASSERT_EQ(local.func("get"_hs).invoke(instance).cast<char>(), 'c');
}

RD_TEST_F(MetaContext, MetaCtor) {
  const auto global = rendu::resolve<clazz>();
  const auto local = rendu::resolve<clazz>(context);

  auto any = global.construct();
  auto other = local.construct();

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(other);

  RD_ASSERT_EQ(any.cast<const clazz &>().value, 0);
  RD_ASSERT_EQ(other.cast<const clazz &>().value, 0);

  argument argument{2};

  any = global.construct(argument);
  other = local.construct(argument);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_FALSE(other);
  RD_ASSERT_EQ(any.cast<const clazz &>().value, 2);

  any = global.construct('c', argument);
  other = local.construct('c', argument);

  RD_ASSERT_FALSE(any);
  RD_ASSERT_TRUE(other);
  RD_ASSERT_EQ(other.cast<const clazz &>().value, 4);
}

RD_TEST_F(MetaContext, MetaConv) {
  argument value{2};

  auto global = rendu::forward_as_meta(value);
  auto local = rendu::forward_as_meta(context, value);

  RD_ASSERT_TRUE(global.allow_cast<int>());
  RD_ASSERT_TRUE(local.allow_cast<int>());

  RD_ASSERT_EQ(global.cast<int>(), value.get());
  RD_ASSERT_EQ(local.cast<int>(), value.get_mul());
}

RD_TEST_F(MetaContext, MetaDtor) {
  auto global = rendu::resolve<clazz>().construct();
  auto local = rendu::resolve<clazz>(context).construct();

  RD_ASSERT_EQ(clazz::bucket, bucket_value);

  global.reset();

  RD_ASSERT_EQ(clazz::bucket, bucket_value);

  local.reset();

  RD_ASSERT_NE(clazz::bucket, bucket_value);
}

RD_TEST_F(MetaContext, MetaProp) {
  using namespace rendu::literals;

  const auto global = rendu::resolve<clazz>();
  const auto local = rendu::resolve<clazz>(context);

  RD_ASSERT_TRUE(global.prop("prop"_hs));
  RD_ASSERT_TRUE(local.prop("prop"_hs));

  RD_ASSERT_EQ(global.prop("prop"_hs).value().type(), rendu::resolve<int>());
  RD_ASSERT_EQ(local.prop("prop"_hs).value().type(), rendu::resolve<int>(context));

  RD_ASSERT_EQ(global.prop("prop"_hs).value().cast<int>(), prop_value);
  RD_ASSERT_EQ(local.prop("prop"_hs).value().cast<int>(), prop_value);

  RD_ASSERT_EQ(global.prop("prop"_hs).value().type().data("marker"_hs).get({}).cast<int>(), global_marker);
  RD_ASSERT_EQ(local.prop("prop"_hs).value().type().data("marker"_hs).get({}).cast<int>(), local_marker);
}

RD_TEST_F(MetaContext, MetaTemplate) {
  using namespace rendu::literals;

  const auto global = rendu::resolve("template"_hs);
  const auto local = rendu::resolve(context, "template"_hs);

  RD_ASSERT_TRUE(global.is_template_specialization());
  RD_ASSERT_TRUE(local.is_template_specialization());

  RD_ASSERT_EQ(global.template_arity(), 1u);
  RD_ASSERT_EQ(local.template_arity(), 2u);

  RD_ASSERT_EQ(global.template_arg(0u), rendu::resolve<int>());
  RD_ASSERT_EQ(local.template_arg(0u), rendu::resolve<int>(context));
  RD_ASSERT_EQ(local.template_arg(1u), rendu::resolve<char>(context));

  RD_ASSERT_EQ(global.template_arg(0u).data("marker"_hs).get({}).cast<int>(), global_marker);
  RD_ASSERT_EQ(local.template_arg(0u).data("marker"_hs).get({}).cast<int>(), local_marker);
}

RD_TEST_F(MetaContext, MetaPointer) {
  using namespace rendu::literals;

  int value = 42;

  const rendu::meta_any global{&value};
  const rendu::meta_any local{context, &value};

  RD_ASSERT_TRUE(global.type().is_pointer());
  RD_ASSERT_TRUE(local.type().is_pointer());

  RD_ASSERT_TRUE(global.type().is_pointer_like());
  RD_ASSERT_TRUE(local.type().is_pointer_like());

  RD_ASSERT_EQ((*global).type().data("marker"_hs).get({}).cast<int>(), global_marker);
  RD_ASSERT_EQ((*local).type().data("marker"_hs).get({}).cast<int>(), local_marker);
}

RD_TEST_F(MetaContext, MetaAssociativeContainer) {
  using namespace rendu::literals;

  std::unordered_map<int, int> map{{{0, 0}}};

  auto global = rendu::forward_as_meta(map).as_associative_container();
  auto local = rendu::forward_as_meta(context, map).as_associative_container();

  RD_ASSERT_TRUE(global);
  RD_ASSERT_TRUE(local);

  RD_ASSERT_EQ(global.size(), 1u);
  RD_ASSERT_EQ(local.size(), 1u);

  RD_ASSERT_EQ(global.key_type().data("marker"_hs).get({}).cast<int>(), global_marker);
  RD_ASSERT_EQ(local.key_type().data("marker"_hs).get({}).cast<int>(), local_marker);

  RD_ASSERT_EQ(global.mapped_type().data("marker"_hs).get({}).cast<int>(), global_marker);
  RD_ASSERT_EQ(local.mapped_type().data("marker"_hs).get({}).cast<int>(), local_marker);

  RD_ASSERT_EQ((*global.begin()).first.type().data("marker"_hs).get({}).cast<int>(), global_marker);
  RD_ASSERT_EQ((*local.begin()).first.type().data("marker"_hs).get({}).cast<int>(), local_marker);

  RD_ASSERT_EQ((*global.begin()).second.type().data("marker"_hs).get({}).cast<int>(), global_marker);
  RD_ASSERT_EQ((*local.begin()).second.type().data("marker"_hs).get({}).cast<int>(), local_marker);
}

RD_TEST_F(MetaContext, MetaSequenceContainer) {
  using namespace rendu::literals;

  std::vector<int> vec{0};

  auto global = rendu::forward_as_meta(vec).as_sequence_container();
  auto local = rendu::forward_as_meta(context, vec).as_sequence_container();

  RD_ASSERT_TRUE(global);
  RD_ASSERT_TRUE(local);

  RD_ASSERT_EQ(global.size(), 1u);
  RD_ASSERT_EQ(local.size(), 1u);

  RD_ASSERT_EQ(global.value_type().data("marker"_hs).get({}).cast<int>(), global_marker);
  RD_ASSERT_EQ(local.value_type().data("marker"_hs).get({}).cast<int>(), local_marker);

  RD_ASSERT_EQ((*global.begin()).type().data("marker"_hs).get({}).cast<int>(), global_marker);
  RD_ASSERT_EQ((*local.begin()).type().data("marker"_hs).get({}).cast<int>(), local_marker);
}

RD_TEST_F(MetaContext, MetaAny) {
  using namespace rendu::literals;

  rendu::meta_any global{42};
  rendu::meta_any ctx_value{context, 42};
  rendu::meta_any in_place{context, std::in_place_type<int>, 42};
  rendu::meta_any two_step_local{rendu::meta_ctx_arg, context};

  RD_ASSERT_TRUE(global);
  RD_ASSERT_TRUE(ctx_value);
  RD_ASSERT_TRUE(in_place);
  RD_ASSERT_FALSE(two_step_local);

  two_step_local = 42;

  RD_ASSERT_TRUE(two_step_local);

  RD_ASSERT_EQ(global.type().data("marker"_hs).get({}).cast<int>(), global_marker);
  RD_ASSERT_EQ(ctx_value.type().data("marker"_hs).get({}).cast<int>(), local_marker);
  RD_ASSERT_EQ(in_place.type().data("marker"_hs).get({}).cast<int>(), local_marker);
  RD_ASSERT_EQ(two_step_local.type().data("marker"_hs).get({}).cast<int>(), local_marker);
}

RD_TEST_F(MetaContext, MetaHandle) {
  using namespace rendu::literals;

  int value = 42;

  rendu::meta_handle global{value};
  rendu::meta_handle ctx_value{context, value};
  rendu::meta_handle two_step_local{rendu::meta_ctx_arg, context};

  RD_ASSERT_TRUE(global);
  RD_ASSERT_TRUE(ctx_value);
  RD_ASSERT_FALSE(two_step_local);

  two_step_local->emplace<int &>(value);

  RD_ASSERT_TRUE(two_step_local);

  RD_ASSERT_EQ(global->type().data("marker"_hs).get({}).cast<int>(), global_marker);
  RD_ASSERT_EQ(ctx_value->type().data("marker"_hs).get({}).cast<int>(), local_marker);
  RD_ASSERT_EQ(two_step_local->type().data("marker"_hs).get({}).cast<int>(), local_marker);
}

RD_TEST_F(MetaContext, ForwardAsMeta) {
  using namespace rendu::literals;

  const auto global = rendu::forward_as_meta(42);
  const auto local = rendu::forward_as_meta(context, 42);

  RD_ASSERT_TRUE(global);
  RD_ASSERT_TRUE(local);

  RD_ASSERT_EQ(global.type().data("marker"_hs).get({}).cast<int>(), global_marker);
  RD_ASSERT_EQ(local.type().data("marker"_hs).get({}).cast<int>(), local_marker);
}
}
