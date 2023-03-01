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

TEST_F(Any, SBO) {
  rendu::any any{'c'};

  ASSERT_TRUE(any);
  ASSERT_TRUE(any.owner());
  ASSERT_EQ(any.type(), rendu::type_id<char>());
  ASSERT_EQ(rendu::any_cast<double>(&any), nullptr);
  ASSERT_EQ(rendu::any_cast<char>(any), 'c');
}

TEST_F(Any, NoSBO) {
  fat instance{.1, .2, .3, .4};
  rendu::any any{instance};

  ASSERT_TRUE(any);
  ASSERT_TRUE(any.owner());
  ASSERT_EQ(any.type(), rendu::type_id<fat>());
  ASSERT_EQ(rendu::any_cast<double>(&any), nullptr);
  ASSERT_EQ(rendu::any_cast<fat>(any), instance);
}

TEST_F(Any, Empty) {
  rendu::any any{};

  ASSERT_FALSE(any);
  ASSERT_TRUE(any.owner());
  ASSERT_EQ(any.type(), rendu::type_id<void>());
  ASSERT_EQ(rendu::any_cast<double>(&any), nullptr);
  ASSERT_EQ(any.data(), nullptr);
}

TEST_F(Any, SBOInPlaceTypeConstruction) {
  rendu::any any{std::in_place_type<int>, 42};

  ASSERT_TRUE(any);
  ASSERT_TRUE(any.owner());
  ASSERT_EQ(any.type(), rendu::type_id<int>());
  ASSERT_EQ(rendu::any_cast<double>(&any), nullptr);
  ASSERT_EQ(rendu::any_cast<int>(any), 42);

  auto other = any.as_ref();

  ASSERT_TRUE(other);
  ASSERT_FALSE(other.owner());
  ASSERT_EQ(other.type(), rendu::type_id<int>());
  ASSERT_EQ(rendu::any_cast<int>(other), 42);
  ASSERT_EQ(other.data(), any.data());
}

TEST_F(Any, SBOAsRefConstruction) {
  int value = 42;
  rendu::any any{rendu::forward_as_any(value)};

  ASSERT_TRUE(any);
  ASSERT_FALSE(any.owner());
  ASSERT_EQ(any.type(), rendu::type_id<int>());

  ASSERT_EQ(rendu::any_cast<double>(&any), nullptr);
  ASSERT_EQ(rendu::any_cast<const int>(&any), &value);
  ASSERT_EQ(rendu::any_cast<int>(&any), &value);
  ASSERT_EQ(rendu::any_cast<const int>(&std::as_const(any)), &value);
  ASSERT_EQ(rendu::any_cast<int>(&std::as_const(any)), &value);

  ASSERT_EQ(rendu::any_cast<const int &>(any), 42);
  ASSERT_EQ(rendu::any_cast<int>(any), 42);

  ASSERT_EQ(any.data(), &value);
  ASSERT_EQ(std::as_const(any).data(), &value);

  any.emplace<int &>(value);

  ASSERT_TRUE(any);
  ASSERT_FALSE(any.owner());
  ASSERT_EQ(any.type(), rendu::type_id<int>());
  ASSERT_EQ(rendu::any_cast<int>(&any), &value);

  auto other = any.as_ref();

  ASSERT_TRUE(other);
  ASSERT_FALSE(other.owner());
  ASSERT_EQ(other.type(), rendu::type_id<int>());
  ASSERT_EQ(rendu::any_cast<int>(other), 42);
  ASSERT_EQ(other.data(), any.data());
}

TEST_F(Any, SBOAsConstRefConstruction) {
  const int value = 42;
  rendu::any any{rendu::forward_as_any(value)};

  ASSERT_TRUE(any);
  ASSERT_FALSE(any.owner());
  ASSERT_EQ(any.type(), rendu::type_id<int>());

  ASSERT_EQ(rendu::any_cast<double>(&any), nullptr);
  ASSERT_EQ(rendu::any_cast<const int>(&any), &value);
  ASSERT_EQ(rendu::any_cast<int>(&any), nullptr);
  ASSERT_EQ(rendu::any_cast<const int>(&std::as_const(any)), &value);
  ASSERT_EQ(rendu::any_cast<int>(&std::as_const(any)), &value);

  ASSERT_EQ(rendu::any_cast<const int &>(any), 42);
  ASSERT_EQ(rendu::any_cast<int>(any), 42);

  ASSERT_EQ(any.data(), nullptr);
  ASSERT_EQ(std::as_const(any).data(), &value);

  any.emplace<const int &>(value);

  ASSERT_TRUE(any);
  ASSERT_FALSE(any.owner());
  ASSERT_EQ(any.type(), rendu::type_id<int>());
  ASSERT_EQ(rendu::any_cast<const int>(&any), &value);

  auto other = any.as_ref();

  ASSERT_TRUE(other);
  ASSERT_FALSE(other.owner());
  ASSERT_EQ(other.type(), rendu::type_id<int>());
  ASSERT_EQ(rendu::any_cast<int>(other), 42);
  ASSERT_EQ(other.data(), any.data());
}

TEST_F(Any, SBOCopyConstruction) {
  rendu::any any{42};
  rendu::any other{any};

  ASSERT_TRUE(any);
  ASSERT_TRUE(other);
  ASSERT_TRUE(any.owner());
  ASSERT_TRUE(other.owner());
  ASSERT_EQ(any.type(), rendu::type_id<int>());
  ASSERT_EQ(other.type(), rendu::type_id<int>());
  ASSERT_EQ(rendu::any_cast<double>(&other), nullptr);
  ASSERT_EQ(rendu::any_cast<int>(other), 42);
}

TEST_F(Any, SBOCopyAssignment) {
  rendu::any any{42};
  rendu::any other{3};

  other = any;

  ASSERT_TRUE(any);
  ASSERT_TRUE(other);
  ASSERT_TRUE(any.owner());
  ASSERT_TRUE(other.owner());
  ASSERT_EQ(any.type(), rendu::type_id<int>());
  ASSERT_EQ(other.type(), rendu::type_id<int>());
  ASSERT_EQ(rendu::any_cast<double>(&other), nullptr);
  ASSERT_EQ(rendu::any_cast<int>(other), 42);
}

TEST_F(Any, SBOMoveConstruction) {
  rendu::any any{42};
  rendu::any other{std::move(any)};

  ASSERT_TRUE(any);
  ASSERT_TRUE(other);
  ASSERT_TRUE(any.owner());
  ASSERT_TRUE(other.owner());
  ASSERT_NE(any.data(), nullptr);
  ASSERT_EQ(any.type(), rendu::type_id<int>());
  ASSERT_EQ(other.type(), rendu::type_id<int>());
  ASSERT_EQ(rendu::any_cast<double>(&other), nullptr);
  ASSERT_EQ(rendu::any_cast<int>(other), 42);
}

TEST_F(Any, SBOMoveAssignment) {
  rendu::any any{42};
  rendu::any other{3};

  other = std::move(any);

  ASSERT_TRUE(any);
  ASSERT_TRUE(other);
  ASSERT_TRUE(any.owner());
  ASSERT_TRUE(other.owner());
  ASSERT_NE(any.data(), nullptr);
  ASSERT_EQ(any.type(), rendu::type_id<int>());
  ASSERT_EQ(other.type(), rendu::type_id<int>());
  ASSERT_EQ(rendu::any_cast<double>(&other), nullptr);
  ASSERT_EQ(rendu::any_cast<int>(other), 42);
}

TEST_F(Any, SBODirectAssignment) {
  rendu::any any{};
  any = 42;

  ASSERT_TRUE(any);
  ASSERT_TRUE(any.owner());
  ASSERT_EQ(any.type(), rendu::type_id<int>());
  ASSERT_EQ(rendu::any_cast<double>(&any), nullptr);
  ASSERT_EQ(rendu::any_cast<int>(any), 42);
}

TEST_F(Any, SBOAssignValue) {
  rendu::any any{42};
  rendu::any other{3};
  rendu::any invalid{'c'};

  ASSERT_TRUE(any);
  ASSERT_EQ(rendu::any_cast<int>(any), 42);

  ASSERT_TRUE(any.assign(other));
  ASSERT_FALSE(any.assign(invalid));
  ASSERT_EQ(rendu::any_cast<int>(any), 3);
}

TEST_F(Any, SBOAsRefAssignValue) {
  int value = 42;
  rendu::any any{rendu::forward_as_any(value)};
  rendu::any other{3};
  rendu::any invalid{'c'};

  ASSERT_TRUE(any);
  ASSERT_EQ(rendu::any_cast<int>(any), 42);

  ASSERT_TRUE(any.assign(other));
  ASSERT_FALSE(any.assign(invalid));
  ASSERT_EQ(rendu::any_cast<int>(any), 3);
  ASSERT_EQ(value, 3);
}

TEST_F(Any, SBOAsConstRefAssignValue) {
  const int value = 42;
  rendu::any any{rendu::forward_as_any(value)};
  rendu::any other{3};
  rendu::any invalid{'c'};

  ASSERT_TRUE(any);
  ASSERT_EQ(rendu::any_cast<int>(any), 42);

  ASSERT_FALSE(any.assign(other));
  ASSERT_FALSE(any.assign(invalid));
  ASSERT_EQ(rendu::any_cast<int>(any), 42);
  ASSERT_EQ(value, 42);
}

TEST_F(Any, SBOTransferValue) {
  rendu::any any{42};

  ASSERT_TRUE(any);
  ASSERT_EQ(rendu::any_cast<int>(any), 42);

  ASSERT_TRUE(any.assign(3));
  ASSERT_FALSE(any.assign('c'));
  ASSERT_EQ(rendu::any_cast<int>(any), 3);
}

TEST_F(Any, SBOTransferConstValue) {
  const int value = 3;
  rendu::any any{42};

  ASSERT_TRUE(any);
  ASSERT_EQ(rendu::any_cast<int>(any), 42);

  ASSERT_TRUE(any.assign(rendu::forward_as_any(value)));
  ASSERT_EQ(rendu::any_cast<int>(any), 3);
}

TEST_F(Any, SBOAsRefTransferValue) {
  int value = 42;
  rendu::any any{rendu::forward_as_any(value)};

  ASSERT_TRUE(any);
  ASSERT_EQ(rendu::any_cast<int>(any), 42);

  ASSERT_TRUE(any.assign(3));
  ASSERT_FALSE(any.assign('c'));
  ASSERT_EQ(rendu::any_cast<int>(any), 3);
  ASSERT_EQ(value, 3);
}

TEST_F(Any, SBOAsConstRefTransferValue) {
  const int value = 42;
  rendu::any any{rendu::forward_as_any(value)};

  ASSERT_TRUE(any);
  ASSERT_EQ(rendu::any_cast<int>(any), 42);

  ASSERT_FALSE(any.assign(3));
  ASSERT_FALSE(any.assign('c'));
  ASSERT_EQ(rendu::any_cast<int>(any), 42);
  ASSERT_EQ(value, 42);
}

TEST_F(Any, NoSBOInPlaceTypeConstruction) {
  fat instance{.1, .2, .3, .4};
  rendu::any any{std::in_place_type<fat>, instance};

  ASSERT_TRUE(any);
  ASSERT_TRUE(any.owner());
  ASSERT_EQ(any.type(), rendu::type_id<fat>());
  ASSERT_EQ(rendu::any_cast<double>(&any), nullptr);
  ASSERT_EQ(rendu::any_cast<fat>(any), instance);

  auto other = any.as_ref();

  ASSERT_TRUE(other);
  ASSERT_FALSE(other.owner());
  ASSERT_EQ(other.type(), rendu::type_id<fat>());
  ASSERT_EQ(rendu::any_cast<fat>(other), (fat{.1, .2, .3, .4}));
  ASSERT_EQ(other.data(), any.data());
}

TEST_F(Any, NoSBOAsRefConstruction) {
  fat instance{.1, .2, .3, .4};
  rendu::any any{rendu::forward_as_any(instance)};

  ASSERT_TRUE(any);
  ASSERT_FALSE(any.owner());
  ASSERT_EQ(any.type(), rendu::type_id<fat>());

  ASSERT_EQ(rendu::any_cast<double>(&any), nullptr);
  ASSERT_EQ(rendu::any_cast<const fat>(&any), &instance);
  ASSERT_EQ(rendu::any_cast<fat>(&any), &instance);
  ASSERT_EQ(rendu::any_cast<const fat>(&std::as_const(any)), &instance);
  ASSERT_EQ(rendu::any_cast<fat>(&std::as_const(any)), &instance);

  ASSERT_EQ(rendu::any_cast<const fat &>(any), instance);
  ASSERT_EQ(rendu::any_cast<fat>(any), instance);

  ASSERT_EQ(any.data(), &instance);
  ASSERT_EQ(std::as_const(any).data(), &instance);

  any.emplace<fat &>(instance);

  ASSERT_TRUE(any);
  ASSERT_FALSE(any.owner());
  ASSERT_EQ(any.type(), rendu::type_id<fat>());
  ASSERT_EQ(rendu::any_cast<fat>(&any), &instance);

  auto other = any.as_ref();

  ASSERT_TRUE(other);
  ASSERT_FALSE(other.owner());
  ASSERT_EQ(other.type(), rendu::type_id<fat>());
  ASSERT_EQ(rendu::any_cast<fat>(other), (fat{.1, .2, .3, .4}));
  ASSERT_EQ(other.data(), any.data());
}

TEST_F(Any, NoSBOAsConstRefConstruction) {
  const fat instance{.1, .2, .3, .4};
  rendu::any any{rendu::forward_as_any(instance)};

  ASSERT_TRUE(any);
  ASSERT_FALSE(any.owner());
  ASSERT_EQ(any.type(), rendu::type_id<fat>());

  ASSERT_EQ(rendu::any_cast<double>(&any), nullptr);
  ASSERT_EQ(rendu::any_cast<const fat>(&any), &instance);
  ASSERT_EQ(rendu::any_cast<fat>(&any), nullptr);
  ASSERT_EQ(rendu::any_cast<const fat>(&std::as_const(any)), &instance);
  ASSERT_EQ(rendu::any_cast<fat>(&std::as_const(any)), &instance);

  ASSERT_EQ(rendu::any_cast<const fat &>(any), instance);
  ASSERT_EQ(rendu::any_cast<fat>(any), instance);

  ASSERT_EQ(any.data(), nullptr);
  ASSERT_EQ(std::as_const(any).data(), &instance);

  any.emplace<const fat &>(instance);

  ASSERT_TRUE(any);
  ASSERT_FALSE(any.owner());
  ASSERT_EQ(any.type(), rendu::type_id<fat>());
  ASSERT_EQ(rendu::any_cast<const fat>(&any), &instance);

  auto other = any.as_ref();

  ASSERT_TRUE(other);
  ASSERT_FALSE(other.owner());
  ASSERT_EQ(other.type(), rendu::type_id<fat>());
  ASSERT_EQ(rendu::any_cast<fat>(other), (fat{.1, .2, .3, .4}));
  ASSERT_EQ(other.data(), any.data());
}

TEST_F(Any, NoSBOCopyConstruction) {
  fat instance{.1, .2, .3, .4};
  rendu::any any{instance};
  rendu::any other{any};

  ASSERT_TRUE(any);
  ASSERT_TRUE(other);
  ASSERT_TRUE(any.owner());
  ASSERT_TRUE(other.owner());
  ASSERT_EQ(any.type(), rendu::type_id<fat>());
  ASSERT_EQ(other.type(), rendu::type_id<fat>());
  ASSERT_EQ(rendu::any_cast<double>(&other), nullptr);
  ASSERT_EQ(rendu::any_cast<fat>(other), instance);
}

TEST_F(Any, NoSBOCopyAssignment) {
  fat instance{.1, .2, .3, .4};
  rendu::any any{instance};
  rendu::any other{3};

  other = any;

  ASSERT_TRUE(any);
  ASSERT_TRUE(other);
  ASSERT_TRUE(any.owner());
  ASSERT_TRUE(other.owner());
  ASSERT_EQ(any.type(), rendu::type_id<fat>());
  ASSERT_EQ(other.type(), rendu::type_id<fat>());
  ASSERT_EQ(rendu::any_cast<double>(&other), nullptr);
  ASSERT_EQ(rendu::any_cast<fat>(other), instance);
}

TEST_F(Any, NoSBOMoveConstruction) {
  fat instance{.1, .2, .3, .4};
  rendu::any any{instance};
  rendu::any other{std::move(any)};

  ASSERT_TRUE(any);
  ASSERT_TRUE(other);
  ASSERT_TRUE(any.owner());
  ASSERT_TRUE(other.owner());
  ASSERT_EQ(any.data(), nullptr);
  ASSERT_EQ(any.type(), rendu::type_id<fat>());
  ASSERT_EQ(other.type(), rendu::type_id<fat>());
  ASSERT_EQ(rendu::any_cast<double>(&other), nullptr);
  ASSERT_EQ(rendu::any_cast<fat>(other), instance);
}

TEST_F(Any, NoSBOMoveAssignment) {
  fat instance{.1, .2, .3, .4};
  rendu::any any{instance};
  rendu::any other{3};

  other = std::move(any);

  ASSERT_TRUE(any);
  ASSERT_TRUE(other);
  ASSERT_TRUE(any.owner());
  ASSERT_TRUE(other.owner());
  ASSERT_EQ(any.data(), nullptr);
  ASSERT_EQ(any.type(), rendu::type_id<fat>());
  ASSERT_EQ(other.type(), rendu::type_id<fat>());
  ASSERT_EQ(rendu::any_cast<double>(&other), nullptr);
  ASSERT_EQ(rendu::any_cast<fat>(other), instance);
}

TEST_F(Any, NoSBODirectAssignment) {
  fat instance{.1, .2, .3, .4};
  rendu::any any{};
  any = instance;

  ASSERT_TRUE(any);
  ASSERT_TRUE(any.owner());
  ASSERT_EQ(any.type(), rendu::type_id<fat>());
  ASSERT_EQ(rendu::any_cast<double>(&any), nullptr);
  ASSERT_EQ(rendu::any_cast<fat>(any), instance);
}

TEST_F(Any, NoSBOAssignValue) {
  rendu::any any{fat{.1, .2, .3, .4}};
  rendu::any other{fat{.0, .1, .2, .3}};
  rendu::any invalid{'c'};

  const void *addr = std::as_const(any).data();

  ASSERT_TRUE(any);
  ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.1, .2, .3, .4}));

  ASSERT_TRUE(any.assign(other));
  ASSERT_FALSE(any.assign(invalid));
  ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.0, .1, .2, .3}));
  ASSERT_EQ(addr, std::as_const(any).data());
}

TEST_F(Any, NoSBOAsRefAssignValue) {
  fat instance{.1, .2, .3, .4};
  rendu::any any{rendu::forward_as_any(instance)};
  rendu::any other{fat{.0, .1, .2, .3}};
  rendu::any invalid{'c'};

  ASSERT_TRUE(any);
  ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.1, .2, .3, .4}));

  ASSERT_TRUE(any.assign(other));
  ASSERT_FALSE(any.assign(invalid));
  ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.0, .1, .2, .3}));
  ASSERT_EQ(instance, (fat{.0, .1, .2, .3}));
}

TEST_F(Any, NoSBOAsConstRefAssignValue) {
  const fat instance{.1, .2, .3, .4};
  rendu::any any{rendu::forward_as_any(instance)};
  rendu::any other{fat{.0, .1, .2, .3}};
  rendu::any invalid{'c'};

  ASSERT_TRUE(any);
  ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.1, .2, .3, .4}));

  ASSERT_FALSE(any.assign(other));
  ASSERT_FALSE(any.assign(invalid));
  ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.1, .2, .3, .4}));
  ASSERT_EQ(instance, (fat{.1, .2, .3, .4}));
}

TEST_F(Any, NoSBOTransferValue) {
  rendu::any any{fat{.1, .2, .3, .4}};

  const void *addr = std::as_const(any).data();

  ASSERT_TRUE(any);
  ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.1, .2, .3, .4}));

  ASSERT_TRUE(any.assign(fat{.0, .1, .2, .3}));
  ASSERT_FALSE(any.assign('c'));
  ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.0, .1, .2, .3}));
  ASSERT_EQ(addr, std::as_const(any).data());
}

TEST_F(Any, NoSBOTransferConstValue) {
  const fat instance{.0, .1, .2, .3};
  rendu::any any{fat{.1, .2, .3, .4}};

  const void *addr = std::as_const(any).data();

  ASSERT_TRUE(any);
  ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.1, .2, .3, .4}));

  ASSERT_TRUE(any.assign(rendu::forward_as_any(instance)));
  ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.0, .1, .2, .3}));
  ASSERT_EQ(addr, std::as_const(any).data());
}

TEST_F(Any, NoSBOAsRefTransferValue) {
  fat instance{.1, .2, .3, .4};
  rendu::any any{rendu::forward_as_any(instance)};

  const void *addr = std::as_const(any).data();

  ASSERT_TRUE(any);
  ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.1, .2, .3, .4}));

  ASSERT_TRUE(any.assign(fat{.0, .1, .2, .3}));
  ASSERT_FALSE(any.assign('c'));
  ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.0, .1, .2, .3}));
  ASSERT_EQ(instance, (fat{.0, .1, .2, .3}));
  ASSERT_EQ(addr, std::as_const(any).data());
}

TEST_F(Any, NoSBOAsConstRefTransferValue) {
  const fat instance{.1, .2, .3, .4};
  rendu::any any{rendu::forward_as_any(instance)};

  const void *addr = std::as_const(any).data();

  ASSERT_TRUE(any);
  ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.1, .2, .3, .4}));

  ASSERT_FALSE(any.assign(fat{.0, .1, .2, .3}));
  ASSERT_FALSE(any.assign('c'));
  ASSERT_EQ(rendu::any_cast<const fat &>(any), (fat{.1, .2, .3, .4}));
  ASSERT_EQ(instance, (fat{.1, .2, .3, .4}));
  ASSERT_EQ(addr, std::as_const(any).data());
}

TEST_F(Any, VoidInPlaceTypeConstruction) {
  rendu::any any{std::in_place_type<void>};

  ASSERT_FALSE(any);
  ASSERT_TRUE(any.owner());
  ASSERT_EQ(any.type(), rendu::type_id<void>());
  ASSERT_EQ(rendu::any_cast<int>(&any), nullptr);
}

TEST_F(Any, VoidCopyConstruction) {
  rendu::any any{std::in_place_type<void>};
  rendu::any other{any};

  ASSERT_FALSE(any);
  ASSERT_FALSE(other);
  ASSERT_TRUE(any.owner());
  ASSERT_TRUE(other.owner());
  ASSERT_EQ(any.type(), rendu::type_id<void>());
  ASSERT_EQ(other.type(), rendu::type_id<void>());
  ASSERT_EQ(rendu::any_cast<int>(&any), nullptr);
  ASSERT_EQ(rendu::any_cast<double>(&other), nullptr);
}

TEST_F(Any, VoidCopyAssignment) {
  rendu::any any{std::in_place_type<void>};
  rendu::any other{42};

  other = any;

  ASSERT_FALSE(any);
  ASSERT_FALSE(other);
  ASSERT_TRUE(any.owner());
  ASSERT_TRUE(other.owner());
  ASSERT_EQ(any.type(), rendu::type_id<void>());
  ASSERT_EQ(other.type(), rendu::type_id<void>());
  ASSERT_EQ(rendu::any_cast<int>(&any), nullptr);
  ASSERT_EQ(rendu::any_cast<double>(&other), nullptr);
}

TEST_F(Any, VoidMoveConstruction) {
  rendu::any any{std::in_place_type<void>};
  rendu::any other{std::move(any)};

  ASSERT_FALSE(any);
  ASSERT_FALSE(other);
  ASSERT_TRUE(any.owner());
  ASSERT_TRUE(other.owner());
  ASSERT_EQ(any.type(), rendu::type_id<void>());
  ASSERT_EQ(other.type(), rendu::type_id<void>());
  ASSERT_EQ(rendu::any_cast<int>(&any), nullptr);
  ASSERT_EQ(rendu::any_cast<double>(&other), nullptr);
}

TEST_F(Any, VoidMoveAssignment) {
  rendu::any any{std::in_place_type<void>};
  rendu::any other{42};

  other = std::move(any);

  ASSERT_FALSE(any);
  ASSERT_FALSE(other);
  ASSERT_TRUE(any.owner());
  ASSERT_TRUE(other.owner());
  ASSERT_EQ(any.type(), rendu::type_id<void>());
  ASSERT_EQ(other.type(), rendu::type_id<void>());
  ASSERT_EQ(rendu::any_cast<int>(&any), nullptr);
  ASSERT_EQ(rendu::any_cast<double>(&other), nullptr);
}

TEST_F(Any, SBOMoveValidButUnspecifiedState) {
  rendu::any any{42};
  rendu::any other{std::move(any)};
  rendu::any valid = std::move(other);

  ASSERT_TRUE(any);
  ASSERT_TRUE(other);
  ASSERT_TRUE(valid);
}

TEST_F(Any, NoSBOMoveValidButUnspecifiedState) {
  fat instance{.1, .2, .3, .4};
  rendu::any any{instance};
  rendu::any other{std::move(any)};
  rendu::any valid = std::move(other);

  ASSERT_TRUE(any);
  ASSERT_TRUE(other);
  ASSERT_TRUE(valid);
}

TEST_F(Any, VoidMoveValidButUnspecifiedState) {
  rendu::any any{std::in_place_type<void>};
  rendu::any other{std::move(any)};
  rendu::any valid = std::move(other);

  ASSERT_FALSE(any);
  ASSERT_FALSE(other);
  ASSERT_FALSE(valid);
}

TEST_F(Any, SBODestruction) {
  {
    rendu::any any{std::in_place_type<empty>};
    any.emplace<empty>();
    any = empty{};
    rendu::any other{std::move(any)};
    any = std::move(other);
  }

  ASSERT_EQ(empty::counter, 6);
}

TEST_F(Any, NoSBODestruction) {
  {
    rendu::any any{std::in_place_type<fat>, 1., 2., 3., 4.};
    any.emplace<fat>(1., 2., 3., 4.);
    any = fat{1., 2., 3., 4.};
    rendu::any other{std::move(any)};
    any = std::move(other);
  }

  ASSERT_EQ(fat::counter, 4);
}

TEST_F(Any, VoidDestruction) {
  // just let asan tell us if everything is ok here
  [[maybe_unused]] rendu::any any{std::in_place_type<void>};
}

TEST_F(Any, Emplace) {
  rendu::any any{};
  any.emplace<int>(42);

  ASSERT_TRUE(any);
  ASSERT_TRUE(any.owner());
  ASSERT_EQ(any.type(), rendu::type_id<int>());
  ASSERT_EQ(rendu::any_cast<double>(&any), nullptr);
  ASSERT_EQ(rendu::any_cast<int>(any), 42);
}

TEST_F(Any, EmplaceVoid) {
  rendu::any any{};
  any.emplace<void>();

  ASSERT_FALSE(any);
  ASSERT_TRUE(any.owner());
  ASSERT_EQ(any.type(), rendu::type_id<void>());
}

TEST_F(Any, Reset) {
  rendu::any any{42};

  ASSERT_TRUE(any);
  ASSERT_TRUE(any.owner());
  ASSERT_EQ(any.type(), rendu::type_id<int>());

  any.reset();

  ASSERT_FALSE(any);
  ASSERT_TRUE(any.owner());
  ASSERT_EQ(any.type(), rendu::type_id<void>());

  int value = 42;
  any.emplace<int &>(value);

  ASSERT_TRUE(any);
  ASSERT_FALSE(any.owner());
  ASSERT_EQ(any.type(), rendu::type_id<int>());

  any.reset();

  ASSERT_FALSE(any);
  ASSERT_TRUE(any.owner());
  ASSERT_EQ(any.type(), rendu::type_id<void>());
}

TEST_F(Any, SBOSwap) {
  rendu::any lhs{'c'};
  rendu::any rhs{42};

  std::swap(lhs, rhs);

  ASSERT_TRUE(lhs.owner());
  ASSERT_TRUE(rhs.owner());

  ASSERT_EQ(lhs.type(), rendu::type_id<int>());
  ASSERT_EQ(rhs.type(), rendu::type_id<char>());
  ASSERT_EQ(rendu::any_cast<char>(&lhs), nullptr);
  ASSERT_EQ(rendu::any_cast<int>(&rhs), nullptr);
  ASSERT_EQ(rendu::any_cast<int>(lhs), 42);
  ASSERT_EQ(rendu::any_cast<char>(rhs), 'c');
}

TEST_F(Any, NoSBOSwap) {
  rendu::any lhs{fat{.1, .2, .3, .4}};
  rendu::any rhs{fat{.4, .3, .2, .1}};

  std::swap(lhs, rhs);

  ASSERT_TRUE(lhs.owner());
  ASSERT_TRUE(rhs.owner());

  ASSERT_EQ(rendu::any_cast<fat>(lhs), (fat{.4, .3, .2, .1}));
  ASSERT_EQ(rendu::any_cast<fat>(rhs), (fat{.1, .2, .3, .4}));
}

TEST_F(Any, VoidSwap) {
  rendu::any lhs{std::in_place_type<void>};
  rendu::any rhs{std::in_place_type<void>};
  const auto *pre = lhs.data();

  std::swap(lhs, rhs);

  ASSERT_TRUE(lhs.owner());
  ASSERT_TRUE(rhs.owner());

  ASSERT_EQ(pre, lhs.data());
}

TEST_F(Any, SBOWithNoSBOSwap) {
  rendu::any lhs{fat{.1, .2, .3, .4}};
  rendu::any rhs{'c'};

  std::swap(lhs, rhs);

  ASSERT_TRUE(lhs.owner());
  ASSERT_TRUE(rhs.owner());

  ASSERT_EQ(lhs.type(), rendu::type_id<char>());
  ASSERT_EQ(rhs.type(), rendu::type_id<fat>());
  ASSERT_EQ(rendu::any_cast<fat>(&lhs), nullptr);
  ASSERT_EQ(rendu::any_cast<char>(&rhs), nullptr);
  ASSERT_EQ(rendu::any_cast<char>(lhs), 'c');
  ASSERT_EQ(rendu::any_cast<fat>(rhs), (fat{.1, .2, .3, .4}));
}

TEST_F(Any, SBOWithRefSwap) {
  int value = 3;
  rendu::any lhs{rendu::forward_as_any(value)};
  rendu::any rhs{'c'};

  std::swap(lhs, rhs);

  ASSERT_TRUE(lhs.owner());
  ASSERT_FALSE(rhs.owner());

  ASSERT_EQ(lhs.type(), rendu::type_id<char>());
  ASSERT_EQ(rhs.type(), rendu::type_id<int>());
  ASSERT_EQ(rendu::any_cast<int>(&lhs), nullptr);
  ASSERT_EQ(rendu::any_cast<char>(&rhs), nullptr);
  ASSERT_EQ(rendu::any_cast<char>(lhs), 'c');
  ASSERT_EQ(rendu::any_cast<int>(rhs), 3);
  ASSERT_EQ(rhs.data(), &value);
}

TEST_F(Any, SBOWithConstRefSwap) {
  const int value = 3;
  rendu::any lhs{rendu::forward_as_any(value)};
  rendu::any rhs{'c'};

  std::swap(lhs, rhs);

  ASSERT_TRUE(lhs.owner());
  ASSERT_FALSE(rhs.owner());

  ASSERT_EQ(lhs.type(), rendu::type_id<char>());
  ASSERT_EQ(rhs.type(), rendu::type_id<int>());
  ASSERT_EQ(rendu::any_cast<int>(&lhs), nullptr);
  ASSERT_EQ(rendu::any_cast<char>(&rhs), nullptr);
  ASSERT_EQ(rendu::any_cast<char>(lhs), 'c');
  ASSERT_EQ(rendu::any_cast<int>(rhs), 3);
  ASSERT_EQ(rhs.data(), nullptr);
  ASSERT_EQ(std::as_const(rhs).data(), &value);
}

TEST_F(Any, SBOWithEmptySwap) {
  rendu::any lhs{'c'};
  rendu::any rhs{};

  std::swap(lhs, rhs);

  ASSERT_FALSE(lhs);
  ASSERT_TRUE(lhs.owner());
  ASSERT_EQ(rhs.type(), rendu::type_id<char>());
  ASSERT_EQ(rendu::any_cast<char>(&lhs), nullptr);
  ASSERT_EQ(rendu::any_cast<double>(&rhs), nullptr);
  ASSERT_EQ(rendu::any_cast<char>(rhs), 'c');

  std::swap(lhs, rhs);

  ASSERT_FALSE(rhs);
  ASSERT_TRUE(rhs.owner());
  ASSERT_EQ(lhs.type(), rendu::type_id<char>());
  ASSERT_EQ(rendu::any_cast<double>(&lhs), nullptr);
  ASSERT_EQ(rendu::any_cast<char>(&rhs), nullptr);
  ASSERT_EQ(rendu::any_cast<char>(lhs), 'c');
}

TEST_F(Any, SBOWithVoidSwap) {
  rendu::any lhs{'c'};
  rendu::any rhs{std::in_place_type<void>};

  std::swap(lhs, rhs);

  ASSERT_FALSE(lhs);
  ASSERT_TRUE(lhs.owner());
  ASSERT_EQ(rhs.type(), rendu::type_id<char>());
  ASSERT_EQ(rendu::any_cast<char>(&lhs), nullptr);
  ASSERT_EQ(rendu::any_cast<double>(&rhs), nullptr);
  ASSERT_EQ(rendu::any_cast<char>(rhs), 'c');

  std::swap(lhs, rhs);

  ASSERT_FALSE(rhs);
  ASSERT_TRUE(rhs.owner());
  ASSERT_EQ(lhs.type(), rendu::type_id<char>());
  ASSERT_EQ(rendu::any_cast<double>(&lhs), nullptr);
  ASSERT_EQ(rendu::any_cast<char>(&rhs), nullptr);
  ASSERT_EQ(rendu::any_cast<char>(lhs), 'c');
}

TEST_F(Any, NoSBOWithRefSwap) {
  int value = 3;
  rendu::any lhs{rendu::forward_as_any(value)};
  rendu::any rhs{fat{.1, .2, .3, .4}};

  std::swap(lhs, rhs);

  ASSERT_TRUE(lhs.owner());
  ASSERT_FALSE(rhs.owner());

  ASSERT_EQ(lhs.type(), rendu::type_id<fat>());
  ASSERT_EQ(rhs.type(), rendu::type_id<int>());
  ASSERT_EQ(rendu::any_cast<int>(&lhs), nullptr);
  ASSERT_EQ(rendu::any_cast<fat>(&rhs), nullptr);
  ASSERT_EQ(rendu::any_cast<fat>(lhs), (fat{.1, .2, .3, .4}));
  ASSERT_EQ(rendu::any_cast<int>(rhs), 3);
  ASSERT_EQ(rhs.data(), &value);
}

TEST_F(Any, NoSBOWithConstRefSwap) {
  const int value = 3;
  rendu::any lhs{rendu::forward_as_any(value)};
  rendu::any rhs{fat{.1, .2, .3, .4}};

  std::swap(lhs, rhs);

  ASSERT_TRUE(lhs.owner());
  ASSERT_FALSE(rhs.owner());

  ASSERT_EQ(lhs.type(), rendu::type_id<fat>());
  ASSERT_EQ(rhs.type(), rendu::type_id<int>());
  ASSERT_EQ(rendu::any_cast<int>(&lhs), nullptr);
  ASSERT_EQ(rendu::any_cast<fat>(&rhs), nullptr);
  ASSERT_EQ(rendu::any_cast<fat>(lhs), (fat{.1, .2, .3, .4}));
  ASSERT_EQ(rendu::any_cast<int>(rhs), 3);
  ASSERT_EQ(rhs.data(), nullptr);
  ASSERT_EQ(std::as_const(rhs).data(), &value);
}

TEST_F(Any, NoSBOWithEmptySwap) {
  rendu::any lhs{fat{.1, .2, .3, .4}};
  rendu::any rhs{};

  std::swap(lhs, rhs);

  ASSERT_FALSE(lhs);
  ASSERT_TRUE(lhs.owner());
  ASSERT_EQ(rhs.type(), rendu::type_id<fat>());
  ASSERT_EQ(rendu::any_cast<fat>(&lhs), nullptr);
  ASSERT_EQ(rendu::any_cast<double>(&rhs), nullptr);
  ASSERT_EQ(rendu::any_cast<fat>(rhs), (fat{.1, .2, .3, .4}));

  std::swap(lhs, rhs);

  ASSERT_FALSE(rhs);
  ASSERT_TRUE(rhs.owner());
  ASSERT_EQ(lhs.type(), rendu::type_id<fat>());
  ASSERT_EQ(rendu::any_cast<double>(&lhs), nullptr);
  ASSERT_EQ(rendu::any_cast<fat>(&rhs), nullptr);
  ASSERT_EQ(rendu::any_cast<fat>(lhs), (fat{.1, .2, .3, .4}));
}

TEST_F(Any, NoSBOWithVoidSwap) {
  rendu::any lhs{fat{.1, .2, .3, .4}};
  rendu::any rhs{std::in_place_type<void>};

  std::swap(lhs, rhs);

  ASSERT_FALSE(lhs);
  ASSERT_TRUE(lhs.owner());
  ASSERT_EQ(rhs.type(), rendu::type_id<fat>());
  ASSERT_EQ(rendu::any_cast<fat>(&lhs), nullptr);
  ASSERT_EQ(rendu::any_cast<double>(&rhs), nullptr);
  ASSERT_EQ(rendu::any_cast<fat>(rhs), (fat{.1, .2, .3, .4}));

  std::swap(lhs, rhs);

  ASSERT_FALSE(rhs);
  ASSERT_TRUE(rhs.owner());
  ASSERT_EQ(lhs.type(), rendu::type_id<fat>());
  ASSERT_EQ(rendu::any_cast<double>(&lhs), nullptr);
  ASSERT_EQ(rendu::any_cast<fat>(&rhs), nullptr);
  ASSERT_EQ(rendu::any_cast<fat>(lhs), (fat{.1, .2, .3, .4}));
}

TEST_F(Any, AsRef) {
  rendu::any any{42};
  auto ref = any.as_ref();
  auto cref = std::as_const(any).as_ref();

  ASSERT_FALSE(ref.owner());
  ASSERT_FALSE(cref.owner());

  ASSERT_EQ(rendu::any_cast<int>(&any), any.data());
  ASSERT_EQ(rendu::any_cast<int>(&ref), any.data());
  ASSERT_EQ(rendu::any_cast<int>(&cref), nullptr);

  ASSERT_EQ(rendu::any_cast<const int>(&any), any.data());
  ASSERT_EQ(rendu::any_cast<const int>(&ref), any.data());
  ASSERT_EQ(rendu::any_cast<const int>(&cref), any.data());

  ASSERT_EQ(rendu::any_cast<int>(any), 42);
  ASSERT_EQ(rendu::any_cast<int>(ref), 42);
  ASSERT_EQ(rendu::any_cast<int>(cref), 42);

  ASSERT_EQ(rendu::any_cast<const int>(any), 42);
  ASSERT_EQ(rendu::any_cast<const int>(ref), 42);
  ASSERT_EQ(rendu::any_cast<const int>(cref), 42);

  ASSERT_EQ(rendu::any_cast<int &>(any), 42);
  ASSERT_EQ(rendu::any_cast<const int &>(any), 42);
  ASSERT_EQ(rendu::any_cast<int &>(ref), 42);
  ASSERT_EQ(rendu::any_cast<const int &>(ref), 42);
  ASSERT_EQ(rendu::any_cast<int>(&cref), nullptr);
  ASSERT_EQ(rendu::any_cast<const int &>(cref), 42);

  rendu::any_cast<int &>(any) = 3;

  ASSERT_EQ(rendu::any_cast<int>(any), 3);
  ASSERT_EQ(rendu::any_cast<int>(ref), 3);
  ASSERT_EQ(rendu::any_cast<int>(cref), 3);

  std::swap(ref, cref);

  ASSERT_FALSE(ref.owner());
  ASSERT_FALSE(cref.owner());

  ASSERT_EQ(rendu::any_cast<int>(&ref), nullptr);
  ASSERT_EQ(rendu::any_cast<int>(&cref), any.data());

  ref = ref.as_ref();
  cref = std::as_const(cref).as_ref();

  ASSERT_FALSE(ref.owner());
  ASSERT_FALSE(cref.owner());

  ASSERT_EQ(rendu::any_cast<int>(&ref), nullptr);
  ASSERT_EQ(rendu::any_cast<int>(&cref), nullptr);
  ASSERT_EQ(rendu::any_cast<const int>(&ref), any.data());
  ASSERT_EQ(rendu::any_cast<const int>(&cref), any.data());

  ASSERT_EQ(rendu::any_cast<int>(&ref), nullptr);
  ASSERT_EQ(rendu::any_cast<int>(&cref), nullptr);

  ASSERT_EQ(rendu::any_cast<const int &>(ref), 3);
  ASSERT_EQ(rendu::any_cast<const int &>(cref), 3);

  ref = 42;
  cref = 42;

  ASSERT_TRUE(ref.owner());
  ASSERT_TRUE(cref.owner());

  ASSERT_NE(rendu::any_cast<int>(&ref), nullptr);
  ASSERT_NE(rendu::any_cast<int>(&cref), nullptr);
  ASSERT_EQ(rendu::any_cast<int &>(ref), 42);
  ASSERT_EQ(rendu::any_cast<int &>(cref), 42);
  ASSERT_EQ(rendu::any_cast<const int &>(ref), 42);
  ASSERT_EQ(rendu::any_cast<const int &>(cref), 42);
  ASSERT_NE(rendu::any_cast<int>(&ref), any.data());
  ASSERT_NE(rendu::any_cast<int>(&cref), any.data());
}

TEST_F(Any, Comparable) {
  auto test = [](rendu::any any, rendu::any other) {
    ASSERT_EQ(any, any);
    ASSERT_NE(other, any);
    ASSERT_NE(any, rendu::any{});

    ASSERT_TRUE(any == any);
    ASSERT_FALSE(other == any);
    ASSERT_TRUE(any != other);
    ASSERT_TRUE(rendu::any{} != any);
  };

  int value = 42;

  test('c', 'a');
  test(fat{.1, .2, .3, .4}, fat{.0, .1, .2, .3});
  test(rendu::forward_as_any(value), 3);
  test(3, rendu::make_any<const int &>(value));
  test('c', value);
}

TEST_F(Any, NotComparable) {
  auto test = [](const auto &instance) {
    auto any = rendu::forward_as_any(instance);

    ASSERT_EQ(any, any);
    ASSERT_NE(any, rendu::any{instance});
    ASSERT_NE(rendu::any{}, any);

    ASSERT_TRUE(any == any);
    ASSERT_FALSE(any == rendu::any{instance});
    ASSERT_TRUE(rendu::any{} != any);
  };

  test(not_comparable{});
  test(std::unordered_map < int, not_comparable > {});
  test(std::vector<not_comparable>{});
}

TEST_F(Any, CompareVoid) {
  rendu::any any{std::in_place_type<void>};

  ASSERT_EQ(any, any);
  ASSERT_EQ(any, rendu::any{std::in_place_type<void>});
  ASSERT_NE(rendu::any{'a'}, any);
  ASSERT_EQ(any, rendu::any{});

  ASSERT_TRUE(any == any);
  ASSERT_TRUE(any == rendu::any{std::in_place_type<void>});
  ASSERT_FALSE(rendu::any{'a'} == any);
  ASSERT_TRUE(any != rendu::any{'a'});
  ASSERT_FALSE(rendu::any{} != any);
}

TEST_F(Any, AnyCast) {
  rendu::any any{42};
  const auto &cany = any;

  ASSERT_EQ(rendu::any_cast<char>(&any), nullptr);
  ASSERT_EQ(rendu::any_cast<char>(&cany), nullptr);
  ASSERT_EQ(*rendu::any_cast<int>(&any), 42);
  ASSERT_EQ(*rendu::any_cast<int>(&cany), 42);
  ASSERT_EQ(rendu::any_cast<int &>(any), 42);
  ASSERT_EQ(rendu::any_cast<const int &>(cany), 42);

  auto instance = std::make_unique<double>(42.);
  rendu::any ref{rendu::forward_as_any(instance)};
  rendu::any cref{rendu::forward_as_any(std::as_const(*instance))};

  ASSERT_EQ(rendu::any_cast<double>(std::move(cref)), 42.);
  ASSERT_EQ(*rendu::any_cast<std::unique_ptr<double>>(std::move(ref)), 42.);
  ASSERT_EQ(rendu::any_cast<int>(rendu::any{42}), 42);
}

RD_DEBUG_TEST_F(AnyDeathTest, AnyCast) {
  rendu::any any{42};
  const auto &cany = any;

  ASSERT_DEATH([[maybe_unused]] auto &elem = rendu::any_cast<double &>(any), "");
  ASSERT_DEATH([[maybe_unused]] const auto &elem = rendu::any_cast<const double &>(cany), "");

  auto instance = std::make_unique<double>(42.);
  rendu::any ref{rendu::forward_as_any(instance)};
  rendu::any cref{rendu::forward_as_any(std::as_const(*instance))};

  ASSERT_DEATH([[maybe_unused]] auto elem = rendu::any_cast<std::unique_ptr<double>>(std::as_const(ref).as_ref()),
                  "");
  ASSERT_DEATH([[maybe_unused]] auto elem = rendu::any_cast<double>(rendu::any{42}), "");
}

TEST_F(Any, MakeAny) {
  int value = 42;
  auto any = rendu::make_any<int>(value);
  auto ext = rendu::make_any<int, sizeof(int), alignof(int)>(value);
  auto ref = rendu::make_any<int &>(value);

  ASSERT_TRUE(any);
  ASSERT_TRUE(ext);
  ASSERT_TRUE(ref);

  ASSERT_TRUE(any.owner());
  ASSERT_TRUE(ext.owner());
  ASSERT_FALSE(ref.owner());

  ASSERT_EQ(rendu::any_cast<const int &>(any), 42);
  ASSERT_EQ(rendu::any_cast<const int &>(ext), 42);
  ASSERT_EQ(rendu::any_cast<const int &>(ref), 42);

  ASSERT_EQ(decltype(any)::length, rendu::any::length);
  ASSERT_NE(decltype(ext)::length, rendu::any::length);
  ASSERT_EQ(decltype(ref)::length, rendu::any::length);

  ASSERT_NE(any.data(), &value);
  ASSERT_NE(ext.data(), &value);
  ASSERT_EQ(ref.data(), &value);
}

TEST_F(Any, ForwardAsAny) {
  int value = 42;
  auto ref = rendu::forward_as_any(value);
  auto cref = rendu::forward_as_any(std::as_const(value));
  auto any = rendu::forward_as_any(std::move(value));

  ASSERT_TRUE(any);
  ASSERT_TRUE(ref);
  ASSERT_TRUE(cref);

  ASSERT_TRUE(any.owner());
  ASSERT_FALSE(ref.owner());
  ASSERT_FALSE(cref.owner());

  ASSERT_NE(rendu::any_cast<int>(&any), nullptr);
  ASSERT_NE(rendu::any_cast<int>(&ref), nullptr);
  ASSERT_EQ(rendu::any_cast<int>(&cref), nullptr);

  ASSERT_EQ(rendu::any_cast<const int &>(any), 42);
  ASSERT_EQ(rendu::any_cast<const int &>(ref), 42);
  ASSERT_EQ(rendu::any_cast<const int &>(cref), 42);

  ASSERT_NE(any.data(), &value);
  ASSERT_EQ(ref.data(), &value);
}

TEST_F(Any, NotCopyableType) {
  const std::unique_ptr<int> value{};
  rendu::any any{std::in_place_type<std::unique_ptr<int>>};
  rendu::any other = rendu::forward_as_any(value);

  ASSERT_TRUE(any);
  ASSERT_TRUE(other);

  ASSERT_TRUE(any.owner());
  ASSERT_FALSE(other.owner());
  ASSERT_EQ(any.type(), other.type());

  ASSERT_FALSE(any.assign(other));
  ASSERT_FALSE(any.assign(std::move(other)));

  rendu::any copy{any};

  ASSERT_TRUE(any);
  ASSERT_FALSE(copy);

  ASSERT_TRUE(any.owner());
  ASSERT_TRUE(copy.owner());

  copy = any;

  ASSERT_TRUE(any);
  ASSERT_FALSE(copy);

  ASSERT_TRUE(any.owner());
  ASSERT_TRUE(copy.owner());
}

TEST_F(Any, NotCopyableValueType) {
  std::vector<rendu::any> vec{};
  vec.emplace_back(std::in_place_type<std::unique_ptr<int>>);
  vec.shrink_to_fit();

  ASSERT_EQ(vec.size(), 1u);
  ASSERT_EQ(vec.capacity(), 1u);
  ASSERT_TRUE(vec[0u]);

  // strong exception guarantee due to noexcept move ctor
  vec.emplace_back(std::in_place_type<std::unique_ptr<int>>);

  ASSERT_EQ(vec.size(), 2u);
  ASSERT_TRUE(vec[0u]);
  ASSERT_TRUE(vec[1u]);
}

TEST_F(Any, NotMovableType) {
  rendu::any any{std::in_place_type<not_movable>};
  rendu::any other{std::in_place_type<not_movable>};

  ASSERT_TRUE(any);
  ASSERT_TRUE(other);

  ASSERT_TRUE(any.owner());
  ASSERT_TRUE(other.owner());
  ASSERT_EQ(any.type(), other.type());

  ASSERT_TRUE(any.assign(other));
  ASSERT_TRUE(any.assign(std::move(other)));

  rendu::any copy{any};

  ASSERT_TRUE(any);
  ASSERT_TRUE(copy);

  ASSERT_TRUE(any.owner());
  ASSERT_TRUE(copy.owner());

  copy = any;

  ASSERT_TRUE(any);
  ASSERT_TRUE(copy);

  ASSERT_TRUE(any.owner());
  ASSERT_TRUE(copy.owner());
}

TEST_F(Any, Array) {
  rendu::any any{std::in_place_type<int[1]>};
  rendu::any copy{any};

  ASSERT_TRUE(any);
  ASSERT_FALSE(copy);

  ASSERT_EQ(any.type(), rendu::type_id<int[1]>());
  ASSERT_NE(rendu::any_cast<int[1]>(&any), nullptr);
  ASSERT_EQ(rendu::any_cast<int[2]>(&any), nullptr);
  ASSERT_EQ(rendu::any_cast<int *>(&any), nullptr);

  rendu::any_cast<int (&)[1]>(any)[0] = 42;

  ASSERT_EQ(rendu::any_cast<const int (&)[1]>(std::as_const(any))[0], 42);
}

TEST_F(Any, CopyMoveReference) {
  int value{};

  auto test = [&](auto &&ref) {
    value = 3;

    auto any = rendu::forward_as_any(ref);
    rendu::any move = std::move(any);
    rendu::any copy = move;

    ASSERT_TRUE(any);
    ASSERT_TRUE(move);
    ASSERT_TRUE(copy);

    ASSERT_FALSE(any.owner());
    ASSERT_FALSE(move.owner());
    ASSERT_TRUE(copy.owner());

    ASSERT_EQ(move.type(), rendu::type_id<int>());
    ASSERT_EQ(copy.type(), rendu::type_id<int>());

    ASSERT_EQ(std::as_const(move).data(), &value);
    ASSERT_NE(std::as_const(copy).data(), &value);

    ASSERT_EQ(rendu::any_cast<int>(move), 3);
    ASSERT_EQ(rendu::any_cast<int>(copy), 3);

    value = 42;

    ASSERT_EQ(rendu::any_cast<const int &>(move), 42);
    ASSERT_EQ(rendu::any_cast<const int &>(copy), 3);
  };

  test(value);
  test(std::as_const(value));
}

TEST_F(Any, SBOVsZeroedSBOSize) {
  rendu::any sbo{42};
  const auto *broken = sbo.data();
  rendu::any other = std::move(sbo);

  ASSERT_NE(broken, other.data());

  rendu::basic_any<0u> dyn{42};
  const auto *valid = dyn.data();
  rendu::basic_any<0u> same = std::move(dyn);

  ASSERT_EQ(valid, same.data());
}

TEST_F(Any, SboAlignment) {
  constexpr auto alignment = alignof(over_aligned);
  rendu::basic_any<alignment, alignment> sbo[2] = {over_aligned{}, over_aligned{}};
  const auto *data = sbo[0].data();

  ASSERT_TRUE((reinterpret_cast<std::uintptr_t>(sbo[0u].data()) % alignment) == 0u);
  ASSERT_TRUE((reinterpret_cast<std::uintptr_t>(sbo[1u].data()) % alignment) == 0u);

  std::swap(sbo[0], sbo[1]);

  ASSERT_TRUE((reinterpret_cast<std::uintptr_t>(sbo[0u].data()) % alignment) == 0u);
  ASSERT_TRUE((reinterpret_cast<std::uintptr_t>(sbo[1u].data()) % alignment) == 0u);

  ASSERT_NE(data, sbo[1].data());
}

TEST_F(Any, NoSboAlignment) {
  constexpr auto alignment = alignof(over_aligned);
  rendu::basic_any<alignment> nosbo[2] = {over_aligned{}, over_aligned{}};
  const auto *data = nosbo[0].data();

  ASSERT_TRUE((reinterpret_cast<std::uintptr_t>(nosbo[0u].data()) % alignment) == 0u);
  ASSERT_TRUE((reinterpret_cast<std::uintptr_t>(nosbo[1u].data()) % alignment) == 0u);

  std::swap(nosbo[0], nosbo[1]);

  ASSERT_TRUE((reinterpret_cast<std::uintptr_t>(nosbo[0u].data()) % alignment) == 0u);
  ASSERT_TRUE((reinterpret_cast<std::uintptr_t>(nosbo[1u].data()) % alignment) == 0u);

  ASSERT_EQ(data, nosbo[1].data());
}

TEST_F(Any, AggregatesMustWork) {
  struct aggregate_type {
    int value;
  };

  // the goal of this test is to enforce the requirements for aggregate types
  rendu::any{std::in_place_type<aggregate_type>, 42}.emplace<aggregate_type>(42);
}

TEST_F(Any, DeducedArrayType) {
  rendu::any any{"array of char"};

  ASSERT_TRUE(any);
  ASSERT_EQ(any.type(), rendu::type_id<const char *>());
  ASSERT_EQ((strcmp("array of char", rendu::any_cast<const char *>(any))), 0);

  any = "another array of char";

  ASSERT_TRUE(any);
  ASSERT_EQ(any.type(), rendu::type_id<const char *>());
  ASSERT_EQ((strcmp("another array of char", rendu::any_cast<const char *>(any))), 0);
}

