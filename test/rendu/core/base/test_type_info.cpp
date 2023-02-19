/*
* Created by boil on 2023/2/19.
*/

#include <test/rdtest.h>
#include <core/base/type_info.h>
#include <core/base/type_traits.h>

template<>
struct rendu::type_name<float> final {
  [[nodiscard]] static constexpr std::string_view value() noexcept {
    return std::string_view{""};
  }
};

RD_TEST(TypeIndex, Functionalities) {
  RD_ASSERT_EQ(rendu::type_index<int>::value(), rendu::type_index<int>::value());
  RD_ASSERT_NE(rendu::type_index<int>::value(), rendu::type_index<char>::value());
  RD_ASSERT_NE(rendu::type_index<int>::value(), rendu::type_index<int &&>::value());
  RD_ASSERT_NE(rendu::type_index<int &>::value(), rendu::type_index<const int &>::value());
  RD_ASSERT_EQ(static_cast<rendu::id_type>(rendu::type_index<int>{}), rendu::type_index<int>::value());
}

RD_TEST(TypeHash, Functionalities) {
  RD_ASSERT_NE(rendu::type_hash<int>::value(), rendu::type_hash<const int>::value());
  RD_ASSERT_NE(rendu::type_hash<int>::value(), rendu::type_hash<char>::value());
  RD_ASSERT_EQ(rendu::type_hash<int>::value(), rendu::type_hash<int>::value());
  RD_ASSERT_EQ(static_cast<rendu::id_type>(rendu::type_hash<int>{}), rendu::type_hash<int>::value());
}

RD_TEST(TypeName, Functionalities) {
  RD_ASSERT_EQ(rendu::type_name<int>::value(), std::string_view{"int"});
  RD_ASSERT_EQ(rendu::type_name<float>{}.value(), std::string_view{""});

  RD_ASSERT_TRUE((rendu::type_name<rendu::integral_constant<3>>::value() == std::string_view{"std::integral_constant<int, 3>"})
                  || (rendu::type_name<rendu::integral_constant<3>>::value() == std::string_view{"std::__1::integral_constant<int, 3>"})
                  || (rendu::type_name<rendu::integral_constant<3>>::value() == std::string_view{"struct std::integral_constant<int,3>"}));

  RD_ASSERT_TRUE(((rendu::type_name<rendu::type_list<rendu::type_list<int, char>, double>>::value()) == std::string_view{"rendu::type_list<rendu::type_list<int, char>, double>"})
                  || ((rendu::type_name<rendu::type_list<rendu::type_list<int, char>, double>>::value()) == std::string_view{"struct rendu::type_list<struct rendu::type_list<int,char>,double>"}));

  RD_ASSERT_EQ(static_cast<std::string_view>(rendu::type_name<int>{}), rendu::type_name<int>::value());
}

RD_TEST(TypeInfo, Functionalities) {
  static_assert(std::is_copy_constructible_v<rendu::type_info>);
  static_assert(std::is_move_constructible_v<rendu::type_info>);
  static_assert(std::is_copy_assignable_v<rendu::type_info>);
  static_assert(std::is_move_assignable_v<rendu::type_info>);

  rendu::type_info info{std::in_place_type<int>};
  rendu::type_info other{std::in_place_type<void>};

  RD_ASSERT_EQ(info, rendu::type_info{std::in_place_type<int &>});
  RD_ASSERT_EQ(info, rendu::type_info{std::in_place_type<int &&>});
  RD_ASSERT_EQ(info, rendu::type_info{std::in_place_type<const int &>});

  RD_ASSERT_NE(info, other);
  RD_ASSERT_TRUE(info == info);
  RD_ASSERT_FALSE(info != info);

  RD_ASSERT_EQ(info.index(), rendu::type_index<int>::value());
  RD_ASSERT_EQ(info.hash(), rendu::type_hash<int>::value());
  RD_ASSERT_EQ(info.name(), rendu::type_name<int>::value());

  other = info;

  RD_ASSERT_EQ(other.index(), rendu::type_index<int>::value());
  RD_ASSERT_EQ(other.hash(), rendu::type_hash<int>::value());
  RD_ASSERT_EQ(other.name(), rendu::type_name<int>::value());

  RD_ASSERT_EQ(other.index(), info.index());
  RD_ASSERT_EQ(other.hash(), info.hash());
  RD_ASSERT_EQ(other.name(), info.name());

  other = std::move(info);

  RD_ASSERT_EQ(other.index(), rendu::type_index<int>::value());
  RD_ASSERT_EQ(other.hash(), rendu::type_hash<int>::value());
  RD_ASSERT_EQ(other.name(), rendu::type_name<int>::value());

  RD_ASSERT_EQ(other.index(), info.index());
  RD_ASSERT_EQ(other.hash(), info.hash());
  RD_ASSERT_EQ(other.name(), info.name());
}

RD_TEST(TypeInfo, Order) {
  rendu::type_info rhs = rendu::type_id<int>();
  rendu::type_info lhs = rendu::type_id<char>();

  // let's adjust the two objects since values are generated at runtime
  rhs < lhs ? void() : std::swap(lhs, rhs);

  RD_ASSERT_FALSE(lhs < lhs);
  RD_ASSERT_FALSE(rhs < rhs);

  RD_ASSERT_LT(rhs, lhs);
  RD_ASSERT_LE(rhs, lhs);

  RD_ASSERT_GT(lhs, rhs);
  RD_ASSERT_GE(lhs, rhs);
}

RD_TEST(TypeId, Functionalities) {
  const int value = 42;

  RD_ASSERT_EQ(rendu::type_id(value), rendu::type_id<int>());
  RD_ASSERT_EQ(rendu::type_id(42), rendu::type_id<int>());

  RD_ASSERT_EQ(rendu::type_id<int>(), rendu::type_id<int>());
  RD_ASSERT_EQ(rendu::type_id<int &>(), rendu::type_id<int &&>());
  RD_ASSERT_EQ(rendu::type_id<int &>(), rendu::type_id<int>());
  RD_ASSERT_NE(rendu::type_id<int>(), rendu::type_id<char>());

  RD_ASSERT_EQ(&rendu::type_id<int>(), &rendu::type_id<int>());
  RD_ASSERT_NE(&rendu::type_id<int>(), &rendu::type_id<void>());
}
