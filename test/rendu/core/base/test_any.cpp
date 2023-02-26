/*
* Created by boil on 2023/2/19.
*/

#include <array>
#include <vector>
#include <unordered_map>
#include <test/rdtest.h>
#include <core/base/any.h>

struct empty {
  ~empty() {
    ++counter;
  }

  inline static int counter = 0;
};

struct fat {
  fat(double v1, double v2, double v3, double v4)
      : value{v1, v2, v3, v4} {}

  ~fat() {
    ++counter;
  }

  bool operator==(const fat &other) const {
    return std::equal(std::begin(value), std::end(value), std::begin(other.value), std::end(other.value));
  }

  inline static int counter{0};
  double value[4];
};

struct not_comparable {
  bool operator==(const not_comparable &) const = delete;
};

struct not_movable {
  not_movable() = default;

  not_movable(const not_movable &) = default;
  not_movable(not_movable &&) = delete;

  not_movable &operator=(const not_movable &) = default;
  not_movable &operator=(not_movable &&) = delete;

  double payload{};
};

struct alignas(64u) over_aligned {};

struct Any : ::testing::Test {
  void SetUp() override {
    fat::counter = 0;
    empty::counter = 0;
  }
};

using AnyDeathTest = Any;

RD_TEST_F(Any, SBO) {
  rendu::any any{'c'};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<char>());
  RD_ASSERT_EQ(rendu::any_cast<double>(&any), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<char>(any), 'c');
}

RD_TEST_F(Any, NoSBO) {
  fat instance{.1, .2, .3, .4};
  rendu::any any{instance};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<fat>());
  RD_ASSERT_EQ(rendu::any_cast<double>(&any), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<fat>(any), instance);
}

RD_TEST_F(Any, Empty) {
  rendu::any any{};

  RD_ASSERT_FALSE(any);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<void>());
  RD_ASSERT_EQ(rendu::any_cast<double>(&any), nullptr);
  RD_ASSERT_EQ(any.data(), nullptr);
}

RD_TEST_F(Any, SBOInPlaceTypeConstruction) {
  rendu::any any{std::in_place_type<int>, 42};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<int>());
  RD_ASSERT_EQ(rendu::any_cast<double>(&any), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<int>(any), 42);

  auto other = any.as_ref();

  RD_ASSERT_TRUE(other);
  RD_ASSERT_FALSE(other.owner());
  RD_ASSERT_EQ(other.type(), rendu::type_id<int>());
  RD_ASSERT_EQ(rendu::any_cast<int>(other), 42);
  RD_ASSERT_EQ(other.data(), any.data());
}

RD_TEST_F(Any, SBOAsRefConstruction) {
  int value = 42;
  rendu::any any{rendu::forward_as_any(value)};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_FALSE(any.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<int>());

  RD_ASSERT_EQ(rendu::any_cast<double>(&any), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<const int>(&any), &value);
  RD_ASSERT_EQ(rendu::any_cast<int>(&any), &value);
  RD_ASSERT_EQ(rendu::any_cast<const int>(&std::as_const(any)), &value);
  RD_ASSERT_EQ(rendu::any_cast<int>(&std::as_const(any)), &value);

  RD_ASSERT_EQ(rendu::any_cast<const int &>(any), 42);
  RD_ASSERT_EQ(rendu::any_cast<int>(any), 42);

  RD_ASSERT_EQ(any.data(), &value);
  RD_ASSERT_EQ(std::as_const(any).data(), &value);

  any.emplace<int &>(value);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_FALSE(any.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<int>());
  RD_ASSERT_EQ(rendu::any_cast<int>(&any), &value);

  auto other = any.as_ref();

  RD_ASSERT_TRUE(other);
  RD_ASSERT_FALSE(other.owner());
  RD_ASSERT_EQ(other.type(), rendu::type_id<int>());
  RD_ASSERT_EQ(rendu::any_cast<int>(other), 42);
  RD_ASSERT_EQ(other.data(), any.data());
}

RD_TEST_F(Any, SBOAsConstRefConstruction) {
  const int value = 42;
  rendu::any any{rendu::forward_as_any(value)};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_FALSE(any.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<int>());

  RD_ASSERT_EQ(rendu::any_cast<double>(&any), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<const int>(&any), &value);
  RD_ASSERT_EQ(rendu::any_cast<int>(&any), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<const int>(&std::as_const(any)), &value);
  RD_ASSERT_EQ(rendu::any_cast<int>(&std::as_const(any)), &value);

  RD_ASSERT_EQ(rendu::any_cast<const int &>(any), 42);
  RD_ASSERT_EQ(rendu::any_cast<int>(any), 42);

  RD_ASSERT_EQ(any.data(), nullptr);
  RD_ASSERT_EQ(std::as_const(any).data(), &value);

  any.emplace<const int &>(value);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_FALSE(any.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<int>());
  RD_ASSERT_EQ(rendu::any_cast<const int>(&any), &value);

  auto other = any.as_ref();

  RD_ASSERT_TRUE(other);
  RD_ASSERT_FALSE(other.owner());
  RD_ASSERT_EQ(other.type(), rendu::type_id<int>());
  RD_ASSERT_EQ(rendu::any_cast<int>(other), 42);
  RD_ASSERT_EQ(other.data(), any.data());
}

RD_TEST_F(Any, SBOCopyConstruction) {
  rendu::any any{42};
  rendu::any other{any};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(other);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_TRUE(other.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<int>());
  RD_ASSERT_EQ(other.type(), rendu::type_id<int>());
  RD_ASSERT_EQ(rendu::any_cast<double>(&other), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<int>(other), 42);
}

RD_TEST_F(Any, SBOCopyAssignment) {
  rendu::any any{42};
  rendu::any other{3};

  other = any;

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(other);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_TRUE(other.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<int>());
  RD_ASSERT_EQ(other.type(), rendu::type_id<int>());
  RD_ASSERT_EQ(rendu::any_cast<double>(&other), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<int>(other), 42);
}

RD_TEST_F(Any, SBOMoveConstruction) {
  rendu::any any{42};
  rendu::any other{std::move(any)};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(other);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_TRUE(other.owner());
  RD_ASSERT_NE(any.data(), nullptr);
  RD_ASSERT_EQ(any.type(), rendu::type_id<int>());
  RD_ASSERT_EQ(other.type(), rendu::type_id<int>());
  RD_ASSERT_EQ(rendu::any_cast<double>(&other), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<int>(other), 42);
}

RD_TEST_F(Any, SBOMoveAssignment) {
  rendu::any any{42};
  rendu::any other{3};

  other = std::move(any);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(other);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_TRUE(other.owner());
  RD_ASSERT_NE(any.data(), nullptr);
  RD_ASSERT_EQ(any.type(), rendu::type_id<int>());
  RD_ASSERT_EQ(other.type(), rendu::type_id<int>());
  RD_ASSERT_EQ(rendu::any_cast<double>(&other), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<int>(other), 42);
}

RD_TEST_F(Any, SBODirectAssignment) {
  rendu::any any{};
  any = 42;

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<int>());
  RD_ASSERT_EQ(rendu::any_cast<double>(&any), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<int>(any), 42);
}

RD_TEST_F(Any, SBOAssignValue) {
  rendu::any any{42};
  rendu::any other{3};
  rendu::any invalid{'c'};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(rendu::any_cast<int>(any), 42);

  RD_ASSERT_TRUE(any.assign(other));
  RD_ASSERT_FALSE(any.assign(invalid));
  RD_ASSERT_EQ(rendu::any_cast<int>(any), 3);
}

RD_TEST_F(Any, SBOAsRefAssignValue) {
  int value = 42;
  rendu::any any{rendu::forward_as_any(value)};
  rendu::any other{3};
  rendu::any invalid{'c'};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(rendu::any_cast<int>(any), 42);

  RD_ASSERT_TRUE(any.assign(other));
  RD_ASSERT_FALSE(any.assign(invalid));
  RD_ASSERT_EQ(rendu::any_cast<int>(any), 3);
  RD_ASSERT_EQ(value, 3);
}

RD_TEST_F(Any, SBOAsConstRefAssignValue) {
  const int value = 42;
  rendu::any any{rendu::forward_as_any(value)};
  rendu::any other{3};
  rendu::any invalid{'c'};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(rendu::any_cast<int>(any), 42);

  RD_ASSERT_FALSE(any.assign(other));
  RD_ASSERT_FALSE(any.assign(invalid));
  RD_ASSERT_EQ(rendu::any_cast<int>(any), 42);
  RD_ASSERT_EQ(value, 42);
}

RD_TEST_F(Any, SBOTransferValue) {
  rendu::any any{42};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(rendu::any_cast<int>(any), 42);

  RD_ASSERT_TRUE(any.assign(3));
  RD_ASSERT_FALSE(any.assign('c'));
  RD_ASSERT_EQ(rendu::any_cast<int>(any), 3);
}

RD_TEST_F(Any, SBOTransferConstValue) {
  const int value = 3;
  rendu::any any{42};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(rendu::any_cast<int>(any), 42);

  RD_ASSERT_TRUE(any.assign(rendu::forward_as_any(value)));
  RD_ASSERT_EQ(rendu::any_cast<int>(any), 3);
}

RD_TEST_F(Any, SBOAsRefTransferValue) {
  int value = 42;
  rendu::any any{rendu::forward_as_any(value)};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(rendu::any_cast<int>(any), 42);

  RD_ASSERT_TRUE(any.assign(3));
  RD_ASSERT_FALSE(any.assign('c'));
  RD_ASSERT_EQ(rendu::any_cast<int>(any), 3);
  RD_ASSERT_EQ(value, 3);
}

RD_TEST_F(Any, SBOAsConstRefTransferValue) {
  const int value = 42;
  rendu::any any{rendu::forward_as_any(value)};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(rendu::any_cast<int>(any), 42);

  RD_ASSERT_FALSE(any.assign(3));
  RD_ASSERT_FALSE(any.assign('c'));
  RD_ASSERT_EQ(rendu::any_cast<int>(any), 42);
  RD_ASSERT_EQ(value, 42);
}

RD_TEST_F(Any, NoSBOInPlaceTypeConstruction) {
  fat instance{.1, .2, .3, .4};
  rendu::any any{std::in_place_type<fat>, instance};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<fat>());
  RD_ASSERT_EQ(rendu::any_cast<double>(&any), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<fat>(any), instance);

  auto other = any.as_ref();

  RD_ASSERT_TRUE(other);
  RD_ASSERT_FALSE(other.owner());
  RD_ASSERT_EQ(other.type(), rendu::type_id<fat>());
  RD_ASSERT_EQ(rendu::any_cast<fat>(other), (fat{.1, .2, .3, .4}));
  RD_ASSERT_EQ(other.data(), any.data());
}

RD_TEST_F(Any, NoSBOAsRefConstruction) {
  fat instance{.1, .2, .3, .4};
  rendu::any any{rendu::forward_as_any(instance)};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_FALSE(any.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<fat>());

  RD_ASSERT_EQ(rendu::any_cast<double>(&any), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<const fat>(&any), &instance);
  RD_ASSERT_EQ(rendu::any_cast<fat>(&any), &instance);
  RD_ASSERT_EQ(rendu::any_cast<const fat>(&std::as_const(any)), &instance);
  RD_ASSERT_EQ(rendu::any_cast<fat>(&std::as_const(any)), &instance);

  RD_ASSERT_EQ(rendu::any_cast<const fat &>(any), instance);
  RD_ASSERT_EQ(rendu::any_cast<fat>(any), instance);

  RD_ASSERT_EQ(any.data(), &instance);
  RD_ASSERT_EQ(std::as_const(any).data(), &instance);

  any.emplace<fat &>(instance);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_FALSE(any.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<fat>());
  RD_ASSERT_EQ(rendu::any_cast<fat>(&any), &instance);

  auto other = any.as_ref();

  RD_ASSERT_TRUE(other);
  RD_ASSERT_FALSE(other.owner());
  RD_ASSERT_EQ(other.type(), rendu::type_id<fat>());
  RD_ASSERT_EQ(rendu::any_cast<fat>(other), (fat{.1, .2, .3, .4}));
  RD_ASSERT_EQ(other.data(), any.data());
}

RD_TEST_F(Any, NoSBOAsConstRefConstruction) {
  const fat instance{.1, .2, .3, .4};
  rendu::any any{rendu::forward_as_any(instance)};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_FALSE(any.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<fat>());

  RD_ASSERT_EQ(rendu::any_cast<double>(&any), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<const fat>(&any), &instance);
  RD_ASSERT_EQ(rendu::any_cast<fat>(&any), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<const fat>(&std::as_const(any)), &instance);
  RD_ASSERT_EQ(rendu::any_cast<fat>(&std::as_const(any)), &instance);

  RD_ASSERT_EQ(rendu::any_cast<const fat &>(any), instance);
  RD_ASSERT_EQ(rendu::any_cast<fat>(any), instance);

  RD_ASSERT_EQ(any.data(), nullptr);
  RD_ASSERT_EQ(std::as_const(any).data(), &instance);

  any.emplace<const fat &>(instance);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_FALSE(any.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<fat>());
  RD_ASSERT_EQ(rendu::any_cast<const fat>(&any), &instance);

  auto other = any.as_ref();

  RD_ASSERT_TRUE(other);
  RD_ASSERT_FALSE(other.owner());
  RD_ASSERT_EQ(other.type(), rendu::type_id<fat>());
  RD_ASSERT_EQ(rendu::any_cast<fat>(other), (fat{.1, .2, .3, .4}));
  RD_ASSERT_EQ(other.data(), any.data());
}

RD_TEST_F(Any, NoSBOCopyConstruction) {
  fat instance{.1, .2, .3, .4};
  rendu::any any{instance};
  rendu::any other{any};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(other);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_TRUE(other.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<fat>());
  RD_ASSERT_EQ(other.type(), rendu::type_id<fat>());
  RD_ASSERT_EQ(rendu::any_cast<double>(&other), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<fat>(other), instance);
}

RD_TEST_F(Any, NoSBOCopyAssignment) {
  fat instance{.1, .2, .3, .4};
  rendu::any any{instance};
  rendu::any other{3};

  other = any;

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(other);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_TRUE(other.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<fat>());
  RD_ASSERT_EQ(other.type(), rendu::type_id<fat>());
  RD_ASSERT_EQ(rendu::any_cast<double>(&other), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<fat>(other), instance);
}

RD_TEST_F(Any, NoSBOMoveConstruction) {
  fat instance{.1, .2, .3, .4};
  rendu::any any{instance};
  rendu::any other{std::move(any)};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(other);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_TRUE(other.owner());
  RD_ASSERT_EQ(any.data(), nullptr);
  RD_ASSERT_EQ(any.type(), rendu::type_id<fat>());
  RD_ASSERT_EQ(other.type(), rendu::type_id<fat>());
  RD_ASSERT_EQ(rendu::any_cast<double>(&other), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<fat>(other), instance);
}

RD_TEST_F(Any, NoSBOMoveAssignment) {
  fat instance{.1, .2, .3, .4};
  rendu::any any{instance};
  rendu::any other{3};

  other = std::move(any);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(other);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_TRUE(other.owner());
  RD_ASSERT_EQ(any.data(), nullptr);
  RD_ASSERT_EQ(any.type(), rendu::type_id<fat>());
  RD_ASSERT_EQ(other.type(), rendu::type_id<fat>());
  RD_ASSERT_EQ(rendu::any_cast<double>(&other), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<fat>(other), instance);
}

RD_TEST_F(Any, NoSBODirectAssignment) {
  fat instance{.1, .2, .3, .4};
  rendu::any any{};
  any = instance;

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<fat>());
  RD_ASSERT_EQ(rendu::any_cast<double>(&any), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<fat>(any), instance);
}

RD_TEST_F(Any, NoSBOAssignValue) {
  rendu::any any{fat{.1, .2, .3, .4}};
  rendu::any other{fat{.0, .1, .2, .3}};
  rendu::any invalid{'c'};

  const void *addr = std::as_const(any).data();

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.1, .2, .3, .4}));

  RD_ASSERT_TRUE(any.assign(other));
  RD_ASSERT_FALSE(any.assign(invalid));
  RD_ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.0, .1, .2, .3}));
  RD_ASSERT_EQ(addr, std::as_const(any).data());
}

RD_TEST_F(Any, NoSBOAsRefAssignValue) {
  fat instance{.1, .2, .3, .4};
  rendu::any any{rendu::forward_as_any(instance)};
  rendu::any other{fat{.0, .1, .2, .3}};
  rendu::any invalid{'c'};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.1, .2, .3, .4}));

  RD_ASSERT_TRUE(any.assign(other));
  RD_ASSERT_FALSE(any.assign(invalid));
  RD_ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.0, .1, .2, .3}));
  RD_ASSERT_EQ(instance, (fat{.0, .1, .2, .3}));
}

RD_TEST_F(Any, NoSBOAsConstRefAssignValue) {
  const fat instance{.1, .2, .3, .4};
  rendu::any any{rendu::forward_as_any(instance)};
  rendu::any other{fat{.0, .1, .2, .3}};
  rendu::any invalid{'c'};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.1, .2, .3, .4}));

  RD_ASSERT_FALSE(any.assign(other));
  RD_ASSERT_FALSE(any.assign(invalid));
  RD_ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.1, .2, .3, .4}));
  RD_ASSERT_EQ(instance, (fat{.1, .2, .3, .4}));
}

RD_TEST_F(Any, NoSBOTransferValue) {
  rendu::any any{fat{.1, .2, .3, .4}};

  const void *addr = std::as_const(any).data();

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.1, .2, .3, .4}));

  RD_ASSERT_TRUE(any.assign(fat{.0, .1, .2, .3}));
  RD_ASSERT_FALSE(any.assign('c'));
  RD_ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.0, .1, .2, .3}));
  RD_ASSERT_EQ(addr, std::as_const(any).data());
}

RD_TEST_F(Any, NoSBOTransferConstValue) {
  const fat instance{.0, .1, .2, .3};
  rendu::any any{fat{.1, .2, .3, .4}};

  const void *addr = std::as_const(any).data();

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.1, .2, .3, .4}));

  RD_ASSERT_TRUE(any.assign(rendu::forward_as_any(instance)));
  RD_ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.0, .1, .2, .3}));
  RD_ASSERT_EQ(addr, std::as_const(any).data());
}

RD_TEST_F(Any, NoSBOAsRefTransferValue) {
  fat instance{.1, .2, .3, .4};
  rendu::any any{rendu::forward_as_any(instance)};

  const void *addr = std::as_const(any).data();

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.1, .2, .3, .4}));

  RD_ASSERT_TRUE(any.assign(fat{.0, .1, .2, .3}));
  RD_ASSERT_FALSE(any.assign('c'));
  RD_ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.0, .1, .2, .3}));
  RD_ASSERT_EQ(instance, (fat{.0, .1, .2, .3}));
  RD_ASSERT_EQ(addr, std::as_const(any).data());
}

RD_TEST_F(Any, NoSBOAsConstRefTransferValue) {
  const fat instance{.1, .2, .3, .4};
  rendu::any any{rendu::forward_as_any(instance)};

  const void *addr = std::as_const(any).data();

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.1, .2, .3, .4}));

  RD_ASSERT_FALSE(any.assign(fat{.0, .1, .2, .3}));
  RD_ASSERT_FALSE(any.assign('c'));
  RD_ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.1, .2, .3, .4}));
  RD_ASSERT_EQ(instance, (fat{.1, .2, .3, .4}));
  RD_ASSERT_EQ(addr, std::as_const(any).data());
}

RD_TEST_F(Any, VoidInPlaceTypeConstruction) {
  rendu::any any{std::in_place_type<void>};

  RD_ASSERT_FALSE(any);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<void>());
  RD_ASSERT_EQ(rendu::any_cast<int>(&any), nullptr);
}

RD_TEST_F(Any, VoidCopyConstruction) {
  rendu::any any{std::in_place_type<void>};
  rendu::any other{any};

  RD_ASSERT_FALSE(any);
  RD_ASSERT_FALSE(other);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_TRUE(other.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<void>());
  RD_ASSERT_EQ(other.type(), rendu::type_id<void>());
  RD_ASSERT_EQ(rendu::any_cast<int>(&any), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<double>(&other), nullptr);
}

RD_TEST_F(Any, VoidCopyAssignment) {
  rendu::any any{std::in_place_type<void>};
  rendu::any other{42};

  other = any;

  RD_ASSERT_FALSE(any);
  RD_ASSERT_FALSE(other);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_TRUE(other.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<void>());
  RD_ASSERT_EQ(other.type(), rendu::type_id<void>());
  RD_ASSERT_EQ(rendu::any_cast<int>(&any), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<double>(&other), nullptr);
}

RD_TEST_F(Any, VoidMoveConstruction) {
  rendu::any any{std::in_place_type<void>};
  rendu::any other{std::move(any)};

  RD_ASSERT_FALSE(any);
  RD_ASSERT_FALSE(other);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_TRUE(other.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<void>());
  RD_ASSERT_EQ(other.type(), rendu::type_id<void>());
  RD_ASSERT_EQ(rendu::any_cast<int>(&any), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<double>(&other), nullptr);
}

RD_TEST_F(Any, VoidMoveAssignment) {
  rendu::any any{std::in_place_type<void>};
  rendu::any other{42};

  other = std::move(any);

  RD_ASSERT_FALSE(any);
  RD_ASSERT_FALSE(other);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_TRUE(other.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<void>());
  RD_ASSERT_EQ(other.type(), rendu::type_id<void>());
  RD_ASSERT_EQ(rendu::any_cast<int>(&any), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<double>(&other), nullptr);
}

RD_TEST_F(Any, SBOMoveValidButUnspecifiedState) {
  rendu::any any{42};
  rendu::any other{std::move(any)};
  rendu::any valid = std::move(other);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(other);
  RD_ASSERT_TRUE(valid);
}

RD_TEST_F(Any, NoSBOMoveValidButUnspecifiedState) {
  fat instance{.1, .2, .3, .4};
  rendu::any any{instance};
  rendu::any other{std::move(any)};
  rendu::any valid = std::move(other);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(other);
  RD_ASSERT_TRUE(valid);
}

RD_TEST_F(Any, VoidMoveValidButUnspecifiedState) {
  rendu::any any{std::in_place_type<void>};
  rendu::any other{std::move(any)};
  rendu::any valid = std::move(other);

  RD_ASSERT_FALSE(any);
  RD_ASSERT_FALSE(other);
  RD_ASSERT_FALSE(valid);
}

RD_TEST_F(Any, SBODestruction) {
  {
    rendu::any any{std::in_place_type<empty>};
    any.emplace<empty>();
    any = empty{};
    rendu::any other{std::move(any)};
    any = std::move(other);
  }

  RD_ASSERT_EQ(empty::counter, 6);
}

RD_TEST_F(Any, NoSBODestruction) {
  {
    rendu::any any{std::in_place_type<fat>, 1., 2., 3., 4.};
    any.emplace<fat>(1., 2., 3., 4.);
    any = fat{1., 2., 3., 4.};
    rendu::any other{std::move(any)};
    any = std::move(other);
  }

  RD_ASSERT_EQ(fat::counter, 4);
}

RD_TEST_F(Any, VoidDestruction) {
  // just let asan tell us if everything is ok here
  [[maybe_unused]] rendu::any any{std::in_place_type<void>};
}

RD_TEST_F(Any, Emplace) {
  rendu::any any{};
  any.emplace<int>(42);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<int>());
  RD_ASSERT_EQ(rendu::any_cast<double>(&any), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<int>(any), 42);
}

RD_TEST_F(Any, EmplaceVoid) {
  rendu::any any{};
  any.emplace<void>();

  RD_ASSERT_FALSE(any);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<void>());
}

RD_TEST_F(Any, Reset) {
  rendu::any any{42};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<int>());

  any.reset();

  RD_ASSERT_FALSE(any);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<void>());

  int value = 42;
  any.emplace<int &>(value);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_FALSE(any.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<int>());

  any.reset();

  RD_ASSERT_FALSE(any);
  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_EQ(any.type(), rendu::type_id<void>());
}

RD_TEST_F(Any, SBOSwap) {
  rendu::any lhs{'c'};
  rendu::any rhs{42};

  std::swap(lhs, rhs);

  RD_ASSERT_TRUE(lhs.owner());
  RD_ASSERT_TRUE(rhs.owner());

  RD_ASSERT_EQ(lhs.type(), rendu::type_id<int>());
  RD_ASSERT_EQ(rhs.type(), rendu::type_id<char>());
  RD_ASSERT_EQ(rendu::any_cast<char>(&lhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<int>(&rhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<int>(lhs), 42);
  RD_ASSERT_EQ(rendu::any_cast<char>(rhs), 'c');
}

RD_TEST_F(Any, NoSBOSwap) {
  rendu::any lhs{fat{.1, .2, .3, .4}};
  rendu::any rhs{fat{.4, .3, .2, .1}};

  std::swap(lhs, rhs);

  RD_ASSERT_TRUE(lhs.owner());
  RD_ASSERT_TRUE(rhs.owner());

  RD_ASSERT_EQ(rendu::any_cast<fat>(lhs), (fat{.4, .3, .2, .1}));
  RD_ASSERT_EQ(rendu::any_cast<fat>(rhs), (fat{.1, .2, .3, .4}));
}

RD_TEST_F(Any, VoidSwap) {
  rendu::any lhs{std::in_place_type<void>};
  rendu::any rhs{std::in_place_type<void>};
  const auto *pre = lhs.data();

  std::swap(lhs, rhs);

  RD_ASSERT_TRUE(lhs.owner());
  RD_ASSERT_TRUE(rhs.owner());

  RD_ASSERT_EQ(pre, lhs.data());
}

RD_TEST_F(Any, SBOWithNoSBOSwap) {
  rendu::any lhs{fat{.1, .2, .3, .4}};
  rendu::any rhs{'c'};

  std::swap(lhs, rhs);

  RD_ASSERT_TRUE(lhs.owner());
  RD_ASSERT_TRUE(rhs.owner());

  RD_ASSERT_EQ(lhs.type(), rendu::type_id<char>());
  RD_ASSERT_EQ(rhs.type(), rendu::type_id<fat>());
  RD_ASSERT_EQ(rendu::any_cast<fat>(&lhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<char>(&rhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<char>(lhs), 'c');
  RD_ASSERT_EQ(rendu::any_cast<fat>(rhs), (fat{.1, .2, .3, .4}));
}

RD_TEST_F(Any, SBOWithRefSwap) {
  int value = 3;
  rendu::any lhs{rendu::forward_as_any(value)};
  rendu::any rhs{'c'};

  std::swap(lhs, rhs);

  RD_ASSERT_TRUE(lhs.owner());
  RD_ASSERT_FALSE(rhs.owner());

  RD_ASSERT_EQ(lhs.type(), rendu::type_id<char>());
  RD_ASSERT_EQ(rhs.type(), rendu::type_id<int>());
  RD_ASSERT_EQ(rendu::any_cast<int>(&lhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<char>(&rhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<char>(lhs), 'c');
  RD_ASSERT_EQ(rendu::any_cast<int>(rhs), 3);
  RD_ASSERT_EQ(rhs.data(), &value);
}

RD_TEST_F(Any, SBOWithConstRefSwap) {
  const int value = 3;
  rendu::any lhs{rendu::forward_as_any(value)};
  rendu::any rhs{'c'};

  std::swap(lhs, rhs);

  RD_ASSERT_TRUE(lhs.owner());
  RD_ASSERT_FALSE(rhs.owner());

  RD_ASSERT_EQ(lhs.type(), rendu::type_id<char>());
  RD_ASSERT_EQ(rhs.type(), rendu::type_id<int>());
  RD_ASSERT_EQ(rendu::any_cast<int>(&lhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<char>(&rhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<char>(lhs), 'c');
  RD_ASSERT_EQ(rendu::any_cast<int>(rhs), 3);
  RD_ASSERT_EQ(rhs.data(), nullptr);
  RD_ASSERT_EQ(std::as_const(rhs).data(), &value);
}

RD_TEST_F(Any, SBOWithEmptySwap) {
  rendu::any lhs{'c'};
  rendu::any rhs{};

  std::swap(lhs, rhs);

  RD_ASSERT_FALSE(lhs);
  RD_ASSERT_TRUE(lhs.owner());
  RD_ASSERT_EQ(rhs.type(), rendu::type_id<char>());
  RD_ASSERT_EQ(rendu::any_cast<char>(&lhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<double>(&rhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<char>(rhs), 'c');

  std::swap(lhs, rhs);

  RD_ASSERT_FALSE(rhs);
  RD_ASSERT_TRUE(rhs.owner());
  RD_ASSERT_EQ(lhs.type(), rendu::type_id<char>());
  RD_ASSERT_EQ(rendu::any_cast<double>(&lhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<char>(&rhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<char>(lhs), 'c');
}

RD_TEST_F(Any, SBOWithVoidSwap) {
  rendu::any lhs{'c'};
  rendu::any rhs{std::in_place_type<void>};

  std::swap(lhs, rhs);

  RD_ASSERT_FALSE(lhs);
  RD_ASSERT_TRUE(lhs.owner());
  RD_ASSERT_EQ(rhs.type(), rendu::type_id<char>());
  RD_ASSERT_EQ(rendu::any_cast<char>(&lhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<double>(&rhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<char>(rhs), 'c');

  std::swap(lhs, rhs);

  RD_ASSERT_FALSE(rhs);
  RD_ASSERT_TRUE(rhs.owner());
  RD_ASSERT_EQ(lhs.type(), rendu::type_id<char>());
  RD_ASSERT_EQ(rendu::any_cast<double>(&lhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<char>(&rhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<char>(lhs), 'c');
}

RD_TEST_F(Any, NoSBOWithRefSwap) {
  int value = 3;
  rendu::any lhs{rendu::forward_as_any(value)};
  rendu::any rhs{fat{.1, .2, .3, .4}};

  std::swap(lhs, rhs);

  RD_ASSERT_TRUE(lhs.owner());
  RD_ASSERT_FALSE(rhs.owner());

  RD_ASSERT_EQ(lhs.type(), rendu::type_id<fat>());
  RD_ASSERT_EQ(rhs.type(), rendu::type_id<int>());
  RD_ASSERT_EQ(rendu::any_cast<int>(&lhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<fat>(&rhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<fat>(lhs), (fat{.1, .2, .3, .4}));
  RD_ASSERT_EQ(rendu::any_cast<int>(rhs), 3);
  RD_ASSERT_EQ(rhs.data(), &value);
}

RD_TEST_F(Any, NoSBOWithConstRefSwap) {
  const int value = 3;
  rendu::any lhs{rendu::forward_as_any(value)};
  rendu::any rhs{fat{.1, .2, .3, .4}};

  std::swap(lhs, rhs);

  RD_ASSERT_TRUE(lhs.owner());
  RD_ASSERT_FALSE(rhs.owner());

  RD_ASSERT_EQ(lhs.type(), rendu::type_id<fat>());
  RD_ASSERT_EQ(rhs.type(), rendu::type_id<int>());
  RD_ASSERT_EQ(rendu::any_cast<int>(&lhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<fat>(&rhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<fat>(lhs), (fat{.1, .2, .3, .4}));
  RD_ASSERT_EQ(rendu::any_cast<int>(rhs), 3);
  RD_ASSERT_EQ(rhs.data(), nullptr);
  RD_ASSERT_EQ(std::as_const(rhs).data(), &value);
}

RD_TEST_F(Any, NoSBOWithEmptySwap) {
  rendu::any lhs{fat{.1, .2, .3, .4}};
  rendu::any rhs{};

  std::swap(lhs, rhs);

  RD_ASSERT_FALSE(lhs);
  RD_ASSERT_TRUE(lhs.owner());
  RD_ASSERT_EQ(rhs.type(), rendu::type_id<fat>());
  RD_ASSERT_EQ(rendu::any_cast<fat>(&lhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<double>(&rhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<fat>(rhs), (fat{.1, .2, .3, .4}));

  std::swap(lhs, rhs);

  RD_ASSERT_FALSE(rhs);
  RD_ASSERT_TRUE(rhs.owner());
  RD_ASSERT_EQ(lhs.type(), rendu::type_id<fat>());
  RD_ASSERT_EQ(rendu::any_cast<double>(&lhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<fat>(&rhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<fat>(lhs), (fat{.1, .2, .3, .4}));
}

RD_TEST_F(Any, NoSBOWithVoidSwap) {
  rendu::any lhs{fat{.1, .2, .3, .4}};
  rendu::any rhs{std::in_place_type<void>};

  std::swap(lhs, rhs);

  RD_ASSERT_FALSE(lhs);
  RD_ASSERT_TRUE(lhs.owner());
  RD_ASSERT_EQ(rhs.type(), rendu::type_id<fat>());
  RD_ASSERT_EQ(rendu::any_cast<fat>(&lhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<double>(&rhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<fat>(rhs), (fat{.1, .2, .3, .4}));

  std::swap(lhs, rhs);

  RD_ASSERT_FALSE(rhs);
  RD_ASSERT_TRUE(rhs.owner());
  RD_ASSERT_EQ(lhs.type(), rendu::type_id<fat>());
  RD_ASSERT_EQ(rendu::any_cast<double>(&lhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<fat>(&rhs), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<fat>(lhs), (fat{.1, .2, .3, .4}));
}

RD_TEST_F(Any, AsRef) {
  rendu::any any{42};
  auto ref = any.as_ref();
  auto cref = std::as_const(any).as_ref();

  RD_ASSERT_FALSE(ref.owner());
  RD_ASSERT_FALSE(cref.owner());

  RD_ASSERT_EQ(rendu::any_cast<int>(&any), any.data());
  RD_ASSERT_EQ(rendu::any_cast<int>(&ref), any.data());
  RD_ASSERT_EQ(rendu::any_cast<int>(&cref), nullptr);

  RD_ASSERT_EQ(rendu::any_cast<const int>(&any), any.data());
  RD_ASSERT_EQ(rendu::any_cast<const int>(&ref), any.data());
  RD_ASSERT_EQ(rendu::any_cast<const int>(&cref), any.data());

  RD_ASSERT_EQ(rendu::any_cast<int>(any), 42);
  RD_ASSERT_EQ(rendu::any_cast<int>(ref), 42);
  RD_ASSERT_EQ(rendu::any_cast<int>(cref), 42);

  RD_ASSERT_EQ(rendu::any_cast<const int>(any), 42);
  RD_ASSERT_EQ(rendu::any_cast<const int>(ref), 42);
  RD_ASSERT_EQ(rendu::any_cast<const int>(cref), 42);

  RD_ASSERT_EQ(rendu::any_cast<int &>(any), 42);
  RD_ASSERT_EQ(rendu::any_cast<const int &>(any), 42);
  RD_ASSERT_EQ(rendu::any_cast<int &>(ref), 42);
  RD_ASSERT_EQ(rendu::any_cast<const int &>(ref), 42);
  RD_ASSERT_EQ(rendu::any_cast<int>(&cref), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<const int &>(cref), 42);

  rendu::any_cast<int &>(any) = 3;

  RD_ASSERT_EQ(rendu::any_cast<int>(any), 3);
  RD_ASSERT_EQ(rendu::any_cast<int>(ref), 3);
  RD_ASSERT_EQ(rendu::any_cast<int>(cref), 3);

  std::swap(ref, cref);

  RD_ASSERT_FALSE(ref.owner());
  RD_ASSERT_FALSE(cref.owner());

  RD_ASSERT_EQ(rendu::any_cast<int>(&ref), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<int>(&cref), any.data());

  ref = ref.as_ref();
  cref = std::as_const(cref).as_ref();

  RD_ASSERT_FALSE(ref.owner());
  RD_ASSERT_FALSE(cref.owner());

  RD_ASSERT_EQ(rendu::any_cast<int>(&ref), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<int>(&cref), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<const int>(&ref), any.data());
  RD_ASSERT_EQ(rendu::any_cast<const int>(&cref), any.data());

  RD_ASSERT_EQ(rendu::any_cast<int>(&ref), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<int>(&cref), nullptr);

  RD_ASSERT_EQ(rendu::any_cast<const int &>(ref), 3);
  RD_ASSERT_EQ(rendu::any_cast<const int &>(cref), 3);

  ref = 42;
  cref = 42;

  RD_ASSERT_TRUE(ref.owner());
  RD_ASSERT_TRUE(cref.owner());

  RD_ASSERT_NE(rendu::any_cast<int>(&ref), nullptr);
  RD_ASSERT_NE(rendu::any_cast<int>(&cref), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<int &>(ref), 42);
  RD_ASSERT_EQ(rendu::any_cast<int &>(cref), 42);
  RD_ASSERT_EQ(rendu::any_cast<const int &>(ref), 42);
  RD_ASSERT_EQ(rendu::any_cast<const int &>(cref), 42);
  RD_ASSERT_NE(rendu::any_cast<int>(&ref), any.data());
  RD_ASSERT_NE(rendu::any_cast<int>(&cref), any.data());
}

RD_TEST_F(Any, Comparable) {
  auto test = [](rendu::any any, rendu::any other) {
    RD_ASSERT_EQ(any, any);
    RD_ASSERT_NE(other, any);
    RD_ASSERT_NE(any, rendu::any{});

    RD_ASSERT_TRUE(any == any);
    RD_ASSERT_FALSE(other == any);
    RD_ASSERT_TRUE(any != other);
    RD_ASSERT_TRUE(rendu::any{} != any);
  };

  int value = 42;

  test('c', 'a');
  test(fat{.1, .2, .3, .4}, fat{.0, .1, .2, .3});
  test(rendu::forward_as_any(value), 3);
  test(3, rendu::make_any<const int &>(value));
  test('c', value);
}

RD_TEST_F(Any, NotComparable) {
  auto test = [](const auto &instance) {
    auto any = rendu::forward_as_any(instance);

    RD_ASSERT_EQ(any, any);
    RD_ASSERT_NE(any, rendu::any{instance});
    RD_ASSERT_NE(rendu::any{}, any);

    RD_ASSERT_TRUE(any == any);
    RD_ASSERT_FALSE(any == rendu::any{instance});
    RD_ASSERT_TRUE(rendu::any{} != any);
  };

  test(not_comparable{});
  test(std::unordered_map < int, not_comparable > {});
  test(std::vector<not_comparable>{});
}

RD_TEST_F(Any, CompareVoid) {
  rendu::any any{std::in_place_type<void>};

  RD_ASSERT_EQ(any, any);
  RD_ASSERT_EQ(any, rendu::any{std::in_place_type<void>});
  RD_ASSERT_NE(rendu::any{'a'}, any);
  RD_ASSERT_EQ(any, rendu::any{});

  RD_ASSERT_TRUE(any == any);
  RD_ASSERT_TRUE(any == rendu::any{std::in_place_type<void>});
  RD_ASSERT_FALSE(rendu::any{'a'} == any);
  RD_ASSERT_TRUE(any != rendu::any{'a'});
  RD_ASSERT_FALSE(rendu::any{} != any);
}

RD_TEST_F(Any, AnyCast) {
  rendu::any any{42};
  const auto &cany = any;

  RD_ASSERT_EQ(rendu::any_cast<char>(&any), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<char>(&cany), nullptr);
  RD_ASSERT_EQ(*rendu::any_cast<int>(&any), 42);
  RD_ASSERT_EQ(*rendu::any_cast<int>(&cany), 42);
  RD_ASSERT_EQ(rendu::any_cast<int &>(any), 42);
  RD_ASSERT_EQ(rendu::any_cast<const int &>(cany), 42);

  auto instance = std::make_unique<double>(42.);
  rendu::any ref{rendu::forward_as_any(instance)};
  rendu::any cref{rendu::forward_as_any(std::as_const(*instance))};

  RD_ASSERT_EQ(rendu::any_cast<double>(std::move(cref)), 42.);
  RD_ASSERT_EQ(*rendu::any_cast<std::unique_ptr<double>>(std::move(ref)), 42.);
  RD_ASSERT_EQ(rendu::any_cast<int>(rendu::any{42}), 42);
}

RD_DEBUG_TEST_F(AnyDeathTest, AnyCast) {
  rendu::any any{42};
  const auto &cany = any;

  RD_ASSERT_DEATH([[maybe_unused]] auto &elem = rendu::any_cast<double &>(any), "");
  RD_ASSERT_DEATH([[maybe_unused]] const auto &elem = rendu::any_cast<const double &>(cany), "");

  auto instance = std::make_unique<double>(42.);
  rendu::any ref{rendu::forward_as_any(instance)};
  rendu::any cref{rendu::forward_as_any(std::as_const(*instance))};

  RD_ASSERT_DEATH([[maybe_unused]] auto elem = rendu::any_cast<std::unique_ptr<double>>(std::as_const(ref).as_ref()),
                  "");
  RD_ASSERT_DEATH([[maybe_unused]] auto elem = rendu::any_cast<double>(rendu::any{42}), "");
}

RD_TEST_F(Any, MakeAny) {
  int value = 42;
  auto any = rendu::make_any<int>(value);
  auto ext = rendu::make_any<int, sizeof(int), alignof(int)>(value);
  auto ref = rendu::make_any<int &>(value);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(ext);
  RD_ASSERT_TRUE(ref);

  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_TRUE(ext.owner());
  RD_ASSERT_FALSE(ref.owner());

  RD_ASSERT_EQ(rendu::any_cast<const int &>(any), 42);
  RD_ASSERT_EQ(rendu::any_cast<const int &>(ext), 42);
  RD_ASSERT_EQ(rendu::any_cast<const int &>(ref), 42);

  RD_ASSERT_EQ(decltype(any)::length, rendu::any::length);
  RD_ASSERT_NE(decltype(ext)::length, rendu::any::length);
  RD_ASSERT_EQ(decltype(ref)::length, rendu::any::length);

  RD_ASSERT_NE(any.data(), &value);
  RD_ASSERT_NE(ext.data(), &value);
  RD_ASSERT_EQ(ref.data(), &value);
}

RD_TEST_F(Any, ForwardAsAny) {
  int value = 42;
  auto ref = rendu::forward_as_any(value);
  auto cref = rendu::forward_as_any(std::as_const(value));
  auto any = rendu::forward_as_any(std::move(value));

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(ref);
  RD_ASSERT_TRUE(cref);

  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_FALSE(ref.owner());
  RD_ASSERT_FALSE(cref.owner());

  RD_ASSERT_NE(rendu::any_cast<int>(&any), nullptr);
  RD_ASSERT_NE(rendu::any_cast<int>(&ref), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<int>(&cref), nullptr);

  RD_ASSERT_EQ(rendu::any_cast<const int &>(any), 42);
  RD_ASSERT_EQ(rendu::any_cast<const int &>(ref), 42);
  RD_ASSERT_EQ(rendu::any_cast<const int &>(cref), 42);

  RD_ASSERT_NE(any.data(), &value);
  RD_ASSERT_EQ(ref.data(), &value);
}

RD_TEST_F(Any, NotCopyableType) {
  const std::unique_ptr<int> value{};
  rendu::any any{std::in_place_type<std::unique_ptr<int>>};
  rendu::any other = rendu::forward_as_any(value);

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(other);

  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_FALSE(other.owner());
  RD_ASSERT_EQ(any.type(), other.type());

  RD_ASSERT_FALSE(any.assign(other));
  RD_ASSERT_FALSE(any.assign(std::move(other)));

  rendu::any copy{any};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_FALSE(copy);

  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_TRUE(copy.owner());

  copy = any;

  RD_ASSERT_TRUE(any);
  RD_ASSERT_FALSE(copy);

  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_TRUE(copy.owner());
}

RD_TEST_F(Any, NotCopyableValueType) {
  std::vector<rendu::any> vec{};
  vec.emplace_back(std::in_place_type<std::unique_ptr<int>>);
  vec.shrink_to_fit();

  RD_ASSERT_EQ(vec.size(), 1u);
  RD_ASSERT_EQ(vec.capacity(), 1u);
  RD_ASSERT_TRUE(vec[0u]);

  // strong exception guarantee due to noexcept move ctor
  vec.emplace_back(std::in_place_type<std::unique_ptr<int>>);

  RD_ASSERT_EQ(vec.size(), 2u);
  RD_ASSERT_TRUE(vec[0u]);
  RD_ASSERT_TRUE(vec[1u]);
}

RD_TEST_F(Any, NotMovableType) {
  rendu::any any{std::in_place_type<not_movable>};
  rendu::any other{std::in_place_type<not_movable>};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(other);

  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_TRUE(other.owner());
  RD_ASSERT_EQ(any.type(), other.type());

  RD_ASSERT_TRUE(any.assign(other));
  RD_ASSERT_TRUE(any.assign(std::move(other)));

  rendu::any copy{any};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(copy);

  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_TRUE(copy.owner());

  copy = any;

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(copy);

  RD_ASSERT_TRUE(any.owner());
  RD_ASSERT_TRUE(copy.owner());
}

RD_TEST_F(Any, Array) {
  rendu::any any{std::in_place_type<int[1]>};
  rendu::any copy{any};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_FALSE(copy);

  RD_ASSERT_EQ(any.type(), rendu::type_id<int[1]>());
  RD_ASSERT_NE(rendu::any_cast<int[1]>(&any), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<int[2]>(&any), nullptr);
  RD_ASSERT_EQ(rendu::any_cast<int *>(&any), nullptr);

  rendu::any_cast<int (&)[1]>(any)[0] = 42;

  RD_ASSERT_EQ(rendu::any_cast<const int (&)[1]>(std::as_const(any))[0], 42);
}

RD_TEST_F(Any, CopyMoveReference) {
  int value{};

  auto test = [&](auto &&ref) {
    value = 3;

    auto any = rendu::forward_as_any(ref);
    rendu::any move = std::move(any);
    rendu::any copy = move;

    RD_ASSERT_TRUE(any);
    RD_ASSERT_TRUE(move);
    RD_ASSERT_TRUE(copy);

    RD_ASSERT_FALSE(any.owner());
    RD_ASSERT_FALSE(move.owner());
    RD_ASSERT_TRUE(copy.owner());

    RD_ASSERT_EQ(move.type(), rendu::type_id<int>());
    RD_ASSERT_EQ(copy.type(), rendu::type_id<int>());

    RD_ASSERT_EQ(std::as_const(move).data(), &value);
    RD_ASSERT_NE(std::as_const(copy).data(), &value);

    RD_ASSERT_EQ(rendu::any_cast<int>(move), 3);
    RD_ASSERT_EQ(rendu::any_cast<int>(copy), 3);

    value = 42;

    RD_ASSERT_EQ(rendu::any_cast<const int &>(move), 42);
    RD_ASSERT_EQ(rendu::any_cast<const int &>(copy), 3);
  };

  test(value);
  test(std::as_const(value));
}

RD_TEST_F(Any, SBOVsZeroedSBOSize) {
  rendu::any sbo{42};
  const auto *broken = sbo.data();
  rendu::any other = std::move(sbo);

  RD_ASSERT_NE(broken, other.data());

  rendu::basic_any<0u> dyn{42};
  const auto *valid = dyn.data();
  rendu::basic_any<0u> same = std::move(dyn);

  RD_ASSERT_EQ(valid, same.data());
}

RD_TEST_F(Any, SboAlignment) {
  constexpr auto alignment = alignof(over_aligned);
  rendu::basic_any<alignment, alignment> sbo[2] = {over_aligned{}, over_aligned{}};
  const auto *data = sbo[0].data();

  RD_ASSERT_TRUE((reinterpret_cast<std::uintptr_t>(sbo[0u].data()) % alignment) == 0u);
  RD_ASSERT_TRUE((reinterpret_cast<std::uintptr_t>(sbo[1u].data()) % alignment) == 0u);

  std::swap(sbo[0], sbo[1]);

  RD_ASSERT_TRUE((reinterpret_cast<std::uintptr_t>(sbo[0u].data()) % alignment) == 0u);
  RD_ASSERT_TRUE((reinterpret_cast<std::uintptr_t>(sbo[1u].data()) % alignment) == 0u);

  RD_ASSERT_NE(data, sbo[1].data());
}

RD_TEST_F(Any, NoSboAlignment) {
  constexpr auto alignment = alignof(over_aligned);
  rendu::basic_any<alignment> nosbo[2] = {over_aligned{}, over_aligned{}};
  const auto *data = nosbo[0].data();

  RD_ASSERT_TRUE((reinterpret_cast<std::uintptr_t>(nosbo[0u].data()) % alignment) == 0u);
  RD_ASSERT_TRUE((reinterpret_cast<std::uintptr_t>(nosbo[1u].data()) % alignment) == 0u);

  std::swap(nosbo[0], nosbo[1]);

  RD_ASSERT_TRUE((reinterpret_cast<std::uintptr_t>(nosbo[0u].data()) % alignment) == 0u);
  RD_ASSERT_TRUE((reinterpret_cast<std::uintptr_t>(nosbo[1u].data()) % alignment) == 0u);

  RD_ASSERT_EQ(data, nosbo[1].data());
}

RD_TEST_F(Any, AggregatesMustWork) {
  struct aggregate_type {
    int value;
  };

  // the goal of this test is to enforce the requirements for aggregate types
  rendu::any{std::in_place_type<aggregate_type>, 42}.emplace<aggregate_type>(42);
}

RD_TEST_F(Any, DeducedArrayType) {
  rendu::any any{"array of char"};

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.type(), rendu::type_id<const char *>());
  RD_ASSERT_EQ((strcmp("array of char", rendu::any_cast<const char *>(any))), 0);

  any = "another array of char";

  RD_ASSERT_TRUE(any);
  RD_ASSERT_EQ(any.type(), rendu::type_id<const char *>());
  RD_ASSERT_EQ((strcmp("another array of char", rendu::any_cast<const char *>(any))), 0);
}

