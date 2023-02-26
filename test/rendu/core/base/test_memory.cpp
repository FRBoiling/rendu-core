/*
* Created by boil on 2023/2/19.
*/
#include <cmath>
#include <test/rdtest.h>
#include <test/throwing_allocator.h>
#include <test/basic_test_allocator.h>
#include <test/throwing_type.h>
#include <core/base/memory.h>


RD_TEST(ToAddress, Functionalities) {
  std::shared_ptr<int> shared = std::make_shared<int>();
  auto *plain = std::addressof(*shared);

  RD_ASSERT_EQ(rendu::to_address(shared), plain);
  RD_ASSERT_EQ(rendu::to_address(plain), plain);
}

RD_TEST(PoccaPocmaAndPocs, Functionalities) {
  test::basic_test_allocator<int> lhs, rhs;
  test::basic_test_allocator<int, std::false_type> no_pocs;

  // code coverage purposes
  RD_ASSERT_FALSE(lhs == rhs);
  RD_ASSERT_NO_FATAL_FAILURE(rendu::propagate_on_container_swap(no_pocs, no_pocs));

  // honestly, I don't even know how one is supposed to test such a thing :)
  rendu::propagate_on_container_copy_assignment(lhs, rhs);
  rendu::propagate_on_container_move_assignment(lhs, rhs);
  rendu::propagate_on_container_swap(lhs, rhs);
}

RD_DEBUG_TEST(PoccaPocmaAndPocsDeathTest, Functionalities) {
  test::basic_test_allocator<int, std::false_type> lhs, rhs;

  RD_ASSERT_DEATH(rendu::propagate_on_container_swap(lhs, rhs), "");
}

RD_TEST(IsPowerOfTwo, Functionalities) {
  // constexpr-ness guaranteed
  constexpr auto zero_is_power_of_two = rendu::is_power_of_two(0u);

  RD_ASSERT_FALSE(zero_is_power_of_two);
  RD_ASSERT_TRUE(rendu::is_power_of_two(1u));
  RD_ASSERT_TRUE(rendu::is_power_of_two(2u));
  RD_ASSERT_TRUE(rendu::is_power_of_two(4u));
  RD_ASSERT_FALSE(rendu::is_power_of_two(7u));
  RD_ASSERT_TRUE(rendu::is_power_of_two(128u));
  RD_ASSERT_FALSE(rendu::is_power_of_two(200u));
}

RD_TEST(NextPowerOfTwo, Functionalities) {
  // constexpr-ness guaranteed
  constexpr auto next_power_of_two_of_zero = rendu::next_power_of_two(0u);

  RD_ASSERT_EQ(next_power_of_two_of_zero, 1u);
  RD_ASSERT_EQ(rendu::next_power_of_two(1u), 1u);
  RD_ASSERT_EQ(rendu::next_power_of_two(2u), 2u);
  RD_ASSERT_EQ(rendu::next_power_of_two(3u), 4u);
  RD_ASSERT_EQ(rendu::next_power_of_two(17u), 32u);
  RD_ASSERT_EQ(rendu::next_power_of_two(32u), 32u);
  RD_ASSERT_EQ(rendu::next_power_of_two(33u), 64u);
  RD_ASSERT_EQ(rendu::next_power_of_two(static_cast<std::size_t>(std::pow(2, 16))),
               static_cast<std::size_t>(std::pow(2, 16)));
  RD_ASSERT_EQ(rendu::next_power_of_two(static_cast<std::size_t>(std::pow(2, 16) + 1u)),
               static_cast<std::size_t>(std::pow(2, 17)));
}

RD_DEBUG_TEST(NextPowerOfTwoDeathTest, Functionalities) {
  RD_ASSERT_DEATH(static_cast<void>(rendu::next_power_of_two(
      (std::size_t{1u} << (std::numeric_limits<std::size_t>::digits - 1)) + 1)), "");
}

RD_TEST(FastMod, Functionalities) {
  // constexpr-ness guaranteed
  constexpr auto fast_mod_of_zero = rendu::fast_mod(0u, 8u);

  RD_ASSERT_EQ(fast_mod_of_zero, 0u);
  RD_ASSERT_EQ(rendu::fast_mod(7u, 8u), 7u);
  RD_ASSERT_EQ(rendu::fast_mod(8u, 8u), 0u);
}

RD_TEST(AllocateUnique, Functionalities) {
  test::throwing_allocator<test::throwing_type> allocator{};
  test::throwing_allocator<test::throwing_type>::trigger_on_allocate = true;
  test::throwing_type::trigger_on_value = 0;

  RD_ASSERT_THROW((rendu::allocate_unique<test::throwing_type>(allocator, 0)),
                  test::throwing_allocator<test::throwing_type>::exception_type);
  RD_ASSERT_THROW((rendu::allocate_unique<test::throwing_type>(allocator, test::throwing_type{0})),
                  test::throwing_type::exception_type);

  std::unique_ptr<test::throwing_type, rendu::allocation_deleter<test::throwing_allocator<test::throwing_type>>>
      ptr = rendu::allocate_unique<test::throwing_type>(allocator, 42);

  RD_ASSERT_TRUE(ptr);
  RD_ASSERT_EQ(*ptr, 42);

  ptr.reset();

  RD_ASSERT_FALSE(ptr);
}

#if defined(RD_HAS_TRACKED_MEMORY_RESOURCE)

RD_TEST(AllocateUnique, NoUsesAllocatorConstruction) {
    test::tracked_memory_resource memory_resource{};
    std::pmr::polymorphic_allocator<int> allocator{&memory_resource};

    using type = std::unique_ptr<int, rendu::allocation_deleter<std::pmr::polymorphic_allocator<int>>>;
    type ptr = rendu::allocate_unique<int>(allocator, 0);

    RD_ASSERT_EQ(memory_resource.do_allocate_counter(), 1u);
    RD_ASSERT_EQ(memory_resource.do_deallocate_counter(), 0u);
}

RD_TEST(AllocateUnique, UsesAllocatorConstruction) {
    using string_type = typename test::tracked_memory_resource::string_type;

    test::tracked_memory_resource memory_resource{};
    std::pmr::polymorphic_allocator<string_type> allocator{&memory_resource};

    using type = std::unique_ptr<string_type, rendu::allocation_deleter<std::pmr::polymorphic_allocator<string_type>>>;
    type ptr = rendu::allocate_unique<string_type>(allocator, test::tracked_memory_resource::default_value);

    RD_ASSERT_GT(memory_resource.do_allocate_counter(), 1u);
    RD_ASSERT_EQ(memory_resource.do_deallocate_counter(), 0u);
}

#endif

RD_TEST(UsesAllocatorConstructionArgs, NoUsesAllocatorConstruction) {
  const auto value = 42;
  const auto args = rendu::uses_allocator_construction_args<int>(std::allocator<int>{}, value);

  static_assert(std::tuple_size_v<decltype(args)> == 1u);
  static_assert(std::is_same_v<decltype(args), const std::tuple<const int &>>);

  RD_ASSERT_EQ(std::get<0>(args), value);
}

RD_TEST(UsesAllocatorConstructionArgs, LeadingAllocatorConvention) {
  const auto value = 42;
  const auto args = rendu::uses_allocator_construction_args<std::tuple<int, char>>(std::allocator<int>{}, value, 'c');

  static_assert(std::tuple_size_v<decltype(args)> == 4u);
  static_assert(std::is_same_v<decltype(args),
                               const std::tuple<std::allocator_arg_t,
                                                const std::allocator<int> &,
                                                const int &,
                                                char &&>>);

  RD_ASSERT_EQ(std::get<2>(args), value);
}

RD_TEST(UsesAllocatorConstructionArgs, TrailingAllocatorConvention) {
  const auto size = 42u;
  const auto args = rendu::uses_allocator_construction_args<std::vector<int>>(std::allocator<int>{}, size);

  static_assert(std::tuple_size_v<decltype(args)> == 2u);
  static_assert(std::is_same_v<decltype(args), const std::tuple<const unsigned int &, const std::allocator<int> &>>);

  RD_ASSERT_EQ(std::get<0>(args), size);
}

RD_TEST(UsesAllocatorConstructionArgs, PairPiecewiseConstruct) {
  const auto size = 42u;
  const auto tup = std::make_tuple(size);
  const auto args = rendu::uses_allocator_construction_args<std::pair<int, std::vector<int>>>(std::allocator<int>{},
                                                                                              std::piecewise_construct,
                                                                                              std::make_tuple(3),
                                                                                              tup);

  static_assert(std::tuple_size_v<decltype(args)> == 3u);
  static_assert(std::is_same_v<decltype(args),
                               const std::tuple<std::piecewise_construct_t,
                                                std::tuple<int &&>,
                                                std::tuple<const unsigned int &, const std::allocator<int> &>>>);

  RD_ASSERT_EQ(std::get<0>(std::get<2>(args)), size);
}

RD_TEST(UsesAllocatorConstructionArgs, PairNoArgs) {
  [[maybe_unused]] const auto
      args = rendu::uses_allocator_construction_args<std::pair<int, std::vector<int>>>(std::allocator<int>{});

  static_assert(std::tuple_size_v<decltype(args)> == 3u);
  static_assert(std::is_same_v<decltype(args),
                               const std::tuple<std::piecewise_construct_t,
                                                std::tuple<>,
                                                std::tuple<const std::allocator<int> &>>>);
}

RD_TEST(UsesAllocatorConstructionArgs, PairValues) {
  const auto size = 42u;
  const auto
      args = rendu::uses_allocator_construction_args<std::pair<int, std::vector<int>>>(std::allocator<int>{}, 3, size);

  static_assert(std::tuple_size_v<decltype(args)> == 3u);
  static_assert(std::is_same_v<decltype(args),
                               const std::tuple<std::piecewise_construct_t,
                                                std::tuple<int &&>,
                                                std::tuple<const unsigned int &, const std::allocator<int> &>>>);

  RD_ASSERT_EQ(std::get<0>(std::get<2>(args)), size);
}

RD_TEST(UsesAllocatorConstructionArgs, PairConstLValueReference) {
  const auto value = std::make_pair(3, 42u);
  const auto
      args = rendu::uses_allocator_construction_args<std::pair<int, std::vector<int>>>(std::allocator<int>{}, value);

  static_assert(std::tuple_size_v<decltype(args)> == 3u);
  static_assert(std::is_same_v<decltype(args),
                               const std::tuple<std::piecewise_construct_t,
                                                std::tuple<const int &>,
                                                std::tuple<const unsigned int &, const std::allocator<int> &>>>);

  RD_ASSERT_EQ(std::get<0>(std::get<1>(args)), 3);
  RD_ASSERT_EQ(std::get<0>(std::get<2>(args)), 42u);
}

RD_TEST(UsesAllocatorConstructionArgs, PairRValueReference) {
  [[maybe_unused]] const auto args =
      rendu::uses_allocator_construction_args<std::pair<int, std::vector<int>>>(std::allocator<int>{},
                                                                                std::make_pair(3, 42u));

  static_assert(std::tuple_size_v<decltype(args)> == 3u);
  static_assert(std::is_same_v<decltype(args),
                               const std::tuple<std::piecewise_construct_t,
                                                std::tuple<int &&>,
                                                std::tuple<unsigned int &&, const std::allocator<int> &>>>);
}

RD_TEST(MakeObjUsingAllocator, Functionalities) {
  const auto size = 42u;
  test::throwing_allocator<int>::trigger_on_allocate = true;

  RD_ASSERT_THROW((rendu::make_obj_using_allocator<std::vector<int,
                                                               test::throwing_allocator<int>>>(test::throwing_allocator<
      int>{}, size)), test::throwing_allocator<int>::exception_type);

  const auto vec = rendu::make_obj_using_allocator<std::vector<int>>(std::allocator<int>{}, size);

  RD_ASSERT_FALSE(vec.empty());
  RD_ASSERT_EQ(vec.size(), size);
}

RD_TEST(UninitializedConstructUsingAllocator, NoUsesAllocatorConstruction) {
  alignas(int) std::byte storage[sizeof(int)];
  std::allocator<int> allocator{};

  int *value = rendu::uninitialized_construct_using_allocator(reinterpret_cast<int *>(&storage), allocator, 42);

  RD_ASSERT_EQ(*value, 42);
}

#if defined(RD_HAS_TRACKED_MEMORY_RESOURCE)

RD_TEST(UninitializedConstructUsingAllocator, UsesAllocatorConstruction) {
    using string_type = typename test::tracked_memory_resource::string_type;

    test::tracked_memory_resource memory_resource{};
    std::pmr::polymorphic_allocator<string_type> allocator{&memory_resource};
    alignas(string_type) std::byte storage[sizeof(string_type)];

    string_type *value = rendu::uninitialized_construct_using_allocator(reinterpret_cast<string_type *>(&storage), allocator, test::tracked_memory_resource::default_value);

    RD_ASSERT_GT(memory_resource.do_allocate_counter(), 0u);
    RD_ASSERT_EQ(memory_resource.do_deallocate_counter(), 0u);
    RD_ASSERT_EQ(*value, test::tracked_memory_resource::default_value);

    value->~string_type();
}

#endif
