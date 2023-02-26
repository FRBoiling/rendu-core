/*
* Created by boil on 2023/2/19.
*/

#include <test/rdtest.h>
#include <core/base/iterator.h>

struct clazz {
  int value{0};
};

RD_TEST(InputIteratorPointer, Functionalities) {
  clazz instance{};
  rendu::input_iterator_pointer ptr{std::move(instance)};
  ptr->value = 42;

  RD_ASSERT_EQ(instance.value, 0);
  RD_ASSERT_EQ(ptr->value, 42);
  RD_ASSERT_EQ(ptr->value, (*ptr).value);
  RD_ASSERT_EQ(ptr.operator->(), &ptr.operator*());
}

RD_TEST(IotaIterator, Functionalities) {
  rendu::iota_iterator<std::size_t> first{};
  const rendu::iota_iterator<std::size_t> last{2u};

  RD_ASSERT_NE(first, last);
  RD_ASSERT_FALSE(first == last);
  RD_ASSERT_TRUE(first != last);

  RD_ASSERT_EQ(*first++, 0u);
  RD_ASSERT_EQ(*first, 1u);
  RD_ASSERT_EQ(*++first, *last);
  RD_ASSERT_EQ(*first, 2u);
}

RD_TEST(IterableAdaptor, Functionalities) {
  std::vector<int> vec{1, 2};
  rendu::iterable_adaptor iterable{vec.begin(), vec.end()};
  decltype(iterable) other{};

  RD_ASSERT_NO_FATAL_FAILURE(other = iterable);
  RD_ASSERT_NO_FATAL_FAILURE(std::swap(other, iterable));

  RD_ASSERT_EQ(iterable.begin(), vec.begin());
  RD_ASSERT_EQ(iterable.end(), vec.end());

  RD_ASSERT_EQ(*iterable.cbegin(), 1);
  RD_ASSERT_EQ(*++iterable.cbegin(), 2);
  RD_ASSERT_EQ(++iterable.cbegin(), --iterable.end());

  for (auto value : rendu::iterable_adaptor<const int *, const void *>{vec.data(), vec.data() + 1u}) {
    RD_ASSERT_EQ(value, 1);
  }
}

