/*
* Created by boil on 2023/2/25.
*/

#include <test/rdtest.h>
#include <core/base/type_traits.h>
#include <core/base/utility.h>

struct functions {
  static void foo(int) {}
  static void foo() {}

  void bar(int) {}
  void bar() {}
};

RD_TEST(Identity, Functionalities) {
  rendu::identity identity;
  int value = 42;

  RD_ASSERT_TRUE(rendu::is_transparent_v<rendu::identity>);
  RD_ASSERT_EQ(identity(value), value);
  RD_ASSERT_EQ(&identity(value), &value);
}

RD_TEST(Overload, Functionalities) {
  RD_ASSERT_EQ(rendu::overload<void(int)>(&functions::foo), static_cast<void (*)(int)>(&functions::foo));
  RD_ASSERT_EQ(rendu::overload<void()>(&functions::foo), static_cast<void (*)()>(&functions::foo));

  RD_ASSERT_EQ(rendu::overload<void(int)>(&functions::bar), static_cast<void (functions::*)(int)>(&functions::bar));
  RD_ASSERT_EQ(rendu::overload<void()>(&functions::bar), static_cast<void (functions::*)()>(&functions::bar));

  functions instance;

  RD_ASSERT_NO_FATAL_FAILURE(rendu::overload<void(int)>(&functions::foo)(0));
  RD_ASSERT_NO_FATAL_FAILURE(rendu::overload<void()>(&functions::foo)());

  RD_ASSERT_NO_FATAL_FAILURE((instance.*rendu::overload<void(int)>(&functions::bar))(0));
  RD_ASSERT_NO_FATAL_FAILURE((instance.*rendu::overload<void()>(&functions::bar))());
}

RD_TEST(Overloaded, Functionalities) {
  int iv = 0;
  char cv = '\0';

  rendu::overloaded func{
      [&iv](int value) { iv = value; },
      [&cv](char value) { cv = value; }};

  func(42);
  func('c');

  RD_ASSERT_EQ(iv, 42);
  RD_ASSERT_EQ(cv, 'c');
}

RD_TEST(YCombinator, Functionalities) {
  rendu::y_combinator gauss([](const auto &self, auto value) -> unsigned int {
    return value ? (value + self(value - 1u)) : 0;
  });

  RD_ASSERT_EQ(gauss(3u), 3u * 4u / 2u);
  RD_ASSERT_EQ(std::as_const(gauss)(7u), 7u * 8u / 2u);
}
