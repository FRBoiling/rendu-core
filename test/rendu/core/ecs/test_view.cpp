/*
* Created by boil on 2023/2/19.
*/

#include <test/rdtest.h>
#include <ecs/entity.h>
#include <ecs/registry.h>
#include <ecs/view.h>

struct empty_type {};

struct stable_type {
  static constexpr auto in_place_delete = true;
  int value;
};

RD_TEST(SingleComponentView, Functionalities) {
  rendu::registry registry;
  auto view = registry.view<char>();
  auto cview = std::as_const(registry).view<const char>();

  const auto e0 = registry.create();
  const auto e1 = registry.create();

  RD_ASSERT_TRUE(view.empty());

  registry.emplace<int>(e1);
  registry.emplace<char>(e1);

  RD_ASSERT_NO_FATAL_FAILURE(view.begin()++);
  RD_ASSERT_NO_FATAL_FAILURE(++cview.begin());
  RD_ASSERT_NO_FATAL_FAILURE([](auto it) { return it++; }(view.rbegin()));
  RD_ASSERT_NO_FATAL_FAILURE([](auto it) { return ++it; }(cview.rbegin()));

  RD_ASSERT_NE(view.begin(), view.end());
  RD_ASSERT_NE(cview.begin(), cview.end());
  RD_ASSERT_NE(view.rbegin(), view.rend());
  RD_ASSERT_NE(cview.rbegin(), cview.rend());
  RD_ASSERT_EQ(view.size(), 1u);
  RD_ASSERT_FALSE(view.empty());

  registry.emplace<char>(e0);

  RD_ASSERT_EQ(view.size(), 2u);

  view.get<char>(e0) = '1';
  std::get<0>(view.get(e1)) = '2';

  RD_ASSERT_EQ(view.get<0u>(e0), '1');
  RD_ASSERT_EQ(cview.get<0u>(e0), view.get<char>(e0));
  RD_ASSERT_EQ(view.get<char>(e1), '2');

  for(auto entity: view) {
    RD_ASSERT_TRUE(entity == e0 || entity == e1);
    RD_ASSERT_TRUE(entity != e0 || cview.get<const char>(entity) == '1');
    RD_ASSERT_TRUE(entity != e1 || std::get<const char &>(cview.get(entity)) == '2');
  }

  registry.erase<char>(e0);
  registry.erase<char>(e1);

  RD_ASSERT_EQ(view.begin(), view.end());
  RD_ASSERT_EQ(view.rbegin(), view.rend());
  RD_ASSERT_TRUE(view.empty());

  decltype(view) invalid{};

  RD_ASSERT_TRUE(view);
  RD_ASSERT_TRUE(cview);
  RD_ASSERT_FALSE(invalid);
}

RD_TEST(SingleComponentView, Constructors) {
  rendu::storage<int> storage{};

  rendu::view<rendu::get_t<int>> invalid{};
  rendu::basic_view from_storage{storage};
  rendu::basic_view from_tuple{std::forward_as_tuple(storage)};

  RD_ASSERT_FALSE(invalid);
  RD_ASSERT_TRUE(from_storage);
  RD_ASSERT_TRUE(from_tuple);

  RD_ASSERT_EQ(&from_storage.handle(), &from_tuple.handle());
}

RD_TEST(SingleComponentView, Handle) {
  rendu::registry registry;
  const auto entity = registry.create();

  auto view = registry.view<int>();
  auto &&handle = view.handle();

  RD_ASSERT_TRUE(handle.empty());
  RD_ASSERT_FALSE(handle.contains(entity));
  RD_ASSERT_EQ(&handle, &view.handle());

  registry.emplace<int>(entity);

  RD_ASSERT_FALSE(handle.empty());
  RD_ASSERT_TRUE(handle.contains(entity));
  RD_ASSERT_EQ(&handle, &view.handle());
}

RD_TEST(SingleComponentView, LazyTypeFromConstRegistry) {
  rendu::registry registry{};
  auto eview = std::as_const(registry).view<const empty_type>();
  auto cview = std::as_const(registry).view<const int>();

  const auto entity = registry.create();
  registry.emplace<empty_type>(entity);
  registry.emplace<int>(entity);

  RD_ASSERT_TRUE(cview);
  RD_ASSERT_TRUE(eview);

  RD_ASSERT_TRUE(cview.empty());
  RD_ASSERT_EQ(eview.size(), 0u);
  RD_ASSERT_FALSE(cview.contains(entity));

  RD_ASSERT_EQ(cview.begin(), cview.end());
  RD_ASSERT_EQ(eview.rbegin(), eview.rend());
  RD_ASSERT_EQ(eview.find(entity), eview.end());
  RD_ASSERT_NE(cview.front(), entity);
  RD_ASSERT_NE(eview.back(), entity);
}

RD_TEST(SingleComponentView, ElementAccess) {
  rendu::registry registry;
  auto view = registry.view<int>();
  auto cview = std::as_const(registry).view<const int>();

  const auto e0 = registry.create();
  registry.emplace<int>(e0, 42);

  const auto e1 = registry.create();
  registry.emplace<int>(e1, 3);

  for(auto i = 0u; i < view.size(); ++i) {
    RD_ASSERT_EQ(view[i], i ? e0 : e1);
    RD_ASSERT_EQ(cview[i], i ? e0 : e1);
  }

  RD_ASSERT_EQ(view[e0], 42);
  RD_ASSERT_EQ(cview[e1], 3);
}

RD_TEST(SingleComponentView, Contains) {
  rendu::registry registry;

  const auto e0 = registry.create();
  registry.emplace<int>(e0);

  const auto e1 = registry.create();
  registry.emplace<int>(e1);

  registry.destroy(e0);

  auto view = registry.view<int>();

  RD_ASSERT_FALSE(view.contains(e0));
  RD_ASSERT_TRUE(view.contains(e1));
}

RD_TEST(SingleComponentView, Empty) {
  rendu::registry registry;

  const auto e0 = registry.create();
  registry.emplace<char>(e0);
  registry.emplace<double>(e0);

  const auto e1 = registry.create();
  registry.emplace<char>(e1);

  auto view = registry.view<int>();

  RD_ASSERT_EQ(view.size(), 0u);
  RD_ASSERT_EQ(view.begin(), view.end());
  RD_ASSERT_EQ(view.rbegin(), view.rend());
}

RD_TEST(SingleComponentView, Each) {
  rendu::registry registry;
  rendu::entity entity[2]{registry.create(), registry.create()};

  auto view = registry.view<int>(rendu::exclude<double>);
  auto cview = std::as_const(registry).view<const int>();

  registry.emplace<int>(entity[0u], 0);
  registry.emplace<int>(entity[1u], 1);

  auto iterable = view.each();
  auto citerable = cview.each();

  RD_ASSERT_NE(citerable.begin(), citerable.end());
  RD_ASSERT_NO_FATAL_FAILURE(iterable.begin()->operator=(*iterable.begin()));
  RD_ASSERT_EQ(decltype(iterable.end()){}, iterable.end());

  auto it = iterable.begin();

  RD_ASSERT_EQ(it.base(), view.begin());
  RD_ASSERT_EQ((it++, ++it), iterable.end());
  RD_ASSERT_EQ(it.base(), view.end());

  view.each([expected = 1](auto rendu, int &value) mutable {
    RD_ASSERT_EQ(static_cast<int>(rendu::to_integral(rendu)), expected);
    RD_ASSERT_EQ(value, expected);
    --expected;
  });

  cview.each([expected = 1](const int &value) mutable {
    RD_ASSERT_EQ(value, expected);
    --expected;
  });

  RD_ASSERT_EQ(std::get<0>(*iterable.begin()), entity[1u]);
  RD_ASSERT_EQ(std::get<0>(*++citerable.begin()), entity[0u]);

  static_assert(std::is_same_v<decltype(std::get<1>(*iterable.begin())), int &>);
  static_assert(std::is_same_v<decltype(std::get<1>(*citerable.begin())), const int &>);

  // do not use iterable, make sure an iterable view works when created from a temporary
  for(auto [rendu, value]: view.each()) {
    RD_ASSERT_EQ(static_cast<int>(rendu::to_integral(rendu)), value);
  }
}

RD_TEST(SingleComponentView, ConstNonConstAndAllInBetween) {
  rendu::registry registry;
  auto view = registry.view<int>();
  auto cview = std::as_const(registry).view<const int>();

  RD_ASSERT_EQ(view.size(), 0u);
  RD_ASSERT_EQ(cview.size(), 0u);

  registry.emplace<int>(registry.create(), 0);

  RD_ASSERT_EQ(view.size(), 1u);
  RD_ASSERT_EQ(cview.size(), 1u);

  static_assert(std::is_same_v<decltype(view.get<0u>({})), int &>);
  static_assert(std::is_same_v<decltype(view.get<int>({})), int &>);
  static_assert(std::is_same_v<decltype(view.get({})), std::tuple<int &>>);

  static_assert(std::is_same_v<decltype(cview.get<0u>({})), const int &>);
  static_assert(std::is_same_v<decltype(cview.get<const int>({})), const int &>);
  static_assert(std::is_same_v<decltype(cview.get({})), std::tuple<const int &>>);

  static_assert(std::is_same_v<decltype(std::as_const(registry).view<int>()), decltype(cview)>);

  view.each([](auto &&i) {
    static_assert(std::is_same_v<decltype(i), int &>);
  });

  cview.each([](auto &&i) {
    static_assert(std::is_same_v<decltype(i), const int &>);
  });

  for(auto [rendu, iv]: view.each()) {
    static_assert(std::is_same_v<decltype(rendu), rendu::entity>);
    static_assert(std::is_same_v<decltype(iv), int &>);
  }

  for(auto [rendu, iv]: cview.each()) {
    static_assert(std::is_same_v<decltype(rendu), rendu::entity>);
    static_assert(std::is_same_v<decltype(iv), const int &>);
  }
}

RD_TEST(SingleComponentView, ConstNonConstAndAllInBetweenWithEmptyType) {
  rendu::registry registry;
  auto view = registry.view<empty_type>();
  auto cview = std::as_const(registry).view<const empty_type>();

  RD_ASSERT_EQ(view.size(), 0u);
  RD_ASSERT_EQ(cview.size(), 0u);

  registry.emplace<empty_type>(registry.create());

  RD_ASSERT_EQ(view.size(), 1u);
  RD_ASSERT_EQ(cview.size(), 1u);

  static_assert(std::is_same_v<decltype(view.get({})), std::tuple<>>);
  static_assert(std::is_same_v<decltype(cview.get({})), std::tuple<>>);

  static_assert(std::is_same_v<decltype(std::as_const(registry).view<empty_type>()), decltype(cview)>);

  for(auto [rendu]: view.each()) {
    static_assert(std::is_same_v<decltype(rendu), rendu::entity>);
  }

  for(auto [rendu]: cview.each()) {
    static_assert(std::is_same_v<decltype(rendu), rendu::entity>);
  }
}

RD_TEST(SingleComponentView, Find) {
  rendu::registry registry;
  auto view = registry.view<int>();

  const auto e0 = registry.create();
  registry.emplace<int>(e0);

  const auto e1 = registry.create();
  registry.emplace<int>(e1);

  const auto e2 = registry.create();
  registry.emplace<int>(e2);

  const auto e3 = registry.create();
  registry.emplace<int>(e3);

  registry.erase<int>(e1);

  RD_ASSERT_NE(view.find(e0), view.end());
  RD_ASSERT_EQ(view.find(e1), view.end());
  RD_ASSERT_NE(view.find(e2), view.end());
  RD_ASSERT_NE(view.find(e3), view.end());

  auto it = view.find(e2);

  RD_ASSERT_EQ(*it, e2);
  RD_ASSERT_EQ(*(++it), e3);
  RD_ASSERT_EQ(*(++it), e0);
  RD_ASSERT_EQ(++it, view.end());
  RD_ASSERT_EQ(++view.find(e0), view.end());

  const auto e4 = registry.create();
  registry.destroy(e4);
  const auto e5 = registry.create();
  registry.emplace<int>(e5);

  RD_ASSERT_NE(view.find(e5), view.end());
  RD_ASSERT_EQ(view.find(e4), view.end());
}

RD_TEST(SingleComponentView, EmptyTypes) {
  rendu::registry registry;
  rendu::entity entities[2u];

  registry.create(std::begin(entities), std::end(entities));
  registry.emplace<int>(entities[0u], 0);
  registry.emplace<empty_type>(entities[0u]);
  registry.emplace<char>(entities[1u], 'c');

  registry.view<empty_type>().each([&](const auto rendu) {
    RD_ASSERT_EQ(entities[0u], rendu);
  });

  registry.view<empty_type>().each([check = true]() mutable {
    RD_ASSERT_TRUE(check);
    check = false;
  });

  for(auto [rendu]: registry.view<empty_type>().each()) {
    static_assert(std::is_same_v<decltype(rendu), rendu::entity>);
    RD_ASSERT_EQ(entities[0u], rendu);
  }

  registry.view<int>().each([&](const auto rendu, int) {
    RD_ASSERT_EQ(entities[0u], rendu);
  });

  registry.view<int>().each([check = true](int) mutable {
    RD_ASSERT_TRUE(check);
    check = false;
  });

  for(auto [rendu, iv]: registry.view<int>().each()) {
    static_assert(std::is_same_v<decltype(rendu), rendu::entity>);
    static_assert(std::is_same_v<decltype(iv), int &>);
    RD_ASSERT_EQ(entities[0u], rendu);
  }
}

RD_TEST(SingleComponentView, FrontBack) {
  rendu::registry registry;
  auto view = registry.view<const int>();

  RD_ASSERT_EQ(view.front(), static_cast<rendu::entity>(rendu::null));
  RD_ASSERT_EQ(view.back(), static_cast<rendu::entity>(rendu::null));

  const auto e0 = registry.create();
  registry.emplace<int>(e0);

  const auto e1 = registry.create();
  registry.emplace<int>(e1);

  RD_ASSERT_EQ(view.front(), e1);
  RD_ASSERT_EQ(view.back(), e0);
}

RD_TEST(SingleComponentView, DeductionGuide) {
  rendu::registry registry;
  rendu::storage_type_t<int> istorage;
  rendu::storage_type_t<stable_type> sstorage;

  static_assert(std::is_same_v<rendu::basic_view<rendu::get_t<rendu::storage_type_t<int>>, rendu::exclude_t<>>, decltype(rendu::basic_view{istorage})>);
  static_assert(std::is_same_v<rendu::basic_view<rendu::get_t<const rendu::storage_type_t<int>>, rendu::exclude_t<>>, decltype(rendu::basic_view{std::as_const(istorage)})>);
  static_assert(std::is_same_v<rendu::basic_view<rendu::get_t<rendu::storage_type_t<stable_type>>, rendu::exclude_t<>>, decltype(rendu::basic_view{sstorage})>);

  static_assert(std::is_same_v<rendu::basic_view<rendu::get_t<rendu::storage_type_t<int>>, rendu::exclude_t<>>, decltype(rendu::basic_view{std::forward_as_tuple(istorage), std::make_tuple()})>);
  static_assert(std::is_same_v<rendu::basic_view<rendu::get_t<const rendu::storage_type_t<int>>, rendu::exclude_t<>>, decltype(rendu::basic_view{std::forward_as_tuple(std::as_const(istorage))})>);
  static_assert(std::is_same_v<rendu::basic_view<rendu::get_t<rendu::storage_type_t<stable_type>>, rendu::exclude_t<>>, decltype(rendu::basic_view{std::forward_as_tuple(sstorage)})>);
}

RD_TEST(SingleComponentView, IterableViewAlgorithmCompatibility) {
  rendu::registry registry;
  const auto entity = registry.create();

  registry.emplace<int>(entity);

  const auto view = registry.view<int>();
  const auto iterable = view.each();
  const auto it = std::find_if(iterable.begin(), iterable.end(), [entity](auto args) { return std::get<0>(args) == entity; });

  RD_ASSERT_EQ(std::get<0>(*it), entity);
}

RD_TEST(SingleComponentView, StableType) {
  rendu::registry registry;
  auto view = registry.view<stable_type>();

  const auto entity = registry.create();
  const auto other = registry.create();

  registry.emplace<stable_type>(entity);
  registry.emplace<stable_type>(other);
  registry.destroy(entity);

  RD_ASSERT_EQ(view.size_hint(), 2u);
  RD_ASSERT_FALSE(view.contains(entity));
  RD_ASSERT_TRUE(view.contains(other));

  RD_ASSERT_EQ(view.front(), other);
  RD_ASSERT_EQ(view.back(), other);

  RD_ASSERT_EQ(*view.begin(), other);
  RD_ASSERT_EQ(++view.begin(), view.end());

  view.each([other](const auto rendu, stable_type) {
    RD_ASSERT_EQ(other, rendu);
  });

  view.each([check = true](stable_type) mutable {
    RD_ASSERT_TRUE(check);
    check = false;
  });

  for(auto [rendu, st]: view.each()) {
    static_assert(std::is_same_v<decltype(rendu), rendu::entity>);
    static_assert(std::is_same_v<decltype(st), stable_type &>);
    RD_ASSERT_EQ(other, rendu);
  }

  registry.compact();

  RD_ASSERT_EQ(view.size_hint(), 1u);
}

RD_TEST(SingleComponentView, Storage) {
  rendu::registry registry;
  const auto entity = registry.create();
  const auto view = registry.view<int>();
  const auto cview = registry.view<const char>();

  static_assert(std::is_same_v<decltype(view.storage()), rendu::storage_type_t<int> &>);
  static_assert(std::is_same_v<decltype(view.storage<0u>()), rendu::storage_type_t<int> &>);
  static_assert(std::is_same_v<decltype(view.storage<int>()), rendu::storage_type_t<int> &>);
  static_assert(std::is_same_v<decltype(view.storage<const int>()), rendu::storage_type_t<int> &>);
  static_assert(std::is_same_v<decltype(cview.storage()), const rendu::storage_type_t<char> &>);
  static_assert(std::is_same_v<decltype(cview.storage<0u>()), const rendu::storage_type_t<char> &>);
  static_assert(std::is_same_v<decltype(cview.storage<char>()), const rendu::storage_type_t<char> &>);
  static_assert(std::is_same_v<decltype(cview.storage<const char>()), const rendu::storage_type_t<char> &>);

  RD_ASSERT_EQ(view.size(), 0u);
  RD_ASSERT_EQ(cview.size(), 0u);

  view.storage().emplace(entity);
  registry.emplace<char>(entity);

  RD_ASSERT_EQ(view.size(), 1u);
  RD_ASSERT_EQ(cview.size(), 1u);
  RD_ASSERT_TRUE(view.storage<int>().contains(entity));
  RD_ASSERT_TRUE(cview.storage<0u>().contains(entity));
  RD_ASSERT_TRUE((registry.all_of<int, char>(entity)));

  view.storage().erase(entity);

  RD_ASSERT_EQ(view.size(), 0u);
  RD_ASSERT_EQ(cview.size(), 1u);
  RD_ASSERT_FALSE(view.storage<0u>().contains(entity));
  RD_ASSERT_TRUE(cview.storage<const char>().contains(entity));
  RD_ASSERT_FALSE((registry.all_of<int, char>(entity)));
}

RD_TEST(MultiComponentView, Functionalities) {
  rendu::registry registry;
  auto view = registry.view<int, char>();
  auto cview = std::as_const(registry).view<const int, const char>();

  const auto e0 = registry.create();
  registry.emplace<char>(e0, '1');

  const auto e1 = registry.create();
  registry.emplace<int>(e1, 42);
  registry.emplace<char>(e1, '2');

  RD_ASSERT_EQ(*view.begin(), e1);
  RD_ASSERT_EQ(*cview.begin(), e1);
  RD_ASSERT_EQ(++view.begin(), (view.end()));
  RD_ASSERT_EQ(++cview.begin(), (cview.end()));

  RD_ASSERT_NO_FATAL_FAILURE((view.begin()++));
  RD_ASSERT_NO_FATAL_FAILURE((++cview.begin()));

  RD_ASSERT_NE(view.begin(), view.end());
  RD_ASSERT_NE(cview.begin(), cview.end());
  RD_ASSERT_EQ(view.size_hint(), 1u);

  for(auto entity: view) {
    RD_ASSERT_EQ(std::get<0>(cview.get<const int, const char>(entity)), 42);
    RD_ASSERT_EQ(std::get<0>(cview.get<0u, 1u>(entity)), 42);

    RD_ASSERT_EQ(std::get<1>(view.get<int, char>(entity)), '2');
    RD_ASSERT_EQ(std::get<1>(view.get<0u, 1u>(entity)), '2');

    RD_ASSERT_EQ(cview.get<const char>(entity), '2');
    RD_ASSERT_EQ(cview.get<1u>(entity), '2');
  }

  decltype(view) invalid{};

  RD_ASSERT_TRUE(view);
  RD_ASSERT_TRUE(cview);
  RD_ASSERT_FALSE(invalid);
}

RD_TEST(MultiComponentView, Constructors) {
  rendu::storage<int> storage{};

  rendu::view<rendu::get_t<int, int>> invalid{};
  rendu::basic_view from_storage{storage, storage};
  rendu::basic_view from_tuple{std::forward_as_tuple(storage, storage)};

  RD_ASSERT_FALSE(invalid);
  RD_ASSERT_TRUE(from_storage);
  RD_ASSERT_TRUE(from_tuple);

  RD_ASSERT_EQ(&from_storage.handle(), &from_tuple.handle());
}

RD_TEST(MultiComponentView, Handle) {
  rendu::registry registry;
  const auto entity = registry.create();

  auto view = registry.view<int, char>();
  auto &&handle = view.handle();

  RD_ASSERT_TRUE(handle.empty());
  RD_ASSERT_FALSE(handle.contains(entity));
  RD_ASSERT_EQ(&handle, &view.handle());

  registry.emplace<int>(entity);

  RD_ASSERT_FALSE(handle.empty());
  RD_ASSERT_TRUE(handle.contains(entity));
  RD_ASSERT_EQ(&handle, &view.handle());

  view = view.refresh();
  auto &&other = view.handle();

  RD_ASSERT_TRUE(other.empty());
  RD_ASSERT_FALSE(other.contains(entity));
  RD_ASSERT_EQ(&other, &view.handle());
  RD_ASSERT_NE(&handle, &other);

  view = view.use<int>();

  RD_ASSERT_NE(&other, &view.handle());
  RD_ASSERT_EQ(&handle, &view.handle());
}

RD_TEST(MultiComponentView, LazyTypesFromConstRegistry) {
  rendu::registry registry{};
  auto view = std::as_const(registry).view<const empty_type, const int>();

  const auto entity = registry.create();
  registry.emplace<empty_type>(entity);
  registry.emplace<int>(entity);

  RD_ASSERT_TRUE(view);

  RD_ASSERT_EQ(view.size_hint(), 0u);
  RD_ASSERT_FALSE(view.contains(entity));

  RD_ASSERT_EQ(view.begin(), view.end());
  RD_ASSERT_EQ(view.find(entity), view.end());
  RD_ASSERT_NE(view.front(), entity);
  RD_ASSERT_NE(view.back(), entity);
}

RD_TEST(MultiComponentView, LazyExcludedTypeFromConstRegistry) {
  rendu::registry registry;

  auto entity = registry.create();
  registry.emplace<int>(entity);

  auto view = std::as_const(registry).view<const int>(rendu::exclude<char>);

  RD_ASSERT_TRUE(view);

  RD_ASSERT_EQ(view.size_hint(), 1u);
  RD_ASSERT_TRUE(view.contains(entity));

  RD_ASSERT_NE(view.begin(), view.end());
  RD_ASSERT_NE(view.find(entity), view.end());
  RD_ASSERT_EQ(view.front(), entity);
  RD_ASSERT_EQ(view.back(), entity);
}

RD_TEST(MultiComponentView, Iterator) {
  rendu::registry registry;
  const rendu::entity entity[2]{registry.create(), registry.create()};

  registry.insert<int>(std::begin(entity), std::end(entity));
  registry.insert<char>(std::begin(entity), std::end(entity));

  const auto view = registry.view<int, char>();
  using iterator = typename decltype(view)::iterator;

  iterator end{view.begin()};
  iterator begin{};
  begin = view.end();
  std::swap(begin, end);

  RD_ASSERT_EQ(begin, view.begin());
  RD_ASSERT_EQ(end, view.end());
  RD_ASSERT_NE(begin, end);

  RD_ASSERT_EQ(*begin, entity[1u]);
  RD_ASSERT_EQ(*begin.operator->(), entity[1u]);
  RD_ASSERT_EQ(begin++, view.begin());

  RD_ASSERT_EQ(*begin, entity[0u]);
  RD_ASSERT_EQ(*begin.operator->(), entity[0u]);
  RD_ASSERT_EQ(++begin, view.end());
}

RD_TEST(MultiComponentView, ElementAccess) {
  rendu::registry registry;
  auto view = registry.view<int, char>();
  auto cview = std::as_const(registry).view<const int, const char>();

  const auto e0 = registry.create();
  registry.emplace<int>(e0, 42);
  registry.emplace<char>(e0, '0');

  const auto e1 = registry.create();
  registry.emplace<int>(e1, 3);
  registry.emplace<char>(e1, '1');

  RD_ASSERT_EQ(view[e0], std::make_tuple(42, '0'));
  RD_ASSERT_EQ(cview[e1], std::make_tuple(3, '1'));
}

RD_TEST(MultiComponentView, Contains) {
  rendu::registry registry;

  const auto e0 = registry.create();
  registry.emplace<int>(e0);
  registry.emplace<char>(e0);

  const auto e1 = registry.create();
  registry.emplace<int>(e1);
  registry.emplace<char>(e1);

  registry.destroy(e0);

  auto view = registry.view<int, char>();

  RD_ASSERT_FALSE(view.contains(e0));
  RD_ASSERT_TRUE(view.contains(e1));
}

RD_TEST(MultiComponentView, SizeHint) {
  rendu::registry registry;

  const auto e0 = registry.create();
  registry.emplace<double>(e0);
  registry.emplace<int>(e0);
  registry.emplace<float>(e0);

  const auto e1 = registry.create();
  registry.emplace<char>(e1);
  registry.emplace<float>(e1);

  auto view = registry.view<char, int, float>();

  RD_ASSERT_EQ(view.size_hint(), 1u);
  RD_ASSERT_EQ(view.begin(), view.end());
}

RD_TEST(MultiComponentView, Each) {
  rendu::registry registry;
  rendu::entity entity[2]{registry.create(), registry.create()};

  auto view = registry.view<int, char>(rendu::exclude<double>);
  auto cview = std::as_const(registry).view<const int, const char>();

  registry.emplace<int>(entity[0u], 0);
  registry.emplace<char>(entity[0u], static_cast<char>(0));

  registry.emplace<int>(entity[1u], 1);
  registry.emplace<char>(entity[1u], static_cast<char>(1));

  auto iterable = view.each();
  auto citerable = cview.each();

  RD_ASSERT_NE(citerable.begin(), citerable.end());
  RD_ASSERT_NO_FATAL_FAILURE(iterable.begin()->operator=(*iterable.begin()));
  RD_ASSERT_EQ(decltype(iterable.end()){}, iterable.end());

  auto it = iterable.begin();

  RD_ASSERT_EQ(it.base(), view.begin());
  RD_ASSERT_EQ((it++, ++it), iterable.end());
  RD_ASSERT_EQ(it.base(), view.end());

  view.each([expected = 1](auto rendu, int &ivalue, char &cvalue) mutable {
    RD_ASSERT_EQ(static_cast<int>(rendu::to_integral(rendu)), expected);
    RD_ASSERT_EQ(ivalue, expected);
    RD_ASSERT_EQ(cvalue, expected);
    --expected;
  });

  cview.each([expected = 1](const int &ivalue, const char &cvalue) mutable {
    RD_ASSERT_EQ(ivalue, expected);
    RD_ASSERT_EQ(cvalue, expected);
    --expected;
  });

  RD_ASSERT_EQ(std::get<0>(*iterable.begin()), entity[1u]);
  RD_ASSERT_EQ(std::get<0>(*++citerable.begin()), entity[0u]);

  static_assert(std::is_same_v<decltype(std::get<1>(*iterable.begin())), int &>);
  static_assert(std::is_same_v<decltype(std::get<2>(*citerable.begin())), const char &>);

  // do not use iterable, make sure an iterable view works when created from a temporary
  for(auto [rendu, ivalue, cvalue]: registry.view<int, char>().each()) {
    RD_ASSERT_EQ(static_cast<int>(rendu::to_integral(rendu)), ivalue);
    RD_ASSERT_EQ(static_cast<char>(rendu::to_integral(rendu)), cvalue);
  }
}

RD_TEST(MultiComponentView, EachWithSuggestedType) {
  rendu::registry registry;

  for(auto i = 0; i < 3; ++i) {
    const auto entity = registry.create();
    registry.emplace<int>(entity, i);
    registry.emplace<char>(entity);
  }

  // makes char a better candidate during iterations
  const auto entity = registry.create();
  registry.emplace<int>(entity, 99);

  registry.view<int, char>().use<int>().each([value = 2](const auto curr, const auto) mutable {
    RD_ASSERT_EQ(curr, value--);
  });

  registry.sort<int>([](const auto lhs, const auto rhs) {
    return lhs < rhs;
  });

  registry.view<int, char>().use<0u>().each([value = 0](const auto curr, const auto) mutable {
    RD_ASSERT_EQ(curr, value++);
  });

  registry.sort<int>([](const auto lhs, const auto rhs) {
    return lhs > rhs;
  });

  auto value = registry.view<int, char>().size_hint();

  for(auto &&curr: registry.view<int, char>().each()) {
    RD_ASSERT_EQ(std::get<1>(curr), static_cast<int>(--value));
  }

  registry.sort<int>([](const auto lhs, const auto rhs) {
    return lhs < rhs;
  });

  value = {};

  for(auto &&curr: registry.view<int, char>().use<int>().each()) {
    RD_ASSERT_EQ(std::get<1>(curr), static_cast<int>(value++));
  }
}

RD_TEST(MultiComponentView, EachWithHoles) {
  rendu::registry registry;

  const auto e0 = registry.create();
  const auto e1 = registry.create();
  const auto e2 = registry.create();

  registry.emplace<char>(e0, '0');
  registry.emplace<char>(e1, '1');

  registry.emplace<int>(e0, 0);
  registry.emplace<int>(e2, 2);

  auto view = registry.view<char, int>();

  view.each([e0](auto entity, const char &c, const int &i) {
    RD_ASSERT_EQ(entity, e0);
    RD_ASSERT_EQ(c, '0');
    RD_ASSERT_EQ(i, 0);
  });

  for(auto &&curr: view.each()) {
    RD_ASSERT_EQ(std::get<0>(curr), e0);
    RD_ASSERT_EQ(std::get<1>(curr), '0');
    RD_ASSERT_EQ(std::get<2>(curr), 0);
  }
}

RD_TEST(MultiComponentView, ConstNonConstAndAllInBetween) {
  rendu::registry registry;
  auto view = registry.view<int, empty_type, const char>();

  RD_ASSERT_EQ(view.size_hint(), 0u);

  const auto entity = registry.create();
  registry.emplace<int>(entity, 0);
  registry.emplace<empty_type>(entity);
  registry.emplace<char>(entity, 'c');

  RD_ASSERT_EQ(view.size_hint(), 1u);

  static_assert(std::is_same_v<decltype(view.get<0u>({})), int &>);
  static_assert(std::is_same_v<decltype(view.get<2u>({})), const char &>);
  static_assert(std::is_same_v<decltype(view.get<0u, 2u>({})), std::tuple<int &, const char &>>);

  static_assert(std::is_same_v<decltype(view.get<int>({})), int &>);
  static_assert(std::is_same_v<decltype(view.get<const char>({})), const char &>);
  static_assert(std::is_same_v<decltype(view.get<int, const char>({})), std::tuple<int &, const char &>>);

  static_assert(std::is_same_v<decltype(view.get({})), std::tuple<int &, const char &>>);

  static_assert(std::is_same_v<decltype(std::as_const(registry).view<char, int>()), decltype(std::as_const(registry).view<const char, const int>())>);
  static_assert(std::is_same_v<decltype(std::as_const(registry).view<char, const int>()), decltype(std::as_const(registry).view<const char, const int>())>);
  static_assert(std::is_same_v<decltype(std::as_const(registry).view<const char, int>()), decltype(std::as_const(registry).view<const char, const int>())>);

  view.each([](auto &&i, auto &&c) {
    static_assert(std::is_same_v<decltype(i), int &>);
    static_assert(std::is_same_v<decltype(c), const char &>);
  });

  for(auto [rendu, iv, cv]: view.each()) {
    static_assert(std::is_same_v<decltype(rendu), rendu::entity>);
    static_assert(std::is_same_v<decltype(iv), int &>);
    static_assert(std::is_same_v<decltype(cv), const char &>);
  }
}

RD_TEST(MultiComponentView, Find) {
  rendu::registry registry;
  auto view = registry.view<int, const char>();

  const auto e0 = registry.create();
  registry.emplace<int>(e0);
  registry.emplace<char>(e0);

  const auto e1 = registry.create();
  registry.emplace<int>(e1);
  registry.emplace<char>(e1);

  const auto e2 = registry.create();
  registry.emplace<int>(e2);
  registry.emplace<char>(e2);

  const auto e3 = registry.create();
  registry.emplace<int>(e3);
  registry.emplace<char>(e3);

  registry.erase<int>(e1);

  RD_ASSERT_NE(view.find(e0), view.end());
  RD_ASSERT_EQ(view.find(e1), view.end());
  RD_ASSERT_NE(view.find(e2), view.end());
  RD_ASSERT_NE(view.find(e3), view.end());

  auto it = view.find(e2);

  RD_ASSERT_EQ(*it, e2);
  RD_ASSERT_EQ(*(++it), e3);
  RD_ASSERT_EQ(*(++it), e0);
  RD_ASSERT_EQ(++it, view.end());
  RD_ASSERT_EQ(++view.find(e0), view.end());

  const auto e4 = registry.create();
  registry.destroy(e4);
  const auto e5 = registry.create();
  registry.emplace<int>(e5);
  registry.emplace<char>(e5);

  RD_ASSERT_NE(view.find(e5), view.end());
  RD_ASSERT_EQ(view.find(e4), view.end());
}

RD_TEST(MultiComponentView, ExcludedComponents) {
  rendu::registry registry;

  const auto e0 = registry.create();
  registry.emplace<int>(e0, 0);

  const auto e1 = registry.create();
  registry.emplace<int>(e1, 1);
  registry.emplace<char>(e1);

  const auto e2 = registry.create();
  registry.emplace<int>(e2, 2);

  const auto e3 = registry.create();
  registry.emplace<int>(e3, 3);
  registry.emplace<char>(e3);

  const auto view = std::as_const(registry).view<const int>(rendu::exclude<char>);

  for(const auto entity: view) {
    RD_ASSERT_TRUE(entity == e0 || entity == e2);

    if(entity == e0) {
      RD_ASSERT_EQ(view.get<const int>(e0), 0);
      RD_ASSERT_EQ(view.get<0u>(e0), 0);
    } else if(entity == e2) {
      RD_ASSERT_EQ(std::get<0>(view.get(e2)), 2);
    }
  }

  registry.emplace<char>(e0);
  registry.emplace<char>(e2);
  registry.erase<char>(e1);
  registry.erase<char>(e3);

  for(const auto entity: view) {
    RD_ASSERT_TRUE(entity == e1 || entity == e3);

    if(entity == e1) {
      RD_ASSERT_EQ(std::get<0>(view.get(e1)), 1);
    } else if(entity == e3) {
      RD_ASSERT_EQ(view.get<const int>(e3), 3);
      RD_ASSERT_EQ(view.get<0u>(e3), 3);
    }
  }
}

RD_TEST(MultiComponentView, EmptyTypes) {
  rendu::registry registry;

  const auto entity = registry.create();
  registry.emplace<int>(entity);
  registry.emplace<char>(entity);
  registry.emplace<empty_type>(entity);

  const auto other = registry.create();
  registry.emplace<int>(other);
  registry.emplace<char>(other);
  registry.emplace<double>(other);
  registry.emplace<empty_type>(other);

  const auto ignored = registry.create();
  registry.emplace<int>(ignored);
  registry.emplace<char>(ignored);

  registry.view<int, char, empty_type>(rendu::exclude<double>).each([entity](const auto rendu, int, char) {
    RD_ASSERT_EQ(entity, rendu);
  });

  for(auto [rendu, iv, cv]: registry.view<int, char, empty_type>(rendu::exclude<double>).each()) {
    static_assert(std::is_same_v<decltype(rendu), rendu::entity>);
    static_assert(std::is_same_v<decltype(iv), int &>);
    static_assert(std::is_same_v<decltype(cv), char &>);
    RD_ASSERT_EQ(entity, rendu);
  }

  registry.view<int, empty_type, char>(rendu::exclude<double>).each([check = true](int, char) mutable {
    RD_ASSERT_TRUE(check);
    check = false;
  });

  for(auto [rendu, iv, cv]: registry.view<int, empty_type, char>(rendu::exclude<double>).each()) {
    static_assert(std::is_same_v<decltype(rendu), rendu::entity>);
    static_assert(std::is_same_v<decltype(iv), int &>);
    static_assert(std::is_same_v<decltype(cv), char &>);
    RD_ASSERT_EQ(entity, rendu);
  }

  registry.view<empty_type, int, char>(rendu::exclude<double>).each([entity](const auto rendu, int, char) {
    RD_ASSERT_EQ(entity, rendu);
  });

  for(auto [rendu, iv, cv]: registry.view<empty_type, int, char>(rendu::exclude<double>).each()) {
    static_assert(std::is_same_v<decltype(rendu), rendu::entity>);
    static_assert(std::is_same_v<decltype(iv), int &>);
    static_assert(std::is_same_v<decltype(cv), char &>);
    RD_ASSERT_EQ(entity, rendu);
  }

  registry.view<empty_type, int, char>(rendu::exclude<double>).use<empty_type>().each([entity](const auto rendu, int, char) {
    RD_ASSERT_EQ(entity, rendu);
  });

  for(auto [rendu, iv, cv]: registry.view<empty_type, int, char>(rendu::exclude<double>).use<0u>().each()) {
    static_assert(std::is_same_v<decltype(rendu), rendu::entity>);
    static_assert(std::is_same_v<decltype(iv), int &>);
    static_assert(std::is_same_v<decltype(cv), char &>);
    RD_ASSERT_EQ(entity, rendu);
  }

  registry.view<int, empty_type, char>(rendu::exclude<double>).use<1u>().each([check = true](int, char) mutable {
    RD_ASSERT_TRUE(check);
    check = false;
  });

  for(auto [rendu, iv, cv]: registry.view<int, empty_type, char>(rendu::exclude<double>).use<empty_type>().each()) {
    static_assert(std::is_same_v<decltype(rendu), rendu::entity>);
    static_assert(std::is_same_v<decltype(iv), int &>);
    static_assert(std::is_same_v<decltype(cv), char &>);
    RD_ASSERT_EQ(entity, rendu);
  }
}

RD_TEST(MultiComponentView, FrontBack) {
  rendu::registry registry;
  auto view = registry.view<const int, const char>();

  RD_ASSERT_EQ(view.front(), static_cast<rendu::entity>(rendu::null));
  RD_ASSERT_EQ(view.back(), static_cast<rendu::entity>(rendu::null));

  const auto e0 = registry.create();
  registry.emplace<int>(e0);
  registry.emplace<char>(e0);

  const auto e1 = registry.create();
  registry.emplace<int>(e1);
  registry.emplace<char>(e1);

  const auto entity = registry.create();
  registry.emplace<char>(entity);

  RD_ASSERT_EQ(view.front(), e1);
  RD_ASSERT_EQ(view.back(), e0);
}

RD_TEST(MultiComponentView, ExtendedGet) {
  using type = decltype(std::declval<rendu::registry>().view<int, empty_type, char>().get({}));
  static_assert(std::tuple_size_v<type> == 2u);
  static_assert(std::is_same_v<std::tuple_element_t<0, type>, int &>);
  static_assert(std::is_same_v<std::tuple_element_t<1, type>, char &>);
}

RD_TEST(MultiComponentView, DeductionGuide) {
  rendu::registry registry;
  rendu::storage_type_t<int> istorage;
  rendu::storage_type_t<double> dstorage;
  rendu::storage_type_t<stable_type> sstorage;

  static_assert(std::is_same_v<rendu::basic_view<rendu::get_t<rendu::storage_type_t<int>, rendu::storage_type_t<double>>, rendu::exclude_t<>>, decltype(rendu::basic_view{istorage, dstorage})>);
  static_assert(std::is_same_v<rendu::basic_view<rendu::get_t<const rendu::storage_type_t<int>, rendu::storage_type_t<double>>, rendu::exclude_t<>>, decltype(rendu::basic_view{std::as_const(istorage), dstorage})>);
  static_assert(std::is_same_v<rendu::basic_view<rendu::get_t<rendu::storage_type_t<int>, const rendu::storage_type_t<double>>, rendu::exclude_t<>>, decltype(rendu::basic_view{istorage, std::as_const(dstorage)})>);
  static_assert(std::is_same_v<rendu::basic_view<rendu::get_t<const rendu::storage_type_t<int>, const rendu::storage_type_t<double>>, rendu::exclude_t<>>, decltype(rendu::basic_view{std::as_const(istorage), std::as_const(dstorage)})>);
  static_assert(std::is_same_v<rendu::basic_view<rendu::get_t<rendu::storage_type_t<int>, rendu::storage_type_t<stable_type>>, rendu::exclude_t<>>, decltype(rendu::basic_view{istorage, sstorage})>);

  static_assert(std::is_same_v<rendu::basic_view<rendu::get_t<rendu::storage_type_t<int>, rendu::storage_type_t<double>>, rendu::exclude_t<>>, decltype(rendu::basic_view{std::forward_as_tuple(istorage, dstorage), std::make_tuple()})>);
  static_assert(std::is_same_v<rendu::basic_view<rendu::get_t<const rendu::storage_type_t<int>, rendu::storage_type_t<double>>, rendu::exclude_t<>>, decltype(rendu::basic_view{std::forward_as_tuple(std::as_const(istorage), dstorage)})>);
  static_assert(std::is_same_v<rendu::basic_view<rendu::get_t<rendu::storage_type_t<int>, const rendu::storage_type_t<double>>, rendu::exclude_t<>>, decltype(rendu::basic_view{std::forward_as_tuple(istorage, std::as_const(dstorage))})>);
  static_assert(std::is_same_v<rendu::basic_view<rendu::get_t<const rendu::storage_type_t<int>, const rendu::storage_type_t<double>>, rendu::exclude_t<>>, decltype(rendu::basic_view{std::forward_as_tuple(std::as_const(istorage), std::as_const(dstorage))})>);
  static_assert(std::is_same_v<rendu::basic_view<rendu::get_t<rendu::storage_type_t<int>, rendu::storage_type_t<stable_type>>, rendu::exclude_t<>>, decltype(rendu::basic_view{std::forward_as_tuple(istorage, sstorage)})>);

  static_assert(std::is_same_v<rendu::basic_view<rendu::get_t<rendu::storage_type_t<int>>, rendu::exclude_t<rendu::storage_type_t<double>>>, decltype(rendu::basic_view{std::forward_as_tuple(istorage), std::forward_as_tuple(dstorage)})>);
  static_assert(std::is_same_v<rendu::basic_view<rendu::get_t<const rendu::storage_type_t<int>>, rendu::exclude_t<rendu::storage_type_t<double>>>, decltype(rendu::basic_view{std::forward_as_tuple(std::as_const(istorage)), std::forward_as_tuple(dstorage)})>);
  static_assert(std::is_same_v<rendu::basic_view<rendu::get_t<rendu::storage_type_t<int>>, rendu::exclude_t<const rendu::storage_type_t<double>>>, decltype(rendu::basic_view{std::forward_as_tuple(istorage), std::forward_as_tuple(std::as_const(dstorage))})>);
  static_assert(std::is_same_v<rendu::basic_view<rendu::get_t<const rendu::storage_type_t<int>>, rendu::exclude_t<const rendu::storage_type_t<double>>>, decltype(rendu::basic_view{std::forward_as_tuple(std::as_const(istorage)), std::forward_as_tuple(std::as_const(dstorage))})>);
  static_assert(std::is_same_v<rendu::basic_view<rendu::get_t<rendu::storage_type_t<int>>, rendu::exclude_t<rendu::storage_type_t<stable_type>>>, decltype(rendu::basic_view{std::forward_as_tuple(istorage), std::forward_as_tuple(sstorage)})>);
}

RD_TEST(MultiComponentView, IterableViewAlgorithmCompatibility) {
  rendu::registry registry;
  const auto entity = registry.create();

  registry.emplace<int>(entity);
  registry.emplace<char>(entity);

  const auto view = registry.view<int, char>();
  const auto iterable = view.each();
  const auto it = std::find_if(iterable.begin(), iterable.end(), [entity](auto args) { return std::get<0>(args) == entity; });

  RD_ASSERT_EQ(std::get<0>(*it), entity);
}

RD_TEST(MultiComponentView, StableType) {
  rendu::registry registry;
  auto view = registry.view<int, stable_type>();

  const auto entity = registry.create();
  const auto other = registry.create();

  registry.emplace<int>(entity);
  registry.emplace<int>(other);
  registry.emplace<stable_type>(entity);
  registry.emplace<stable_type>(other);
  registry.destroy(entity);

  RD_ASSERT_EQ(view.size_hint(), 1u);

  view = view.use<stable_type>();

  RD_ASSERT_EQ(view.size_hint(), 2u);
  RD_ASSERT_FALSE(view.contains(entity));
  RD_ASSERT_TRUE(view.contains(other));

  RD_ASSERT_EQ(view.front(), other);
  RD_ASSERT_EQ(view.back(), other);

  RD_ASSERT_EQ(*view.begin(), other);
  RD_ASSERT_EQ(++view.begin(), view.end());

  view.each([other](const auto rendu, int, stable_type) {
    RD_ASSERT_EQ(other, rendu);
  });

  view.each([check = true](int, stable_type) mutable {
    RD_ASSERT_TRUE(check);
    check = false;
  });

  for(auto [rendu, iv, st]: view.each()) {
    static_assert(std::is_same_v<decltype(rendu), rendu::entity>);
    static_assert(std::is_same_v<decltype(iv), int &>);
    static_assert(std::is_same_v<decltype(st), stable_type &>);
    RD_ASSERT_EQ(other, rendu);
  }

  registry.compact();

  RD_ASSERT_EQ(view.size_hint(), 1u);
}

RD_TEST(MultiComponentView, StableTypeWithExcludedComponent) {
  rendu::registry registry;
  auto view = registry.view<stable_type>(rendu::exclude<int>).use<stable_type>();

  const auto entity = registry.create();
  const auto other = registry.create();

  registry.emplace<stable_type>(entity, 0);
  registry.emplace<stable_type>(other, 42);
  registry.emplace<int>(entity);

  RD_ASSERT_EQ(view.size_hint(), 2u);
  RD_ASSERT_FALSE(view.contains(entity));
  RD_ASSERT_TRUE(view.contains(other));

  registry.destroy(entity);

  RD_ASSERT_EQ(view.size_hint(), 2u);
  RD_ASSERT_FALSE(view.contains(entity));
  RD_ASSERT_TRUE(view.contains(other));

  for(auto rendu: view) {
    constexpr rendu::entity tombstone = rendu::tombstone;
    RD_ASSERT_NE(rendu, tombstone);
    RD_ASSERT_EQ(rendu, other);
  }

  for(auto [rendu, comp]: view.each()) {
    constexpr rendu::entity tombstone = rendu::tombstone;
    RD_ASSERT_NE(rendu, tombstone);
    RD_ASSERT_EQ(rendu, other);
    RD_ASSERT_EQ(comp.value, 42);
  }

  view.each([other](const auto rendu, auto &&...) {
    constexpr rendu::entity tombstone = rendu::tombstone;
    RD_ASSERT_NE(rendu, tombstone);
    RD_ASSERT_EQ(rendu, other);
  });
}

RD_TEST(MultiComponentView, SameComponrenduypes) {
  rendu::registry registry;
  rendu::storage_type_t<int> storage;
  rendu::storage_type_t<int> other;
  rendu::basic_view view{storage, other};

  storage.bind(rendu::forward_as_any(registry));
  other.bind(rendu::forward_as_any(registry));

  const rendu::entity e0{42u};
  const rendu::entity e1{3u};

  storage.emplace(e0, 7);
  other.emplace(e0, 9);
  other.emplace(e1, 1);

  RD_ASSERT_TRUE(view.contains(e0));
  RD_ASSERT_FALSE(view.contains(e1));

  RD_ASSERT_EQ((view.get<0u, 1u>(e0)), (std::make_tuple(7, 9)));
  RD_ASSERT_EQ(view.get<1u>(e0), 9);

  for(auto rendu: view) {
    RD_ASSERT_EQ(rendu, e0);
  }

  view.each([&](auto rendu, auto &&first, auto &&second) {
    RD_ASSERT_EQ(rendu, e0);
    RD_ASSERT_EQ(first, 7);
    RD_ASSERT_EQ(second, 9);
  });

  for(auto [rendu, first, second]: view.each()) {
    RD_ASSERT_EQ(rendu, e0);
    RD_ASSERT_EQ(first, 7);
    RD_ASSERT_EQ(second, 9);
  }

  RD_ASSERT_EQ(&view.handle(), &storage);
  RD_ASSERT_EQ(&view.use<1u>().handle(), &other);
}

RD_TEST(View, Pipe) {
  rendu::registry registry;
  const auto entity = registry.create();
  const auto other = registry.create();

  registry.emplace<int>(entity);
  registry.emplace<char>(entity);
  registry.emplace<double>(entity);
  registry.emplace<empty_type>(entity);

  registry.emplace<int>(other);
  registry.emplace<char>(other);
  registry.emplace<stable_type>(other);

  const auto view1 = registry.view<int>(rendu::exclude<const double>);
  const auto view2 = registry.view<const char>(rendu::exclude<float>);
  const auto view3 = registry.view<empty_type>();
  const auto view4 = registry.view<stable_type>();

  static_assert(std::is_same_v<rendu::basic_view<rendu::get_t<rendu::storage_type_t<int>, const rendu::storage_type_t<char>>, rendu::exclude_t<const rendu::storage_type_t<double>, rendu::storage_type_t<float>>>, decltype(view1 | view2)>);
  static_assert(std::is_same_v<rendu::basic_view<rendu::get_t<const rendu::storage_type_t<char>, rendu::storage_type_t<int>>, rendu::exclude_t<rendu::storage_type_t<float>, const rendu::storage_type_t<double>>>, decltype(view2 | view1)>);
  static_assert(std::is_same_v<decltype((view3 | view2) | view1), decltype(view3 | (view2 | view1))>);

  RD_ASSERT_FALSE((view1 | view2).contains(entity));
  RD_ASSERT_TRUE((view1 | view2).contains(other));

  RD_ASSERT_TRUE((view3 | view2).contains(entity));
  RD_ASSERT_FALSE((view3 | view2).contains(other));

  RD_ASSERT_FALSE((view1 | view2 | view3).contains(entity));
  RD_ASSERT_FALSE((view1 | view2 | view3).contains(other));

  RD_ASSERT_FALSE((view1 | view4 | view2).contains(entity));
  RD_ASSERT_TRUE((view1 | view4 | view2).contains(other));
}

RD_TEST(MultiComponentView, Storage) {
  rendu::registry registry;
  const auto entity = registry.create();
  const auto view = registry.view<int, const char>(rendu::exclude<double, const float>);

  static_assert(std::is_same_v<decltype(view.storage<0u>()), rendu::storage_type_t<int> &>);
  static_assert(std::is_same_v<decltype(view.storage<int>()), rendu::storage_type_t<int> &>);
  static_assert(std::is_same_v<decltype(view.storage<const int>()), rendu::storage_type_t<int> &>);
  static_assert(std::is_same_v<decltype(view.storage<1u>()), const rendu::storage_type_t<char> &>);
  static_assert(std::is_same_v<decltype(view.storage<char>()), const rendu::storage_type_t<char> &>);
  static_assert(std::is_same_v<decltype(view.storage<const char>()), const rendu::storage_type_t<char> &>);
  static_assert(std::is_same_v<decltype(view.storage<2u>()), rendu::storage_type_t<double> &>);
  static_assert(std::is_same_v<decltype(view.storage<double>()), rendu::storage_type_t<double> &>);
  static_assert(std::is_same_v<decltype(view.storage<const double>()), rendu::storage_type_t<double> &>);
  static_assert(std::is_same_v<decltype(view.storage<3u>()), const rendu::storage_type_t<float> &>);
  static_assert(std::is_same_v<decltype(view.storage<float>()), const rendu::storage_type_t<float> &>);
  static_assert(std::is_same_v<decltype(view.storage<const float>()), const rendu::storage_type_t<float> &>);

  RD_ASSERT_EQ(view.size_hint(), 0u);

  view.storage<int>().emplace(entity);
  view.storage<double>().emplace(entity);
  registry.emplace<char>(entity);
  registry.emplace<float>(entity);

  RD_ASSERT_EQ(view.size_hint(), 1u);
  RD_ASSERT_EQ(view.begin(), view.end());
  RD_ASSERT_TRUE(view.storage<int>().contains(entity));
  RD_ASSERT_TRUE(view.storage<const char>().contains(entity));
  RD_ASSERT_TRUE(view.storage<double>().contains(entity));
  RD_ASSERT_TRUE(view.storage<const float>().contains(entity));
  RD_ASSERT_TRUE((registry.all_of<int, char, double, float>(entity)));

  view.storage<double>().erase(entity);
  registry.erase<float>(entity);

  RD_ASSERT_EQ(view.size_hint(), 1u);
  RD_ASSERT_NE(view.begin(), view.end());
  RD_ASSERT_TRUE(view.storage<const int>().contains(entity));
  RD_ASSERT_TRUE(view.storage<char>().contains(entity));
  RD_ASSERT_FALSE(view.storage<const double>().contains(entity));
  RD_ASSERT_FALSE(view.storage<float>().contains(entity));
  RD_ASSERT_TRUE((registry.all_of<int, char>(entity)));
  RD_ASSERT_FALSE((registry.any_of<double, float>(entity)));

  view.storage<0u>().erase(entity);

  RD_ASSERT_EQ(view.size_hint(), 0u);
  RD_ASSERT_EQ(view.begin(), view.end());
  RD_ASSERT_FALSE(view.storage<0u>().contains(entity));
  RD_ASSERT_TRUE(view.storage<1u>().contains(entity));
  RD_ASSERT_FALSE(view.storage<2u>().contains(entity));
  RD_ASSERT_FALSE(view.storage<3u>().contains(entity));
  RD_ASSERT_TRUE((registry.all_of<char>(entity)));
  RD_ASSERT_FALSE((registry.any_of<int, double, float>(entity)));
}
