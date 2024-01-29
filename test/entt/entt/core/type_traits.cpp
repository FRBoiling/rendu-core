#include <functional>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
#include <gtest/gtest.h>
#include <entt/entt.hpp>

struct not_comparable {
  bool operator==(const not_comparable &) const = delete;
};

struct nlohmann_json_like final {
  using value_type = nlohmann_json_like;

  bool operator==(const nlohmann_json_like &) const {
    return true;
  }
};

struct clazz {
  char foo(int) {
    return {};
  }

  int bar(double, float) const {
    return {};
  }

  bool quux;
};

int free_function(int, const double &) {
  return 42;
}

template<typename, typename Type = void>
struct multi_argument_operation {
  using type = Type;
};

TEST(SizeOf, Functionalities) {
  EXPECT_TRUE(entt::size_of_v<void> == 0u);
  EXPECT_TRUE(entt::size_of_v<char> == sizeof(char));
  EXPECT_TRUE(entt::size_of_v<int[]> == 0u);
  EXPECT_TRUE(entt::size_of_v<int[3]> == sizeof(int[3]));
}

TEST(UnpackAsType, Functionalities) {
  auto test = [](auto &&...args) {
    return [](entt::unpack_as_type<int, decltype(args)>... value) {
      return (value + ... + 0);
    };
  };

  ASSERT_EQ(test('c', 42., true)(1, 2, 3), 6);
}

TEST(UnpackAsValue, Functionalities) {
  auto test = [](auto &&...args) {
    return (entt::unpack_as_value<2, decltype(args)> + ... + 0);
  };

  ASSERT_EQ(test('c', 42., true), 6);
}

TEST(IntegralConstant, Functionalities) {
  entt::integral_constant<3> constant{};
  EXPECT_TRUE(constant.value == 3);
}


TEST(TypeList, Functionalities) {
  using type = entt::type_list<int, char>;
  using other = entt::type_list<double>;

  EXPECT_TRUE(type::size == 2u);
  EXPECT_TRUE(other::size == 1u);
}

TEST(ValueList, Functionalities) {
  using value = entt::value_list<0, 2>;
  using other = entt::value_list<1>;

  EXPECT_TRUE(value::size == 2u);
  EXPECT_TRUE(other::size == 1u);
}

TEST(IsComplete, Functionalities) {
  EXPECT_TRUE(!entt::is_complete_v<void>);
  EXPECT_TRUE(entt::is_complete_v<int>);
}

TEST(IsIterator, Functionalities) {
  EXPECT_TRUE(!entt::is_iterator_v<void>);
  EXPECT_TRUE(!entt::is_iterator_v<int>);

  EXPECT_TRUE(!entt::is_iterator_v<void *>);
  EXPECT_TRUE(entt::is_iterator_v<int *>);

  EXPECT_TRUE(entt::is_iterator_v<std::vector<int>::iterator>);
  EXPECT_TRUE(entt::is_iterator_v<std::vector<int>::const_iterator>);
  EXPECT_TRUE(entt::is_iterator_v<std::vector<int>::reverse_iterator>);
}

TEST(IsEBCOEligible, Functionalities) {
  EXPECT_TRUE(entt::is_ebco_eligible_v<not_comparable>);
  EXPECT_TRUE(!entt::is_ebco_eligible_v<nlohmann_json_like>);
  EXPECT_TRUE(!entt::is_ebco_eligible_v<double>);
  EXPECT_TRUE(!entt::is_ebco_eligible_v<void>);
}

TEST(IsTransparent, Functionalities) {
  EXPECT_TRUE(!entt::is_transparent_v<std::less<int>>);
  EXPECT_TRUE(entt::is_transparent_v<std::less<void>>);
  EXPECT_TRUE(!entt::is_transparent_v<std::logical_not<double>>);
  EXPECT_TRUE(entt::is_transparent_v<std::logical_not<void>>);
}

TEST(IsEqualityComparable, Functionalities) {
  EXPECT_TRUE(entt::is_EqualityComparable_v<int>);
  EXPECT_TRUE(entt::is_EqualityComparable_v<const int>);
  EXPECT_TRUE(entt::is_EqualityComparable_v<std::vector<int>>);
  EXPECT_TRUE(entt::is_EqualityComparable_v<std::vector<std::vector<int>>>);
  EXPECT_TRUE(entt::is_EqualityComparable_v<std::vector<not_comparable>::iterator>);
  EXPECT_TRUE(entt::is_EqualityComparable_v<nlohmann_json_like>);

  EXPECT_TRUE(!entt::is_EqualityComparable_v<int[3u]>);
  EXPECT_TRUE(!entt::is_EqualityComparable_v<not_comparable>);
  EXPECT_TRUE(!entt::is_EqualityComparable_v<const not_comparable>);
  EXPECT_TRUE(!entt::is_EqualityComparable_v<std::vector<not_comparable>>);
  EXPECT_TRUE(!entt::is_EqualityComparable_v<std::vector<std::vector<not_comparable>>>);
  EXPECT_TRUE(!entt::is_EqualityComparable_v<void>);
}

TEST(Tag, Functionalities) {
  using namespace entt::literals;
  EXPECT_TRUE(entt::tag<"foobar"_hs>::value == entt::hashed_string::value("foobar"));
}
