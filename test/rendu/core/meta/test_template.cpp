/*
* Created by boil on 2023/2/26.
*/

#include <test/rdtest.h>
#include <core/base/type_traits.h>
#include <core/meta/meta.h>
#include <core/meta/resolve.h>
#include <core/meta/template.h>

template<typename>
struct function_type;

template<typename Ret, typename... Args>
struct function_type<Ret(Args...)> {};

template<typename Ret, typename... Args>
struct rendu::meta_template_traits<function_type<Ret(Args...)>> {
  using class_type = meta_class_template_tag<function_type>;
  using args_type = type_list<Ret, Args...>;
};

RD_TEST(MetaTemplate, Invalid) {
  const auto type = rendu::resolve<int>();

  RD_ASSERT_FALSE(type.is_template_specialization());
  RD_ASSERT_EQ(type.template_arity(), 0u);
  RD_ASSERT_EQ(type.template_type(), rendu::meta_type{});
  RD_ASSERT_EQ(type.template_arg(0u), rendu::meta_type{});
}

RD_TEST(MetaTemplate, Valid) {
  const auto type = rendu::resolve<rendu::type_list<int, char>>();

  RD_ASSERT_TRUE(type.is_template_specialization());
  RD_ASSERT_EQ(type.template_arity(), 2u);
  RD_ASSERT_EQ(type.template_type(), rendu::resolve<rendu::meta_class_template_tag<rendu::type_list>>());
  RD_ASSERT_EQ(type.template_arg(0u), rendu::resolve<int>());
  RD_ASSERT_EQ(type.template_arg(1u), rendu::resolve<char>());
  RD_ASSERT_EQ(type.template_arg(2u), rendu::meta_type{});
}

RD_TEST(MetaTemplate, CustomTraits) {
  const auto type = rendu::resolve<function_type<void(int, const char &)>>();

  RD_ASSERT_TRUE(type.is_template_specialization());
  RD_ASSERT_EQ(type.template_arity(), 3u);
  RD_ASSERT_EQ(type.template_type(), rendu::resolve<rendu::meta_class_template_tag<function_type>>());
  RD_ASSERT_EQ(type.template_arg(0u), rendu::resolve<void>());
  RD_ASSERT_EQ(type.template_arg(1u), rendu::resolve<int>());
  RD_ASSERT_EQ(type.template_arg(2u), rendu::resolve<char>());
  RD_ASSERT_EQ(type.template_arg(3u), rendu::meta_type{});
}

