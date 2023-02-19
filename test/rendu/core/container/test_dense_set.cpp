/*
* Created by boil on 2023/2/19.
*/
#include <test/rdtest.h>
#include <container/dense_set.h>
#include <ecs/entity.h>
#include <base/memory.h>
#include "test/throwing_allocator.h"

namespace {

struct transparent_equal_to {
  using is_transparent = void;

  template<typename Type, typename Other>
  constexpr std::enable_if_t<std::is_convertible_v<Other, Type>, bool>
  operator()(const Type &lhs, const Other &rhs) const {
    return lhs == static_cast<Type>(rhs);
  }
};

RD_TEST(DenseSet, Functionalities) {
  rendu::dense_set<int, std::identity, transparent_equal_to> set;
  const auto &cset = set;

  RD_ASSERT_NO_FATAL_FAILURE([[maybe_unused]] auto alloc = set.get_allocator());

  RD_ASSERT_TRUE(set.empty());
  RD_ASSERT_EQ(set.size(), 0u);
  RD_ASSERT_EQ(set.load_factor(), 0.f);
  RD_ASSERT_EQ(set.max_load_factor(), .875f);
  RD_ASSERT_EQ(set.max_size(), (std::vector<std::pair<std::size_t, int>>{}.max_size()));

  set.max_load_factor(.9f);

  RD_ASSERT_EQ(set.max_load_factor(), .9f);

  RD_ASSERT_EQ(set.begin(), set.end());
  RD_ASSERT_EQ(cset.begin(), cset.end());
  RD_ASSERT_EQ(set.cbegin(), set.cend());

  RD_ASSERT_NE(set.max_bucket_count(), 0u);
  RD_ASSERT_EQ(set.bucket_count(), 8u);
  RD_ASSERT_EQ(set.bucket_size(3u), 0u);

  RD_ASSERT_EQ(set.bucket(0), 0u);
  RD_ASSERT_EQ(set.bucket(3), 3u);
  RD_ASSERT_EQ(set.bucket(8), 0u);
  RD_ASSERT_EQ(set.bucket(10), 2u);

  RD_ASSERT_EQ(set.begin(1u), set.end(1u));
  RD_ASSERT_EQ(cset.begin(1u), cset.end(1u));
  RD_ASSERT_EQ(set.cbegin(1u), set.cend(1u));

  RD_ASSERT_FALSE(set.contains(42));
  RD_ASSERT_FALSE(set.contains(4.2));

  RD_ASSERT_EQ(set.find(42), set.end());
  RD_ASSERT_EQ(set.find(4.2), set.end());
  RD_ASSERT_EQ(cset.find(42), set.cend());
  RD_ASSERT_EQ(cset.find(4.2), set.cend());

  RD_ASSERT_EQ(set.hash_function()(42), 42);
  RD_ASSERT_TRUE(set.key_eq()(42, 42));

  set.emplace(0);

  RD_ASSERT_EQ(set.count(0), 1u);
  RD_ASSERT_EQ(set.count(4.2), 0u);
  RD_ASSERT_EQ(cset.count(0.0), 1u);
  RD_ASSERT_EQ(cset.count(42), 0u);

  RD_ASSERT_FALSE(set.empty());
  RD_ASSERT_EQ(set.size(), 1u);

  RD_ASSERT_NE(set.begin(), set.end());
  RD_ASSERT_NE(cset.begin(), cset.end());
  RD_ASSERT_NE(set.cbegin(), set.cend());

  RD_ASSERT_TRUE(set.contains(0));
  RD_ASSERT_EQ(set.bucket(0), 0u);

  set.
      clear();

  RD_ASSERT_TRUE(set.empty());
  RD_ASSERT_EQ(set.size(), 0u);

  RD_ASSERT_EQ(set.begin(), set.end());
  RD_ASSERT_EQ(cset.begin(), cset.end());
  RD_ASSERT_EQ(set.cbegin(), set.cend());

  RD_ASSERT_FALSE(set.contains(0));
}

RD_TEST(DenseSet, Constructors) {
  constexpr std::size_t minimum_bucket_count = 8u;
  rendu::dense_set<int> set;

  RD_ASSERT_EQ(set.bucket_count(), minimum_bucket_count);

  set = rendu::dense_set<int>{std::allocator<int>{}};
  set = rendu::dense_set<int>{2u * minimum_bucket_count, std::allocator<float>{}};
  set = rendu::dense_set<int>{4u * minimum_bucket_count, std::hash<int>(), std::allocator<double>{}};

  set.emplace(3);

  rendu::dense_set<int> temp{set, set.get_allocator()};
  rendu::dense_set<int> other{std::move(temp), set.get_allocator()};

  RD_ASSERT_EQ(set.size(), 1u);
  RD_ASSERT_EQ(other.size(), 1u);
  RD_ASSERT_EQ(set.bucket_count(), 4u * minimum_bucket_count);
  RD_ASSERT_EQ(other.bucket_count(), 4u * minimum_bucket_count);
}

RD_TEST(DenseSet, Copy) {
  rendu::dense_set<std::size_t, std::identity> set;
  set.
      max_load_factor(set
                          .
                              max_load_factor()
                          - .05f);
  set.emplace(3u);

  rendu::dense_set<std::size_t, std::identity> other{set};

  RD_ASSERT_TRUE(set.contains(3u));
  RD_ASSERT_TRUE(other.contains(3u));
  RD_ASSERT_EQ(set.max_load_factor(), other.max_load_factor());

  set.emplace(1u);
  set.emplace(11u);
  other.emplace(0u);
  other = set;

  RD_ASSERT_TRUE(other.contains(3u));
  RD_ASSERT_TRUE(other.contains(1u));
  RD_ASSERT_TRUE(other.contains(11u));
  RD_ASSERT_FALSE(other.contains(0u));

  RD_ASSERT_EQ(other.bucket(3u), set.bucket(11u));
  RD_ASSERT_EQ(other.bucket(3u), other.bucket(11u));
  RD_ASSERT_EQ(*other.begin(3u), *set.begin(3u));
  RD_ASSERT_EQ(*other.begin(3u), 11u);
  RD_ASSERT_EQ((*++other.begin(3u)), 3u);
}

RD_TEST(DenseSet, Move) {
  rendu::dense_set<std::size_t, std::identity> set;
  set.
      max_load_factor(set
                          .
                              max_load_factor()
                          - .05f);
  set.emplace(3u);

  rendu::dense_set<std::size_t, std::identity> other{std::move(set)};

  RD_ASSERT_EQ(set.size(), 0u);
  RD_ASSERT_TRUE(other.contains(3u));
  RD_ASSERT_EQ(set.max_load_factor(), other.max_load_factor());

  set = other;
  set.emplace(1u);
  set.emplace(11u);
  other.emplace(0u);
  other = std::move(set);

  RD_ASSERT_EQ(set.size(), 0u);
  RD_ASSERT_TRUE(other.contains(3u));
  RD_ASSERT_TRUE(other.contains(1u));
  RD_ASSERT_TRUE(other.contains(11u));
  RD_ASSERT_FALSE(other.contains(0u));

  RD_ASSERT_EQ(other.bucket(3u), other.bucket(11u));
  RD_ASSERT_EQ(*other.begin(3u), 11u);
  RD_ASSERT_EQ(*++other.begin(3u), 3u);
}

RD_TEST(DenseSet, Iterator) {
  using iterator = typename rendu::dense_set<int>::iterator;

  static_assert(std::is_same_v<iterator::value_type, int>);
  static_assert(std::is_same_v<iterator::pointer, const int *>);
  static_assert(std::is_same_v<iterator::reference, const int &>);

  rendu::dense_set<int> set;
  set.emplace(3);

  iterator end{set.begin()};
  iterator begin{};
  begin = set.end();
  std::swap(begin, end
  );

  RD_ASSERT_EQ(begin, set.begin());
  RD_ASSERT_EQ(end, set.end());
  RD_ASSERT_NE(begin, end);

  RD_ASSERT_EQ(begin++, set.begin());
  RD_ASSERT_EQ(begin--, set.end());

  RD_ASSERT_EQ(begin + 1, set.end());
  RD_ASSERT_EQ(end - 1, set.begin());

  RD_ASSERT_EQ(++begin, set.end());
  RD_ASSERT_EQ(--begin, set.begin());

  RD_ASSERT_EQ(begin += 1, set.end());
  RD_ASSERT_EQ(begin -= 1, set.begin());

  RD_ASSERT_EQ(begin + (end - begin), set.end());
  RD_ASSERT_EQ(begin - (begin - end), set.end());

  RD_ASSERT_EQ(end - (end - begin), set.begin());
  RD_ASSERT_EQ(end + (begin - end), set.begin());

  RD_ASSERT_EQ(begin[0u], *set.begin().operator->());
  RD_ASSERT_EQ(begin[0u], *set.begin());

  RD_ASSERT_LT(begin, end);
  RD_ASSERT_LE(begin, set.begin());

  RD_ASSERT_GT(end, begin);
  RD_ASSERT_GE(end, set.end());

  set.emplace(42);
  begin = set.begin();

  RD_ASSERT_EQ(begin[0u], 3);
  RD_ASSERT_EQ(begin[1u], 42);
}

RD_TEST(DenseSet, ConstIterator) {
  using iterator = typename rendu::dense_set<int>::const_iterator;

  static_assert(std::is_same_v<iterator::value_type, int>);
  static_assert(std::is_same_v<iterator::pointer, const int *>);
  static_assert(std::is_same_v<iterator::reference, const int &>);

  rendu::dense_set<int> set;
  set.emplace(3);

  iterator cend{set.cbegin()};
  iterator cbegin{};
  cbegin = set.cend();
  std::swap(cbegin, cend
  );

  RD_ASSERT_EQ(cbegin, set.cbegin());
  RD_ASSERT_EQ(cend, set.cend());
  RD_ASSERT_NE(cbegin, cend);

  RD_ASSERT_EQ(cbegin++, set.cbegin());
  RD_ASSERT_EQ(cbegin--, set.cend());

  RD_ASSERT_EQ(cbegin + 1, set.cend());
  RD_ASSERT_EQ(cend - 1, set.cbegin());

  RD_ASSERT_EQ(++cbegin, set.cend());
  RD_ASSERT_EQ(--cbegin, set.cbegin());

  RD_ASSERT_EQ(cbegin += 1, set.cend());
  RD_ASSERT_EQ(cbegin -= 1, set.cbegin());

  RD_ASSERT_EQ(cbegin + (cend - cbegin), set.cend());
  RD_ASSERT_EQ(cbegin - (cbegin - cend), set.cend());

  RD_ASSERT_EQ(cend - (cend - cbegin), set.cbegin());
  RD_ASSERT_EQ(cend + (cbegin - cend), set.cbegin());

  RD_ASSERT_EQ(cbegin[0u], *set.cbegin().operator->());
  RD_ASSERT_EQ(cbegin[0u], *set.cbegin());

  RD_ASSERT_LT(cbegin, cend);
  RD_ASSERT_LE(cbegin, set.cbegin());

  RD_ASSERT_GT(cend, cbegin);
  RD_ASSERT_GE(cend, set.cend());

  set.emplace(42);
  cbegin = set.cbegin();

  RD_ASSERT_EQ(cbegin[0u], 3);
  RD_ASSERT_EQ(cbegin[1u], 42);
}

RD_TEST(DenseSet, IteratorConversion) {
  rendu::dense_set<int> set;
  set.emplace(3);

  typename rendu::dense_set<int, int>::iterator it = set.begin();
  typename rendu::dense_set<int, int>::const_iterator cit = it;

  static_assert(std::is_same_v<decltype(*it), const int &>);
  static_assert(std::is_same_v<decltype(*cit), const int &>);

  RD_ASSERT_EQ(*it, 3);
  RD_ASSERT_EQ(*it.operator->(), 3);
  RD_ASSERT_EQ(it.operator->(), cit.operator->());
  RD_ASSERT_EQ(*it, *cit);

  RD_ASSERT_EQ(it - cit, 0);
  RD_ASSERT_EQ(cit - it, 0);
  RD_ASSERT_LE(it, cit);
  RD_ASSERT_LE(cit, it);
  RD_ASSERT_GE(it, cit);
  RD_ASSERT_GE(cit, it);
  RD_ASSERT_EQ(it, cit);
  RD_ASSERT_NE(++cit, it);
}

RD_TEST(DenseSet, Insert) {
  rendu::dense_set<int> set;
  typename rendu::dense_set<int>::iterator it;
  bool result;

  RD_ASSERT_TRUE(set.empty());
  RD_ASSERT_EQ(set.size(), 0u);
  RD_ASSERT_EQ(set.find(0), set.end());
  RD_ASSERT_FALSE(set.contains(0));

  int value{1};
  std::tie(it, result
  ) = set.
      insert(std::as_const(value)
  );

  RD_ASSERT_TRUE(result);
  RD_ASSERT_EQ(set.size(), 1u);
  RD_ASSERT_EQ(it, --set.end());
  RD_ASSERT_TRUE(set.contains(1));
  RD_ASSERT_NE(set.find(1), set.end());
  RD_ASSERT_EQ(*it, 1);

  std::tie(it, result
  ) = set.
      insert(value);

  RD_ASSERT_FALSE(result);
  RD_ASSERT_EQ(set.size(), 1u);
  RD_ASSERT_EQ(it, --set.end());
  RD_ASSERT_EQ(*it, 1);

  std::tie(it, result
  ) = set.insert(3);

  RD_ASSERT_TRUE(result);
  RD_ASSERT_EQ(set.size(), 2u);
  RD_ASSERT_EQ(it, --set.end());
  RD_ASSERT_TRUE(set.contains(3));
  RD_ASSERT_NE(set.find(3), set.end());
  RD_ASSERT_EQ(*it, 3);

  std::tie(it, result
  ) = set.insert(3);

  RD_ASSERT_FALSE(result);
  RD_ASSERT_EQ(set.size(), 2u);
  RD_ASSERT_EQ(it, --set.end());
  RD_ASSERT_EQ(*it, 3);

  int range[2u]{7, 9};
  set.
      insert(std::begin(range), std::end(range)
  );

  RD_ASSERT_EQ(set.size(), 4u);
  RD_ASSERT_TRUE(set.contains(7));
  RD_ASSERT_NE(set.find(9), set.end());
}

RD_TEST(DenseSet, InsertRehash) {
  constexpr std::size_t minimum_bucket_count = 8u;
  rendu::dense_set<std::size_t, std::identity> set;

  RD_ASSERT_EQ(set.size(), 0u);
  RD_ASSERT_EQ(set.bucket_count(), minimum_bucket_count);

  for (
      std::size_t next{};
      next < minimum_bucket_count;
      ++next) {
    RD_ASSERT_TRUE(set.insert(next).second);
  }

  RD_ASSERT_EQ(set.size(), minimum_bucket_count);
  RD_ASSERT_GT(set.bucket_count(), minimum_bucket_count);
  RD_ASSERT_TRUE(set.contains(minimum_bucket_count / 2u));
  RD_ASSERT_EQ(set.bucket(minimum_bucket_count / 2u), minimum_bucket_count / 2u);
  RD_ASSERT_FALSE(set.contains(minimum_bucket_count));

  RD_ASSERT_TRUE(set.insert(minimum_bucket_count).second);

  RD_ASSERT_EQ(set.size(), minimum_bucket_count + 1u);
  RD_ASSERT_EQ(set.bucket_count(), minimum_bucket_count * 2u);
  RD_ASSERT_TRUE(set.contains(minimum_bucket_count / 2u));
  RD_ASSERT_EQ(set.bucket(minimum_bucket_count / 2u), minimum_bucket_count / 2u);
  RD_ASSERT_TRUE(set.contains(minimum_bucket_count));

  for (
      std::size_t next{};
      next <=
          minimum_bucket_count;
      ++next) {
    RD_ASSERT_TRUE(set.contains(next));
    RD_ASSERT_EQ(set.bucket(next), next);
  }
}

RD_TEST(DenseSet, InsertSameBucket) {
  constexpr std::size_t minimum_bucket_count = 8u;
  rendu::dense_set<std::size_t, std::identity> set;

  for (
      std::size_t next{};
      next < minimum_bucket_count;
      ++next) {
    RD_ASSERT_EQ(set.cbegin(next), set.cend(next));
  }

  RD_ASSERT_TRUE(set.insert(1u).second);
  RD_ASSERT_TRUE(set.insert(9u).second);

  RD_ASSERT_EQ(set.size(), 2u);
  RD_ASSERT_TRUE(set.contains(1u));
  RD_ASSERT_NE(set.find(9u), set.end());
  RD_ASSERT_EQ(set.bucket(1u), 1u);
  RD_ASSERT_EQ(set.bucket(9u), 1u);
  RD_ASSERT_EQ(set.bucket_size(1u), 2u);
  RD_ASSERT_EQ(set.cbegin(6u), set.cend(6u));
}

RD_TEST(DenseSet, Emplace) {
  rendu::dense_set<int> set;
  typename rendu::dense_set<int>::iterator it;
  bool result;

  RD_ASSERT_TRUE(set.empty());
  RD_ASSERT_EQ(set.size(), 0u);
  RD_ASSERT_EQ(set.find(0), set.end());
  RD_ASSERT_FALSE(set.contains(0));

  std::tie(it, result
  ) = set.
      emplace();

  RD_ASSERT_TRUE(result);
  RD_ASSERT_EQ(set.size(), 1u);
  RD_ASSERT_EQ(it, --set.end());
  RD_ASSERT_TRUE(set.contains(0));
  RD_ASSERT_NE(set.find(0), set.end());
  RD_ASSERT_EQ(*it, 0);

  std::tie(it, result
  ) = set.
      emplace();

  RD_ASSERT_FALSE(result);
  RD_ASSERT_EQ(set.size(), 1u);
  RD_ASSERT_EQ(it, --set.end());
  RD_ASSERT_EQ(*it, 0);

  std::tie(it, result
  ) = set.emplace(1);

  RD_ASSERT_TRUE(result);
  RD_ASSERT_EQ(set.size(), 2u);
  RD_ASSERT_EQ(it, --set.end());
  RD_ASSERT_TRUE(set.contains(1));
  RD_ASSERT_NE(set.find(1), set.end());
  RD_ASSERT_EQ(*it, 1);

  std::tie(it, result
  ) = set.emplace(1);

  RD_ASSERT_FALSE(result);
  RD_ASSERT_EQ(set.size(), 2u);
  RD_ASSERT_EQ(it, --set.end());
  RD_ASSERT_EQ(*it, 1);
}

RD_TEST(DenseSet, EmplaceRehash) {
  constexpr std::size_t minimum_bucket_count = 8u;
  rendu::dense_set<std::size_t, std::identity> set;

  RD_ASSERT_EQ(set.size(), 0u);
  RD_ASSERT_EQ(set.bucket_count(), minimum_bucket_count);

  for (
      std::size_t next{};
      next < minimum_bucket_count;
      ++next) {
    RD_ASSERT_TRUE(set.emplace(next).second);
    RD_ASSERT_LE(set.load_factor(), set.max_load_factor());
  }

  RD_ASSERT_EQ(set.size(), minimum_bucket_count);
  RD_ASSERT_GT(set.bucket_count(), minimum_bucket_count);
  RD_ASSERT_TRUE(set.contains(minimum_bucket_count / 2u));
  RD_ASSERT_EQ(set.bucket(minimum_bucket_count / 2u), minimum_bucket_count / 2u);
  RD_ASSERT_FALSE(set.contains(minimum_bucket_count));

  RD_ASSERT_TRUE(set.emplace(minimum_bucket_count).second);

  RD_ASSERT_EQ(set.size(), minimum_bucket_count + 1u);
  RD_ASSERT_EQ(set.bucket_count(), minimum_bucket_count * 2u);
  RD_ASSERT_TRUE(set.contains(minimum_bucket_count / 2u));
  RD_ASSERT_EQ(set.bucket(minimum_bucket_count / 2u), minimum_bucket_count / 2u);
  RD_ASSERT_TRUE(set.contains(minimum_bucket_count));

  for (
      std::size_t next{};
      next <=
          minimum_bucket_count;
      ++next) {
    RD_ASSERT_TRUE(set.contains(next));
    RD_ASSERT_EQ(set.bucket(next), next);
  }
}

RD_TEST(DenseSet, EmplaceSameBucket) {
  constexpr std::size_t minimum_bucket_count = 8u;
  rendu::dense_set<std::size_t, std::identity> set;

  for (
      std::size_t next{};
      next < minimum_bucket_count;
      ++next) {
    RD_ASSERT_EQ(set.cbegin(next), set.cend(next));
  }

  RD_ASSERT_TRUE(set.emplace(1u).second);
  RD_ASSERT_TRUE(set.emplace(9u).second);

  RD_ASSERT_EQ(set.size(), 2u);
  RD_ASSERT_TRUE(set.contains(1u));
  RD_ASSERT_NE(set.find(9u), set.end());
  RD_ASSERT_EQ(set.bucket(1u), 1u);
  RD_ASSERT_EQ(set.bucket(9u), 1u);
  RD_ASSERT_EQ(set.bucket_size(1u), 2u);
  RD_ASSERT_EQ(set.cbegin(6u), set.cend(6u));
}

RD_TEST(DenseSet, Erase) {
  constexpr std::size_t minimum_bucket_count = 8u;
  rendu::dense_set<std::size_t, std::identity> set;

  for (
      std::size_t next{}, last = minimum_bucket_count + 1u;
      next < last;
      ++next) {
    set.
        emplace(next);
  }

  RD_ASSERT_EQ(set.bucket_count(), 2 * minimum_bucket_count);
  RD_ASSERT_EQ(set.size(), minimum_bucket_count + 1u);

  for (
      std::size_t next{}, last = minimum_bucket_count + 1u;
      next < last;
      ++next) {
    RD_ASSERT_TRUE(set.contains(next));
  }

  auto it = set.erase(++set.begin());
  it = set.erase(it, it + 1);

  RD_ASSERT_EQ(*--set.end(), 6u);
  RD_ASSERT_EQ(set.erase(6u), 1u);
  RD_ASSERT_EQ(set.erase(6u), 0u);

  RD_ASSERT_EQ(set.bucket_count(), 2 * minimum_bucket_count);
  RD_ASSERT_EQ(set.size(), minimum_bucket_count + 1u - 3u);

  RD_ASSERT_EQ(it, ++set.begin());
  RD_ASSERT_EQ(*it, 7u);
  RD_ASSERT_EQ(*--set.end(), 5u);

  for (
      std::size_t next{}, last = minimum_bucket_count + 1u;
      next < last;
      ++next) {
    if (next == 1u || next == 8u || next == 6u) {
      RD_ASSERT_FALSE(set.contains(next));
      RD_ASSERT_EQ(set.bucket_size(next), 0u);
    } else {
      RD_ASSERT_TRUE(set.contains(next));
      RD_ASSERT_EQ(set.bucket(next), next);
      RD_ASSERT_EQ(set.bucket_size(next), 1u);
    }
  }

  set.
      erase(set
                .
                    begin(), set
                .
                    end()
  );

  for (
      std::size_t next{}, last = minimum_bucket_count + 1u;
      next < last;
      ++next) {
    RD_ASSERT_FALSE(set.contains(next));
  }

  RD_ASSERT_EQ(set.bucket_count(), 2 * minimum_bucket_count);
  RD_ASSERT_EQ(set.size(), 0u);
}

RD_TEST(DenseSet, EraseWithMovableKeyValue) {
  constexpr std::size_t minimum_bucket_count = 8u;
  rendu::dense_set<std::string> set;

  set.emplace("0");
  set.emplace("1");

  RD_ASSERT_EQ(set.bucket_count(), minimum_bucket_count);
  RD_ASSERT_EQ(set.size(), 2u);

  auto it = set.erase(set.find("0"));

  RD_ASSERT_EQ(*it, "1");
  RD_ASSERT_EQ(set.size(), 1u);
  RD_ASSERT_FALSE(set.contains("0"));
}

RD_TEST(DenseSet, EraseFromBucket) {
  constexpr std::size_t minimum_bucket_count = 8u;
  rendu::dense_set<std::size_t, std::identity> set;

  RD_ASSERT_EQ(set.bucket_count(), minimum_bucket_count);
  RD_ASSERT_EQ(set.size(), 0u);

  for (
      std::size_t next{};
      next < 4u; ++next) {
    RD_ASSERT_TRUE(set.emplace(2u * minimum_bucket_count * next).second);
    RD_ASSERT_TRUE(set.emplace(2u * minimum_bucket_count * next + 2u).second);
    RD_ASSERT_TRUE(set.emplace(2u * minimum_bucket_count * (next + 1u) - 1u).second);
  }

  RD_ASSERT_EQ(set.bucket_count(), 2u * minimum_bucket_count);
  RD_ASSERT_EQ(set.size(), 12u);

  RD_ASSERT_EQ(set.bucket_size(0u), 4u);
  RD_ASSERT_EQ(set.bucket_size(2u), 4u);
  RD_ASSERT_EQ(set.bucket_size(15u), 4u);

  set.
      erase(set
                .
                    end()
                - 3, set.
      end()
  );

  RD_ASSERT_EQ(set.bucket_count(), 2u * minimum_bucket_count);
  RD_ASSERT_EQ(set.size(), 9u);

  RD_ASSERT_EQ(set.bucket_size(0u), 3u);
  RD_ASSERT_EQ(set.bucket_size(2u), 3u);
  RD_ASSERT_EQ(set.bucket_size(15u), 3u);

  for (
      std::size_t next{};
      next < 3u; ++next) {
    RD_ASSERT_TRUE(set.contains(2u * minimum_bucket_count * next));
    RD_ASSERT_EQ(set.bucket(2u * minimum_bucket_count * next), 0u);

    RD_ASSERT_TRUE(set.contains(2u * minimum_bucket_count * next + 2u));
    RD_ASSERT_EQ(set.bucket(2u * minimum_bucket_count * next + 2u), 2u);

    RD_ASSERT_TRUE(set.contains(2u * minimum_bucket_count * (next + 1u) - 1u));
    RD_ASSERT_EQ(set.bucket(2u * minimum_bucket_count * (next + 1u) - 1u), 15u);
  }

  RD_ASSERT_FALSE(set.contains(2u * minimum_bucket_count * 3u));
  RD_ASSERT_FALSE(set.contains(2u * minimum_bucket_count * 3u + 2u));
  RD_ASSERT_FALSE(set.contains(2u * minimum_bucket_count * (3u + 1u) - 1u));

  set.erase(*++set.begin(0u));
  set.erase(*++set.begin(2u));
  set.erase(*++set.begin(15u));

  RD_ASSERT_EQ(set.bucket_count(), 2u * minimum_bucket_count);
  RD_ASSERT_EQ(set.size(), 6u);

  RD_ASSERT_EQ(set.bucket_size(0u), 2u);
  RD_ASSERT_EQ(set.bucket_size(2u), 2u);
  RD_ASSERT_EQ(set.bucket_size(15u), 2u);

  RD_ASSERT_FALSE(set.contains(2u * minimum_bucket_count * 1u));
  RD_ASSERT_FALSE(set.contains(2u * minimum_bucket_count * 1u + 2u));
  RD_ASSERT_FALSE(set.contains(2u * minimum_bucket_count * (1u + 1u) - 1u));

  while (set.begin(15) != set.end(15u)) {
    set.
        erase(*set
        .begin(15));
  }

  RD_ASSERT_EQ(set.bucket_count(), 2u * minimum_bucket_count);
  RD_ASSERT_EQ(set.size(), 4u);

  RD_ASSERT_EQ(set.bucket_size(0u), 2u);
  RD_ASSERT_EQ(set.bucket_size(2u), 2u);
  RD_ASSERT_EQ(set.bucket_size(15u), 0u);

  RD_ASSERT_TRUE(set.contains(0u * minimum_bucket_count));
  RD_ASSERT_TRUE(set.contains(0u * minimum_bucket_count + 2u));
  RD_ASSERT_TRUE(set.contains(4u * minimum_bucket_count));
  RD_ASSERT_TRUE(set.contains(4u * minimum_bucket_count + 2u));

  set.erase(4u * minimum_bucket_count + 2u);
  set.erase(0u * minimum_bucket_count);

  RD_ASSERT_EQ(set.bucket_count(), 2u * minimum_bucket_count);
  RD_ASSERT_EQ(set.size(), 2u);

  RD_ASSERT_EQ(set.bucket_size(0u), 1u);
  RD_ASSERT_EQ(set.bucket_size(2u), 1u);
  RD_ASSERT_EQ(set.bucket_size(15u), 0u);

  RD_ASSERT_FALSE(set.contains(0u * minimum_bucket_count));
  RD_ASSERT_TRUE(set.contains(0u * minimum_bucket_count + 2u));
  RD_ASSERT_TRUE(set.contains(4u * minimum_bucket_count));
  RD_ASSERT_FALSE(set.contains(4u * minimum_bucket_count + 2u));
}

RD_TEST(DenseSet, Swap) {
  rendu::dense_set<int> set;
  rendu::dense_set<int> other;

  set.emplace(0);

  RD_ASSERT_FALSE(set.empty());
  RD_ASSERT_TRUE(other.empty());
  RD_ASSERT_TRUE(set.contains(0));
  RD_ASSERT_FALSE(other.contains(0));

  set.
      swap(other);

  RD_ASSERT_TRUE(set.empty());
  RD_ASSERT_FALSE(other.empty());
  RD_ASSERT_FALSE(set.contains(0));
  RD_ASSERT_TRUE(other.contains(0));
}

RD_TEST(DenseSet, EqualRange) {
  rendu::dense_set<int, std::identity, transparent_equal_to> set;
  const auto &cset = set;

  set.emplace(42);

  RD_ASSERT_EQ(set.equal_range(0).first, set.end());
  RD_ASSERT_EQ(set.equal_range(0).second, set.end());

  RD_ASSERT_EQ(cset.equal_range(0).first, cset.cend());
  RD_ASSERT_EQ(cset.equal_range(0).second, cset.cend());

  RD_ASSERT_EQ(set.equal_range(0.0).first, set.end());
  RD_ASSERT_EQ(set.equal_range(0.0).second, set.end());

  RD_ASSERT_EQ(cset.equal_range(0.0).first, cset.cend());
  RD_ASSERT_EQ(cset.equal_range(0.0).second, cset.cend());

  RD_ASSERT_NE(set.equal_range(42).first, set.end());
  RD_ASSERT_EQ(*set.equal_range(42).first, 42);
  RD_ASSERT_EQ(set.equal_range(42).second, set.end());

  RD_ASSERT_NE(cset.equal_range(42).first, cset.cend());
  RD_ASSERT_EQ(*cset.equal_range(42).first, 42);
  RD_ASSERT_EQ(cset.equal_range(42).second, cset.cend());

  RD_ASSERT_NE(set.equal_range(42.0).first, set.end());
  RD_ASSERT_EQ(*set.equal_range(42.0).first, 42);
  RD_ASSERT_EQ(set.equal_range(42.0).second, set.end());

  RD_ASSERT_NE(cset.equal_range(42.0).first, cset.cend());
  RD_ASSERT_EQ(*cset.equal_range(42.0).first, 42);
  RD_ASSERT_EQ(cset.equal_range(42.0).second, cset.cend());
}

RD_TEST(DenseSet, LocalIterator) {
  using iterator = typename rendu::dense_set<std::size_t, std::identity>::local_iterator;

  static_assert(std::is_same_v<iterator::value_type, std::size_t>);
  static_assert(std::is_same_v<iterator::pointer, const std::size_t *>);
  static_assert(std::is_same_v<iterator::reference, const std::size_t &>);

  constexpr std::size_t minimum_bucket_count = 8u;
  rendu::dense_set<std::size_t, std::identity> set;
  set.emplace(3u);
  set.emplace(3u + minimum_bucket_count);

  iterator end{set.begin(3u)};
  iterator begin{};
  begin = set.end(3u);
  std::swap(begin, end
  );

  RD_ASSERT_EQ(begin, set.begin(3u));
  RD_ASSERT_EQ(end, set.end(3u));
  RD_ASSERT_NE(begin, end);

  RD_ASSERT_EQ(*begin.operator->(), 3u + minimum_bucket_count);
  RD_ASSERT_EQ(*begin, 3u + minimum_bucket_count);

  RD_ASSERT_EQ(begin++, set.begin(3u));
  RD_ASSERT_EQ(++begin, set.end(3u));
}

RD_TEST(DenseSet, ConstLocalIterator) {
  using iterator = typename rendu::dense_set<std::size_t, std::identity>::const_local_iterator;

  static_assert(std::is_same_v<iterator::value_type, std::size_t>);
  static_assert(std::is_same_v<iterator::pointer, const std::size_t *>);
  static_assert(std::is_same_v<iterator::reference, const std::size_t &>);

  constexpr std::size_t minimum_bucket_count = 8u;
  rendu::dense_set<std::size_t, std::identity> set;
  set.emplace(3u);
  set.emplace(3u + minimum_bucket_count);

  iterator cend{set.begin(3u)};
  iterator cbegin{};
  cbegin = set.end(3u);
  std::swap(cbegin, cend
  );

  RD_ASSERT_EQ(cbegin, set.begin(3u));
  RD_ASSERT_EQ(cend, set.end(3u));
  RD_ASSERT_NE(cbegin, cend);

  RD_ASSERT_EQ(*cbegin.operator->(), 3u + minimum_bucket_count);
  RD_ASSERT_EQ(*cbegin, 3u + minimum_bucket_count);

  RD_ASSERT_EQ(cbegin++, set.begin(3u));
  RD_ASSERT_EQ(++cbegin, set.end(3u));
}

RD_TEST(DenseSet, LocalIteratorConversion) {
  rendu::dense_set<int> set;
  set.emplace(3);

  typename rendu::dense_set<int>::local_iterator it = set.begin(set.bucket(3));
  typename rendu::dense_set<int>::const_local_iterator cit = it;

  static_assert(std::is_same_v<decltype(*it), const int &>);
  static_assert(std::is_same_v<decltype(*cit), const int &>);

  RD_ASSERT_EQ(*it, 3);
  RD_ASSERT_EQ(*it.operator->(), 3);
  RD_ASSERT_EQ(it.operator->(), cit.operator->());
  RD_ASSERT_EQ(*it, *cit);

  RD_ASSERT_EQ(it, cit);
  RD_ASSERT_NE(++cit, it);
}

RD_TEST(DenseSet, Rehash) {
  constexpr std::size_t minimum_bucket_count = 8u;
  rendu::dense_set<std::size_t, std::identity> set;
  set.emplace(32u);

  RD_ASSERT_EQ(set.bucket_count(), minimum_bucket_count);
  RD_ASSERT_TRUE(set.contains(32u));
  RD_ASSERT_EQ(set.bucket(32u), 0u);

  set.rehash(12u);

  RD_ASSERT_EQ(set.bucket_count(), 2u * minimum_bucket_count);
  RD_ASSERT_TRUE(set.contains(32u));
  RD_ASSERT_EQ(set.bucket(32u), 0u);

  set.rehash(44u);

  RD_ASSERT_EQ(set.bucket_count(), 8u * minimum_bucket_count);
  RD_ASSERT_TRUE(set.contains(32u));
  RD_ASSERT_EQ(set.bucket(32u), 32u);

  set.rehash(0u);

  RD_ASSERT_EQ(set.bucket_count(), minimum_bucket_count);
  RD_ASSERT_TRUE(set.contains(32u));
  RD_ASSERT_EQ(set.bucket(32u), 0u);

  for (
      std::size_t next{};
      next < minimum_bucket_count;
      ++next) {
    set.
        emplace(next);
  }

  RD_ASSERT_EQ(set.size(), minimum_bucket_count + 1u);
  RD_ASSERT_EQ(set.bucket_count(), 2u * minimum_bucket_count);

  set.rehash(0u);

  RD_ASSERT_EQ(set.bucket_count(), 2u * minimum_bucket_count);
  RD_ASSERT_TRUE(set.contains(32u));

  set.rehash(55u);

  RD_ASSERT_EQ(set.bucket_count(), 8u * minimum_bucket_count);
  RD_ASSERT_TRUE(set.contains(32u));

  set.rehash(2u);

  RD_ASSERT_EQ(set.bucket_count(), 2u * minimum_bucket_count);
  RD_ASSERT_TRUE(set.contains(32u));
  RD_ASSERT_EQ(set.bucket(32u), 0u);

  for (
      std::size_t next{};
      next < minimum_bucket_count;
      ++next) {
    RD_ASSERT_TRUE(set.contains(next));
    RD_ASSERT_EQ(set.bucket(next), next);
  }

  RD_ASSERT_EQ(set.bucket_size(0u), 2u);
  RD_ASSERT_EQ(set.bucket_size(3u), 1u);

  RD_ASSERT_EQ(*set.begin(0u), 0u);
  RD_ASSERT_EQ(*++set.begin(0u), 32u);

  set.
      clear();
  set.rehash(2u);

  RD_ASSERT_EQ(set.bucket_count(), minimum_bucket_count);
  RD_ASSERT_FALSE(set.contains(32u));

  for (
      std::size_t next{};
      next < minimum_bucket_count;
      ++next) {
    RD_ASSERT_FALSE(set.contains(next));
  }

  RD_ASSERT_EQ(set.bucket_size(0u), 0u);
  RD_ASSERT_EQ(set.bucket_size(3u), 0u);
}

RD_TEST(DenseSet, Reserve) {
  constexpr std::size_t minimum_bucket_count = 8u;
  rendu::dense_set<int> set;

  RD_ASSERT_EQ(set.bucket_count(), minimum_bucket_count);

  set.reserve(0u);

  RD_ASSERT_EQ(set.bucket_count(), minimum_bucket_count);

  set.
      reserve(minimum_bucket_count);

  RD_ASSERT_EQ(set.bucket_count(), 2 * minimum_bucket_count);
  RD_ASSERT_EQ(set.bucket_count(),
               rendu::next_power_of_two(static_cast<std::size_t>(std::ceil(
                   minimum_bucket_count / set.max_load_factor()))));
}

RD_TEST(DenseSet, ThrowingAllocator) {
  using allocator = test::throwing_allocator<std::size_t>;
  using packed_allocator = test::throwing_allocator<std::pair<std::size_t, std::size_t>>;
  using packed_exception = typename packed_allocator::exception_type;

  constexpr std::size_t minimum_bucket_count = 8u;
  rendu::dense_set<std::size_t, std::hash<std::size_t>, std::equal_to<std::size_t>, allocator> set{};

  packed_allocator::trigger_on_allocate = true;

  RD_ASSERT_EQ(set.bucket_count(), minimum_bucket_count);
  RD_ASSERT_THROW(set.reserve(2u * set.bucket_count()), packed_exception);
  RD_ASSERT_EQ(set.bucket_count(), minimum_bucket_count);

  packed_allocator::trigger_on_allocate = true;

  RD_ASSERT_THROW(set.emplace(), packed_exception);
  RD_ASSERT_FALSE(set.contains(0u));

  packed_allocator::trigger_on_allocate = true;

  RD_ASSERT_THROW(set.emplace(std::size_t{}), packed_exception);
  RD_ASSERT_FALSE(set.contains(0u));

  packed_allocator::trigger_on_allocate = true;

  RD_ASSERT_THROW(set.insert(0u), packed_exception);
  RD_ASSERT_FALSE(set.contains(0u));
}

#if defined(ENTT_HAS_TRACKED_MEMORY_RESOURCE)

RD_TEST(DenseSet, NoUsesAllocatorConstruction) {
    using allocator = std::pmr::polymorphic_allocator<int>;

    test::tracked_memory_resource memory_resource{};
    rendu::dense_set<int, std::hash<int>, std::equal_to<int>, allocator> set{&memory_resource};

    set.reserve(1u);
    memory_resource.reset();
    set.emplace(0);

    RD_ASSERT_TRUE(set.get_allocator().resource()->is_equal(memory_resource));
    RD_ASSERT_EQ(memory_resource.do_allocate_counter(), 0u);
    RD_ASSERT_EQ(memory_resource.do_deallocate_counter(), 0u);
}

RD_TEST(DenseSet, UsesAllocatorConstruction) {
    using string_type = typename test::tracked_memory_resource::string_type;
    using allocator = std::pmr::polymorphic_allocator<string_type>;

    test::tracked_memory_resource memory_resource{};
    rendu::dense_set<string_type, std::hash<string_type>, std::equal_to<string_type>, allocator> set{&memory_resource};

    set.reserve(1u);
    memory_resource.reset();
    set.emplace(test::tracked_memory_resource::default_value);

    RD_ASSERT_TRUE(set.get_allocator().resource()->is_equal(memory_resource));
    RD_ASSERT_GT(memory_resource.do_allocate_counter(), 0u);
    RD_ASSERT_EQ(memory_resource.do_deallocate_counter(), 0u);
}
#endif
}
