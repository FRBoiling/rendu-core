/*
* Created by boil on 2023/2/26.
*/

#include <array>
#include <test/rdtest.h>
#include <core/base/hashed_string.h>
#include <core/meta/container.h>
#include <core/meta/factory.h>
#include <core/meta/meta.h>
#include <core/meta/resolve.h>

namespace meta_container {

struct invalid_type {};

struct MetaContainer : ::testing::Test {
  void SetUp() override {
    using namespace rendu::literals;

    rendu::meta<double>()
        .type("double"_hs);

    rendu::meta<int>()
        .type("int"_hs);
  }

  void TearDown() override {
    rendu::meta_reset();
  }
};

using MetaContainerDeathTest = MetaContainer;

RD_TEST_F(MetaContainer, InvalidContainer) {
  RD_ASSERT_FALSE(rendu::meta_any{42}.as_sequence_container());
  RD_ASSERT_FALSE(rendu::meta_any{42}.as_associative_container());

  RD_ASSERT_FALSE((rendu::meta_any{std::map<int, char>{}}.as_sequence_container()));
  RD_ASSERT_FALSE(rendu::meta_any{std::vector<int>{}}.as_associative_container());
}

RD_TEST_F(MetaContainer, EmptySequenceContainer) {
  rendu::meta_sequence_container container{};

  RD_ASSERT_FALSE(container);

  rendu::meta_any any{std::vector<int>{}};
  container = any.as_sequence_container();

  RD_ASSERT_TRUE(container);
}

RD_TEST_F(MetaContainer, EmptyAssociativeContainer) {
  rendu::meta_associative_container container{};

  RD_ASSERT_FALSE(container);

  rendu::meta_any any{std::map<int, char>{}};
  container = any.as_associative_container();

  RD_ASSERT_TRUE(container);
}

RD_TEST_F(MetaContainer, SequenceContainerIterator) {
  std::vector<int> vec{2, 3, 4};
  auto any = rendu::forward_as_meta(vec);
  rendu::meta_sequence_container::iterator first{};
  auto view = any.as_sequence_container();

  RD_ASSERT_FALSE(first);

  first = view.begin();
  const auto last = view.end();

  RD_ASSERT_TRUE(first);
  RD_ASSERT_TRUE(last);

  RD_ASSERT_FALSE(first == last);
  RD_ASSERT_TRUE(first != last);

  RD_ASSERT_EQ((first++)->cast<int>(), 2);
  RD_ASSERT_EQ((++first)->cast<int>(), 4);

  RD_ASSERT_NE(first++, last);
  RD_ASSERT_TRUE(first == last);
  RD_ASSERT_FALSE(first != last);
  RD_ASSERT_EQ(first--, last);

  RD_ASSERT_EQ((first--)->cast<int>(), 4);
  RD_ASSERT_EQ((--first)->cast<int>(), 2);
}

RD_TEST_F(MetaContainer, AssociativeContainerIterator) {
  std::map<int, char> map{{2, 'c'}, {3, 'd'}, {4, 'e'}};
  auto any = rendu::forward_as_meta(map);
  rendu::meta_associative_container::iterator first{};
  auto view = any.as_associative_container();

  RD_ASSERT_FALSE(first);

  first = view.begin();
  const auto last = view.end();

  RD_ASSERT_TRUE(first);
  RD_ASSERT_TRUE(last);

  RD_ASSERT_FALSE(first == last);
  RD_ASSERT_TRUE(first != last);

  RD_ASSERT_NE(first, last);
  RD_ASSERT_EQ((first++)->first.cast<int>(), 2);
  RD_ASSERT_EQ((++first)->second.cast<char>(), 'e');
  RD_ASSERT_NE(first++, last);
  RD_ASSERT_EQ(first, last);

  RD_ASSERT_TRUE(first == last);
  RD_ASSERT_FALSE(first != last);
}

RD_TEST_F(MetaContainer, StdVector) {
  std::vector<int> vec{};
  auto any = rendu::forward_as_meta(vec);
  auto view = any.as_sequence_container();
  auto cview = std::as_const(any).as_sequence_container();

  RD_ASSERT_TRUE(view);
  RD_ASSERT_EQ(view.value_type(), rendu::resolve<int>());

  RD_ASSERT_EQ(view.size(), 0u);
  RD_ASSERT_EQ(view.begin(), view.end());
  RD_ASSERT_TRUE(view.resize(3u));
  RD_ASSERT_EQ(view.size(), 3u);
  RD_ASSERT_NE(view.begin(), view.end());

  view[0].cast<int &>() = 2;
  view[1].cast<int &>() = 3;
  view[2].cast<int &>() = 4;

  RD_ASSERT_EQ(view[1u].cast<int>(), 3);

  auto it = view.begin();
  auto ret = view.insert(it, 0);

  RD_ASSERT_TRUE(ret);
  RD_ASSERT_FALSE(view.insert(ret, invalid_type{}));
  RD_ASSERT_TRUE(view.insert(++ret, 1.));

  RD_ASSERT_EQ(view.size(), 5u);
  RD_ASSERT_EQ(view.begin()->cast<int>(), 0);
  RD_ASSERT_EQ((++view.begin())->cast<int>(), 1);

  ret = view.insert(cview.end(), 42);

  RD_ASSERT_TRUE(ret);
  RD_ASSERT_EQ(*ret, 42);

  it = view.begin();
  ret = view.erase(it);

  RD_ASSERT_TRUE(ret);
  RD_ASSERT_EQ(view.size(), 5u);
  RD_ASSERT_EQ(ret->cast<int>(), 1);

  ret = view.erase(cview.begin());

  RD_ASSERT_TRUE(ret);
  RD_ASSERT_EQ(view.size(), 4u);
  RD_ASSERT_EQ(ret->cast<int>(), 2);

  RD_ASSERT_TRUE(view.clear());
  RD_ASSERT_EQ(view.size(), 0u);
}

RD_TEST_F(MetaContainer, StdArray) {
  std::array<int, 3> arr{};
  auto any = rendu::forward_as_meta(arr);
  auto view = any.as_sequence_container();

  RD_ASSERT_TRUE(view);
  RD_ASSERT_EQ(view.value_type(), rendu::resolve<int>());

  RD_ASSERT_EQ(view.size(), 3u);
  RD_ASSERT_NE(view.begin(), view.end());
  RD_ASSERT_FALSE(view.resize(5u));
  RD_ASSERT_EQ(view.size(), 3u);

  view[0].cast<int &>() = 2;
  view[1].cast<int &>() = 3;
  view[2].cast<int &>() = 4;

  RD_ASSERT_EQ(view[1u].cast<int>(), 3);

  auto it = view.begin();
  auto ret = view.insert(it, 0);

  RD_ASSERT_FALSE(ret);
  RD_ASSERT_FALSE(view.insert(it, 'c'));
  RD_ASSERT_FALSE(view.insert(++it, 1.));

  RD_ASSERT_EQ(view.size(), 3u);
  RD_ASSERT_EQ(view.begin()->cast<int>(), 2);
  RD_ASSERT_EQ((++view.begin())->cast<int>(), 3);

  it = view.begin();
  ret = view.erase(it);

  RD_ASSERT_FALSE(ret);
  RD_ASSERT_EQ(view.size(), 3u);
  RD_ASSERT_EQ(it->cast<int>(), 2);

  RD_ASSERT_FALSE(view.clear());
  RD_ASSERT_EQ(view.size(), 3u);
}

RD_TEST_F(MetaContainer, StdList) {
  std::list<int> list{};
  auto any = rendu::forward_as_meta(list);
  auto view = any.as_sequence_container();
  auto cview = std::as_const(any).as_sequence_container();

  RD_ASSERT_TRUE(view);
  RD_ASSERT_EQ(view.value_type(), rendu::resolve<int>());

  RD_ASSERT_EQ(view.size(), 0u);
  RD_ASSERT_EQ(view.begin(), view.end());
  RD_ASSERT_TRUE(view.resize(3u));
  RD_ASSERT_EQ(view.size(), 3u);
  RD_ASSERT_NE(view.begin(), view.end());

  view[0].cast<int &>() = 2;
  view[1].cast<int &>() = 3;
  view[2].cast<int &>() = 4;

  RD_ASSERT_EQ(view[1u].cast<int>(), 3);

  auto it = view.begin();
  auto ret = view.insert(it, 0);

  RD_ASSERT_TRUE(ret);
  RD_ASSERT_FALSE(view.insert(ret, invalid_type{}));
  RD_ASSERT_TRUE(view.insert(++ret, 1.));

  RD_ASSERT_EQ(view.size(), 5u);
  RD_ASSERT_EQ(view.begin()->cast<int>(), 0);
  RD_ASSERT_EQ((++view.begin())->cast<int>(), 1);

  ret = view.insert(cview.end(), 42);

  RD_ASSERT_TRUE(ret);
  RD_ASSERT_EQ(*ret, 42);

  it = view.begin();
  ret = view.erase(it);

  RD_ASSERT_TRUE(ret);
  RD_ASSERT_EQ(view.size(), 5u);
  RD_ASSERT_EQ(ret->cast<int>(), 1);

  ret = view.erase(cview.begin());

  RD_ASSERT_TRUE(ret);
  RD_ASSERT_EQ(view.size(), 4u);
  RD_ASSERT_EQ(ret->cast<int>(), 2);

  RD_ASSERT_TRUE(view.clear());
  RD_ASSERT_EQ(view.size(), 0u);
}

RD_TEST_F(MetaContainer, StdDeque) {
  std::deque<int> deque{};
  auto any = rendu::forward_as_meta(deque);
  auto view = any.as_sequence_container();
  auto cview = std::as_const(any).as_sequence_container();

  RD_ASSERT_TRUE(view);
  RD_ASSERT_EQ(view.value_type(), rendu::resolve<int>());

  RD_ASSERT_EQ(view.size(), 0u);
  RD_ASSERT_EQ(view.begin(), view.end());
  RD_ASSERT_TRUE(view.resize(3u));
  RD_ASSERT_EQ(view.size(), 3u);
  RD_ASSERT_NE(view.begin(), view.end());

  view[0].cast<int &>() = 2;
  view[1].cast<int &>() = 3;
  view[2].cast<int &>() = 4;

  RD_ASSERT_EQ(view[1u].cast<int>(), 3);

  auto it = view.begin();
  auto ret = view.insert(it, 0);

  RD_ASSERT_TRUE(ret);
  RD_ASSERT_FALSE(view.insert(ret, invalid_type{}));
  RD_ASSERT_TRUE(view.insert(++ret, 1.));

  RD_ASSERT_EQ(view.size(), 5u);
  RD_ASSERT_EQ(view.begin()->cast<int>(), 0);
  RD_ASSERT_EQ((++view.begin())->cast<int>(), 1);

  ret = view.insert(cview.end(), 42);

  RD_ASSERT_TRUE(ret);
  RD_ASSERT_EQ(*ret, 42);

  it = view.begin();
  ret = view.erase(it);

  RD_ASSERT_TRUE(ret);
  RD_ASSERT_EQ(view.size(), 5u);
  RD_ASSERT_EQ(ret->cast<int>(), 1);

  ret = view.erase(cview.begin());

  RD_ASSERT_TRUE(ret);
  RD_ASSERT_EQ(view.size(), 4u);
  RD_ASSERT_EQ(ret->cast<int>(), 2);

  RD_ASSERT_TRUE(view.clear());
  RD_ASSERT_EQ(view.size(), 0u);
}

RD_TEST_F(MetaContainer, StdMap) {
  std::map<int, char> map{{2, 'c'}, {3, 'd'}, {4, 'e'}};
  auto any = rendu::forward_as_meta(map);
  auto view = any.as_associative_container();

  RD_ASSERT_TRUE(view);
  RD_ASSERT_FALSE(view.key_only());
  RD_ASSERT_EQ(view.key_type(), rendu::resolve<int>());
  RD_ASSERT_EQ(view.mapped_type(), rendu::resolve<char>());
  RD_ASSERT_EQ(view.value_type(), (rendu::resolve<std::pair<const int, char>>()));

  RD_ASSERT_EQ(view.size(), 3u);
  RD_ASSERT_NE(view.begin(), view.end());

  RD_ASSERT_EQ(view.find(3)->second.cast<char>(), 'd');

  RD_ASSERT_FALSE(view.insert(invalid_type{}, 'a'));
  RD_ASSERT_FALSE(view.insert(1, invalid_type{}));

  RD_ASSERT_TRUE(view.insert(0, 'a'));
  RD_ASSERT_TRUE(view.insert(1., static_cast<int>('b')));

  RD_ASSERT_EQ(view.size(), 5u);
  RD_ASSERT_EQ(view.find(0)->second.cast<char>(), 'a');
  RD_ASSERT_EQ(view.find(1.)->second.cast<char>(), 'b');

  RD_ASSERT_EQ(view.erase(invalid_type{}), 0u);
  RD_ASSERT_FALSE(view.find(invalid_type{}));
  RD_ASSERT_EQ(view.size(), 5u);

  RD_ASSERT_EQ(view.erase(0), 1u);
  RD_ASSERT_EQ(view.size(), 4u);
  RD_ASSERT_EQ(view.find(0), view.end());

  view.find(1.)->second.cast<char &>() = 'f';

  RD_ASSERT_EQ(view.find(1.f)->second.cast<char>(), 'f');

  RD_ASSERT_EQ(view.erase(1.), 1u);
  RD_ASSERT_TRUE(view.clear());
  RD_ASSERT_EQ(view.size(), 0u);
}

RD_TEST_F(MetaContainer, StdSet) {
  std::set<int> set{2, 3, 4};
  auto any = rendu::forward_as_meta(set);
  auto view = any.as_associative_container();

  RD_ASSERT_TRUE(view);
  RD_ASSERT_TRUE(view.key_only());
  RD_ASSERT_EQ(view.key_type(), rendu::resolve<int>());
  RD_ASSERT_EQ(view.mapped_type(), rendu::meta_type{});
  RD_ASSERT_EQ(view.value_type(), rendu::resolve<int>());

  RD_ASSERT_EQ(view.size(), 3u);
  RD_ASSERT_NE(view.begin(), view.end());

  RD_ASSERT_EQ(view.find(3)->first.cast<int>(), 3);

  RD_ASSERT_FALSE(view.insert(invalid_type{}));

  RD_ASSERT_TRUE(view.insert(.0));
  RD_ASSERT_TRUE(view.insert(1));

  RD_ASSERT_EQ(view.size(), 5u);
  RD_ASSERT_EQ(view.find(0)->first.cast<int>(), 0);
  RD_ASSERT_EQ(view.find(1.)->first.cast<int>(), 1);

  RD_ASSERT_EQ(view.erase(invalid_type{}), 0u);
  RD_ASSERT_FALSE(view.find(invalid_type{}));
  RD_ASSERT_EQ(view.size(), 5u);

  RD_ASSERT_EQ(view.erase(0), 1u);
  RD_ASSERT_EQ(view.size(), 4u);
  RD_ASSERT_EQ(view.find(0), view.end());

  RD_ASSERT_EQ(view.find(1.f)->first.try_cast<int>(), nullptr);
  RD_ASSERT_NE(view.find(1.)->first.try_cast<const int>(), nullptr);
  RD_ASSERT_EQ(view.find(true)->first.cast<const int &>(), 1);

  RD_ASSERT_EQ(view.erase(1.), 1u);
  RD_ASSERT_TRUE(view.clear());
  RD_ASSERT_EQ(view.size(), 0u);
}

RD_TEST_F(MetaContainer, DenseMap) {
  rendu::dense_map<int, char> map{};
  auto any = rendu::forward_as_meta(map);
  auto view = any.as_associative_container();

  map.emplace(2, 'c');
  map.emplace(3, 'd');
  map.emplace(4, '3');

  RD_ASSERT_TRUE(view);
  RD_ASSERT_FALSE(view.key_only());
  RD_ASSERT_EQ(view.key_type(), rendu::resolve<int>());
  RD_ASSERT_EQ(view.mapped_type(), rendu::resolve<char>());
  RD_ASSERT_EQ(view.value_type(), (rendu::resolve<std::pair<const int, char>>()));

  RD_ASSERT_EQ(view.size(), 3u);
  RD_ASSERT_NE(view.begin(), view.end());

  RD_ASSERT_EQ(view.find(3)->second.cast<char>(), 'd');

  RD_ASSERT_FALSE(view.insert(invalid_type{}, 'a'));
  RD_ASSERT_FALSE(view.insert(1, invalid_type{}));

  RD_ASSERT_TRUE(view.insert(0, 'a'));
  RD_ASSERT_TRUE(view.insert(1., static_cast<int>('b')));

  RD_ASSERT_EQ(view.size(), 5u);
  RD_ASSERT_EQ(view.find(0)->second.cast<char>(), 'a');
  RD_ASSERT_EQ(view.find(1.)->second.cast<char>(), 'b');

  RD_ASSERT_EQ(view.erase(invalid_type{}), 0u);
  RD_ASSERT_FALSE(view.find(invalid_type{}));
  RD_ASSERT_EQ(view.size(), 5u);

  RD_ASSERT_EQ(view.erase(0), 1u);
  RD_ASSERT_EQ(view.size(), 4u);
  RD_ASSERT_EQ(view.find(0), view.end());

  view.find(1.)->second.cast<char &>() = 'f';

  RD_ASSERT_EQ(view.find(1.f)->second.cast<char>(), 'f');

  RD_ASSERT_EQ(view.erase(1.), 1u);
  RD_ASSERT_TRUE(view.clear());
  RD_ASSERT_EQ(view.size(), 0u);
}

RD_TEST_F(MetaContainer, DenseSet) {
  rendu::dense_set<int> set{};
  auto any = rendu::forward_as_meta(set);
  auto view = any.as_associative_container();

  set.emplace(2);
  set.emplace(3);
  set.emplace(4);

  RD_ASSERT_TRUE(view);
  RD_ASSERT_TRUE(view.key_only());
  RD_ASSERT_EQ(view.key_type(), rendu::resolve<int>());
  RD_ASSERT_EQ(view.mapped_type(), rendu::meta_type{});
  RD_ASSERT_EQ(view.value_type(), rendu::resolve<int>());

  RD_ASSERT_EQ(view.size(), 3u);
  RD_ASSERT_NE(view.begin(), view.end());

  RD_ASSERT_EQ(view.find(3)->first.cast<int>(), 3);

  RD_ASSERT_FALSE(view.insert(invalid_type{}));

  RD_ASSERT_TRUE(view.insert(.0));
  RD_ASSERT_TRUE(view.insert(1));

  RD_ASSERT_EQ(view.size(), 5u);
  RD_ASSERT_EQ(view.find(0)->first.cast<int>(), 0);
  RD_ASSERT_EQ(view.find(1.)->first.cast<int>(), 1);

  RD_ASSERT_EQ(view.erase(invalid_type{}), 0u);
  RD_ASSERT_FALSE(view.find(invalid_type{}));
  RD_ASSERT_EQ(view.size(), 5u);

  RD_ASSERT_EQ(view.erase(0), 1u);
  RD_ASSERT_EQ(view.size(), 4u);
  RD_ASSERT_EQ(view.find(0), view.end());

  RD_ASSERT_EQ(view.find(1.f)->first.try_cast<int>(), nullptr);
  RD_ASSERT_NE(view.find(1.)->first.try_cast<const int>(), nullptr);
  RD_ASSERT_EQ(view.find(true)->first.cast<const int &>(), 1);

  RD_ASSERT_EQ(view.erase(1.), 1u);
  RD_ASSERT_TRUE(view.clear());
  RD_ASSERT_EQ(view.size(), 0u);
}

RD_TEST_F(MetaContainer, ConstSequenceContainer) {
  std::vector<int> vec{};
  auto any = rendu::forward_as_meta(std::as_const(vec));
  auto view = any.as_sequence_container();

  RD_ASSERT_TRUE(view);
  RD_ASSERT_EQ(view.value_type(), rendu::resolve<int>());

  RD_ASSERT_EQ(view.size(), 0u);
  RD_ASSERT_EQ(view.begin(), view.end());
  RD_ASSERT_FALSE(view.resize(3u));
  RD_ASSERT_EQ(view.size(), 0u);
  RD_ASSERT_EQ(view.begin(), view.end());

  vec.push_back(42);

  RD_ASSERT_EQ(view.size(), 1u);
  RD_ASSERT_NE(view.begin(), view.end());
  RD_ASSERT_EQ(view[0].cast<const int &>(), 42);

  auto it = view.begin();
  auto ret = view.insert(it, 0);

  RD_ASSERT_FALSE(ret);
  RD_ASSERT_EQ(view.size(), 1u);
  RD_ASSERT_EQ(it->cast<int>(), 42);
  RD_ASSERT_EQ(++it, view.end());

  it = view.begin();
  ret = view.erase(it);

  RD_ASSERT_FALSE(ret);
  RD_ASSERT_EQ(view.size(), 1u);

  RD_ASSERT_FALSE(view.clear());
  RD_ASSERT_EQ(view.size(), 1u);
}

RD_DEBUG_TEST_F(MetaContainerDeathTest, ConstSequenceContainer) {
  std::vector<int> vec{};
  auto any = rendu::forward_as_meta(std::as_const(vec));
  auto view = any.as_sequence_container();

  RD_ASSERT_TRUE(view);
  RD_ASSERT_DEATH(view[0].cast<int &>() = 2, "");
}

RD_TEST_F(MetaContainer, ConstKeyValueAssociativeContainer) {
  std::map<int, char> map{};
  auto any = rendu::forward_as_meta(std::as_const(map));
  auto view = any.as_associative_container();

  RD_ASSERT_TRUE(view);
  RD_ASSERT_FALSE(view.key_only());
  RD_ASSERT_EQ(view.key_type(), rendu::resolve<int>());
  RD_ASSERT_EQ(view.mapped_type(), rendu::resolve<char>());
  RD_ASSERT_EQ(view.value_type(), (rendu::resolve<std::pair<const int, char>>()));

  RD_ASSERT_EQ(view.size(), 0u);
  RD_ASSERT_EQ(view.begin(), view.end());

  map[2] = 'c';

  RD_ASSERT_EQ(view.size(), 1u);
  RD_ASSERT_NE(view.begin(), view.end());
  RD_ASSERT_EQ(view.find(2)->second.cast<const char &>(), 'c');

  RD_ASSERT_FALSE(view.insert(0, 'a'));
  RD_ASSERT_EQ(view.size(), 1u);
  RD_ASSERT_EQ(view.find(0), view.end());
  RD_ASSERT_EQ(view.find(2)->second.cast<char>(), 'c');

  RD_ASSERT_EQ(view.erase(2), 0u);
  RD_ASSERT_EQ(view.size(), 1u);
  RD_ASSERT_NE(view.find(2), view.end());

  RD_ASSERT_FALSE(view.clear());
  RD_ASSERT_EQ(view.size(), 1u);
}

RD_DEBUG_TEST_F(MetaContainerDeathTest, ConstKeyValueAssociativeContainer) {
  std::map<int, char> map{};
  auto any = rendu::forward_as_meta(std::as_const(map));
  auto view = any.as_associative_container();

  RD_ASSERT_TRUE(view);
  RD_ASSERT_DEATH(view.find(2)->second.cast<char &>() = 'a', "");
}

RD_TEST_F(MetaContainer, ConstKeyOnlyAssociativeContainer) {
  std::set<int> set{};
  auto any = rendu::forward_as_meta(std::as_const(set));
  auto view = any.as_associative_container();

  RD_ASSERT_TRUE(view);
  RD_ASSERT_TRUE(view.key_only());
  RD_ASSERT_EQ(view.key_type(), rendu::resolve<int>());
  RD_ASSERT_EQ(view.mapped_type(), rendu::meta_type{});
  RD_ASSERT_EQ(view.value_type(), (rendu::resolve<int>()));

  RD_ASSERT_EQ(view.size(), 0u);
  RD_ASSERT_EQ(view.begin(), view.end());

  set.insert(2);

  RD_ASSERT_EQ(view.size(), 1u);
  RD_ASSERT_NE(view.begin(), view.end());

  RD_ASSERT_EQ(view.find(2)->first.try_cast<int>(), nullptr);
  RD_ASSERT_NE(view.find(2)->first.try_cast<const int>(), nullptr);
  RD_ASSERT_EQ(view.find(2)->first.cast<int>(), 2);
  RD_ASSERT_EQ(view.find(2)->first.cast<const int &>(), 2);

  RD_ASSERT_FALSE(view.insert(0));
  RD_ASSERT_EQ(view.size(), 1u);
  RD_ASSERT_EQ(view.find(0), view.end());
  RD_ASSERT_EQ(view.find(2)->first.cast<int>(), 2);

  RD_ASSERT_EQ(view.erase(2), 0u);
  RD_ASSERT_EQ(view.size(), 1u);
  RD_ASSERT_NE(view.find(2), view.end());

  RD_ASSERT_FALSE(view.clear());
  RD_ASSERT_EQ(view.size(), 1u);
}

RD_TEST_F(MetaContainer, SequenceContainerConstMetaAny) {
  auto test = [](const rendu::meta_any any) {
    auto view = any.as_sequence_container();

    RD_ASSERT_TRUE(view);
    RD_ASSERT_EQ(view.value_type(), rendu::resolve<int>());
    RD_ASSERT_EQ(view[0].cast<const int &>(), 42);
  };

  std::vector<int> vec{42};

  test(vec);
  test(rendu::forward_as_meta(vec));
  test(rendu::forward_as_meta(std::as_const(vec)));
}

RD_DEBUG_TEST_F(MetaContainerDeathTest, SequenceContainerConstMetaAny) {
  auto test = [](const rendu::meta_any any) {
    auto view = any.as_sequence_container();

    RD_ASSERT_TRUE(view);
    RD_ASSERT_DEATH(view[0].cast<int &>() = 2, "");
  };

  std::vector<int> vec{42};

  test(vec);
  test(rendu::forward_as_meta(vec));
  test(rendu::forward_as_meta(std::as_const(vec)));
}

RD_TEST_F(MetaContainer, KeyValueAssociativeContainerConstMetaAny) {
  auto test = [](const rendu::meta_any any) {
    auto view = any.as_associative_container();

    RD_ASSERT_TRUE(view);
    RD_ASSERT_EQ(view.value_type(), (rendu::resolve<std::pair<const int, char>>()));
    RD_ASSERT_EQ(view.find(2)->second.cast<const char &>(), 'c');
  };

  std::map<int, char> map{{2, 'c'}};

  test(map);
  test(rendu::forward_as_meta(map));
  test(rendu::forward_as_meta(std::as_const(map)));
}

RD_DEBUG_TEST_F(MetaContainerDeathTest, KeyValueAssociativeContainerConstMetaAny) {
  auto test = [](const rendu::meta_any any) {
    auto view = any.as_associative_container();

    RD_ASSERT_TRUE(view);
    RD_ASSERT_DEATH(view.find(2)->second.cast<char &>() = 'a', "");
  };

  std::map<int, char> map{{2, 'c'}};

  test(map);
  test(rendu::forward_as_meta(map));
  test(rendu::forward_as_meta(std::as_const(map)));
}

RD_TEST_F(MetaContainer, KeyOnlyAssociativeContainerConstMetaAny) {
  auto test = [](const rendu::meta_any any) {
    auto view = any.as_associative_container();

    RD_ASSERT_TRUE(view);
    RD_ASSERT_EQ(view.value_type(), (rendu::resolve<int>()));

    RD_ASSERT_EQ(view.find(2)->first.try_cast<int>(), nullptr);
    RD_ASSERT_NE(view.find(2)->first.try_cast<const int>(), nullptr);
    RD_ASSERT_EQ(view.find(2)->first.cast<int>(), 2);
    RD_ASSERT_EQ(view.find(2)->first.cast<const int &>(), 2);
  };

  std::set<int> set{2};

  test(set);
  test(rendu::forward_as_meta(set));
  test(rendu::forward_as_meta(std::as_const(set)));
}

RD_TEST_F(MetaContainer, StdVectorBool) {
  using proxy_type = typename std::vector<bool>::reference;
  using const_proxy_type = typename std::vector<bool>::const_reference;

  std::vector<bool> vec{};
  auto any = rendu::forward_as_meta(vec);
  auto cany = std::as_const(any).as_ref();

  auto view = any.as_sequence_container();
  auto cview = cany.as_sequence_container();

  RD_ASSERT_TRUE(view);
  RD_ASSERT_EQ(view.value_type(), rendu::resolve<bool>());

  RD_ASSERT_EQ(view.size(), 0u);
  RD_ASSERT_EQ(view.begin(), view.end());
  RD_ASSERT_TRUE(view.resize(3u));
  RD_ASSERT_EQ(view.size(), 3u);
  RD_ASSERT_NE(view.begin(), view.end());

  view[0].cast<proxy_type>() = true;
  view[1].cast<proxy_type>() = true;
  view[2].cast<proxy_type>() = false;

  RD_ASSERT_EQ(cview[1u].cast<const_proxy_type>(), true);

  auto it = view.begin();
  auto ret = view.insert(it, true);

  RD_ASSERT_TRUE(ret);
  RD_ASSERT_FALSE(view.insert(ret, invalid_type{}));
  RD_ASSERT_TRUE(view.insert(++ret, false));

  RD_ASSERT_EQ(view.size(), 5u);
  RD_ASSERT_EQ(view.begin()->cast<proxy_type>(), true);
  RD_ASSERT_EQ((++cview.begin())->cast<const_proxy_type>(), false);

  it = view.begin();
  ret = view.erase(it);

  RD_ASSERT_TRUE(ret);
  RD_ASSERT_EQ(view.size(), 4u);
  RD_ASSERT_EQ(ret->cast<proxy_type>(), false);

  RD_ASSERT_TRUE(view.clear());
  RD_ASSERT_EQ(cview.size(), 0u);
}
}
