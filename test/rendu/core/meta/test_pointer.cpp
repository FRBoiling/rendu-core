/*
* Created by boil on 2023/2/26.
*/

#include <test/rdtest.h>
#include <core/meta/resolve.h>
#include <core/meta/pointer.h>

template<typename Type>
struct wrapped_shared_ptr {
  wrapped_shared_ptr(Type init)
      : ptr{new Type{init}} {}

  Type &deref() const {
    return *ptr;
  }

 private:
  std::shared_ptr<Type> ptr;
};

struct self_ptr {
  using element_type = self_ptr;

  self_ptr(int v)
      : value{v} {}

  const self_ptr &operator*() const {
    return *this;
  }

  int value;
};

struct proxy_ptr {
  using element_type = proxy_ptr;

  proxy_ptr(int &v)
      : value{&v} {}

  proxy_ptr operator*() const {
    return *this;
  }

  int *value;
};

template<typename Type>
struct adl_wrapped_shared_ptr : wrapped_shared_ptr<Type> {};

template<typename Type>
struct spec_wrapped_shared_ptr : wrapped_shared_ptr<Type> {};

template<typename Type>
struct rendu::is_meta_pointer_like<adl_wrapped_shared_ptr<Type>> : std::true_type {};

template<typename Type>
struct rendu::is_meta_pointer_like<spec_wrapped_shared_ptr<Type>> : std::true_type {};

template<>
struct rendu::is_meta_pointer_like<self_ptr> : std::true_type {};

template<>
struct rendu::is_meta_pointer_like<proxy_ptr> : std::true_type {};

template<typename Type>
struct rendu::adl_meta_pointer_like<spec_wrapped_shared_ptr<Type>> {
  static decltype(auto) dereference(const spec_wrapped_shared_ptr<Type> &ptr) {
    return ptr.deref();
  }
};

template<typename Type>
Type &dereference_meta_pointer_like(const adl_wrapped_shared_ptr<Type> &ptr) {
  return ptr.deref();
}

int test_function() {
  return 42;
}

RD_TEST(MetaPointerLike, DereferenceOperatorInvalidType) {
  int value = 0;
  rendu::meta_any any{value};

  RD_ASSERT_FALSE(any.type().is_pointer());
  RD_ASSERT_FALSE(any.type().is_pointer_like());
  RD_ASSERT_EQ(any.type(), rendu::resolve<int>());

  auto deref = *any;

  RD_ASSERT_FALSE(deref);
}

RD_TEST(MetaPointerLike, DereferenceOperatorConstType) {
  const int value = 42;
  rendu::meta_any any{&value};

  RD_ASSERT_TRUE(any.type().is_pointer());
  RD_ASSERT_TRUE(any.type().is_pointer_like());
  RD_ASSERT_EQ(any.type(), rendu::resolve<const int *>());

  auto deref = *any;

  RD_ASSERT_TRUE(deref);
  RD_ASSERT_FALSE(deref.type().is_pointer());
  RD_ASSERT_FALSE(deref.type().is_pointer_like());
  RD_ASSERT_EQ(deref.type(), rendu::resolve<int>());

  RD_ASSERT_EQ(deref.try_cast<int>(), nullptr);
  RD_ASSERT_EQ(deref.try_cast<const int>(), &value);
  RD_ASSERT_EQ(deref.cast<const int &>(), 42);
}

RD_TEST(MetaPointerLike, DereferenceOperatorConstAnyNonConstType) {
  int value = 42;
  const rendu::meta_any any{&value};
  auto deref = *any;

  RD_ASSERT_TRUE(deref);
  RD_ASSERT_FALSE(deref.type().is_pointer());
  RD_ASSERT_FALSE(deref.type().is_pointer_like());
  RD_ASSERT_EQ(deref.type(), rendu::resolve<int>());

  RD_ASSERT_NE(deref.try_cast<int>(), nullptr);
  RD_ASSERT_NE(deref.try_cast<const int>(), nullptr);
  RD_ASSERT_EQ(deref.cast<int &>(), 42);
  RD_ASSERT_EQ(deref.cast<const int &>(), 42);
}

RD_TEST(MetaPointerLike, DereferenceOperatorConstAnyConstType) {
  const int value = 42;
  const rendu::meta_any any{&value};
  auto deref = *any;

  RD_ASSERT_TRUE(deref);
  RD_ASSERT_FALSE(deref.type().is_pointer());
  RD_ASSERT_FALSE(deref.type().is_pointer_like());
  RD_ASSERT_EQ(deref.type(), rendu::resolve<int>());

  RD_ASSERT_EQ(deref.try_cast<int>(), nullptr);
  RD_ASSERT_NE(deref.try_cast<const int>(), nullptr);
  RD_ASSERT_EQ(deref.cast<const int &>(), 42);
}

RD_TEST(MetaPointerLike, DereferenceOperatorRawPointer) {
  int value = 0;
  rendu::meta_any any{&value};

  RD_ASSERT_TRUE(any.type().is_pointer());
  RD_ASSERT_TRUE(any.type().is_pointer_like());
  RD_ASSERT_EQ(any.type(), rendu::resolve<int *>());

  auto deref = *any;

  RD_ASSERT_TRUE(deref);
  RD_ASSERT_FALSE(deref.type().is_pointer());
  RD_ASSERT_FALSE(deref.type().is_pointer_like());
  RD_ASSERT_EQ(deref.type(), rendu::resolve<int>());

  deref.cast<int &>() = 42;

  RD_ASSERT_EQ(*any.cast<int *>(), 42);
  RD_ASSERT_EQ(value, 42);
}

RD_TEST(MetaPointerLike, DereferenceOperatorSmartPointer) {
  auto value = std::make_shared<int>(0);
  rendu::meta_any any{value};

  RD_ASSERT_FALSE(any.type().is_pointer());
  RD_ASSERT_TRUE(any.type().is_pointer_like());
  RD_ASSERT_EQ(any.type(), rendu::resolve<std::shared_ptr<int>>());

  auto deref = *any;

  RD_ASSERT_TRUE(deref);
  RD_ASSERT_FALSE(deref.type().is_pointer());
  RD_ASSERT_FALSE(deref.type().is_pointer_like());
  RD_ASSERT_EQ(deref.type(), rendu::resolve<int>());

  deref.cast<int &>() = 42;

  RD_ASSERT_EQ(*any.cast<std::shared_ptr<int>>(), 42);
  RD_ASSERT_EQ(*value, 42);
}

RD_TEST(MetaPointerLike, PointerToConstMoveOnlyType) {
  const std::unique_ptr<int> instance;
  rendu::meta_any any{&instance};
  auto deref = *any;

  RD_ASSERT_TRUE(any);
  RD_ASSERT_TRUE(deref);

  RD_ASSERT_EQ(deref.try_cast<std::unique_ptr<int>>(), nullptr);
  RD_ASSERT_NE(deref.try_cast<const std::unique_ptr<int>>(), nullptr);
  RD_ASSERT_EQ(&deref.cast<const std::unique_ptr<int> &>(), &instance);
}

RD_TEST(MetaPointerLike, AsRef) {
  int value = 0;
  int *ptr = &value;
  rendu::meta_any any{rendu::forward_as_meta(ptr)};

  RD_ASSERT_TRUE(any.type().is_pointer());
  RD_ASSERT_TRUE(any.type().is_pointer_like());
  RD_ASSERT_EQ(any.type(), rendu::resolve<int *>());

  auto deref = *any;

  RD_ASSERT_TRUE(deref);
  RD_ASSERT_FALSE(deref.type().is_pointer());
  RD_ASSERT_FALSE(deref.type().is_pointer_like());
  RD_ASSERT_EQ(deref.type(), rendu::resolve<int>());

  deref.cast<int &>() = 42;

  RD_ASSERT_EQ(*any.cast<int *>(), 42);
  RD_ASSERT_EQ(value, 42);
}

RD_TEST(MetaPointerLike, AsConstRef) {
  int value = 42;
  int *const ptr = &value;
  rendu::meta_any any{rendu::forward_as_meta(ptr)};

  RD_ASSERT_TRUE(any.type().is_pointer());
  RD_ASSERT_TRUE(any.type().is_pointer_like());
  RD_ASSERT_EQ(any.type(), rendu::resolve<int *>());

  auto deref = *any;

  RD_ASSERT_TRUE(deref);
  RD_ASSERT_FALSE(deref.type().is_pointer());
  RD_ASSERT_FALSE(deref.type().is_pointer_like());
  RD_ASSERT_EQ(deref.type(), rendu::resolve<int>());

  deref.cast<int &>() = 42;

  RD_ASSERT_EQ(*any.cast<int *>(), 42);
  RD_ASSERT_EQ(value, 42);
}

RD_TEST(MetaPointerLike, DereferenceOverload) {
  auto test = [](rendu::meta_any any) {
    RD_ASSERT_FALSE(any.type().is_pointer());
    RD_ASSERT_TRUE(any.type().is_pointer_like());

    auto deref = *any;

    RD_ASSERT_TRUE(deref);
    RD_ASSERT_FALSE(deref.type().is_pointer());
    RD_ASSERT_FALSE(deref.type().is_pointer_like());
    RD_ASSERT_EQ(deref.type(), rendu::resolve<int>());

    RD_ASSERT_EQ(deref.cast<int &>(), 42);
    RD_ASSERT_EQ(deref.cast<const int &>(), 42);
  };

  test(adl_wrapped_shared_ptr<int>{42});
  test(spec_wrapped_shared_ptr<int>{42});
}

RD_TEST(MetaPointerLike, DereferencePointerToConstOverload) {
  auto test = [](rendu::meta_any any) {
    RD_ASSERT_FALSE(any.type().is_pointer());
    RD_ASSERT_TRUE(any.type().is_pointer_like());

    auto deref = *any;

    RD_ASSERT_TRUE(deref);
    RD_ASSERT_FALSE(deref.type().is_pointer());
    RD_ASSERT_FALSE(deref.type().is_pointer_like());
    RD_ASSERT_EQ(deref.type(), rendu::resolve<int>());
    RD_ASSERT_EQ(deref.cast<const int &>(), 42);
  };

  test(adl_wrapped_shared_ptr<const int>{42});
  test(spec_wrapped_shared_ptr<const int>{42});
}

RD_TEST(MetaPointerLike, DereferencePointerToVoid) {
  auto test = [](rendu::meta_any any) {
    RD_ASSERT_TRUE(any.type().is_pointer());
    RD_ASSERT_TRUE(any.type().is_pointer_like());

    auto deref = *any;

    RD_ASSERT_FALSE(deref);
  };

  test(static_cast<void *>(nullptr));
  test(static_cast<const void *>(nullptr));
}

RD_TEST(MetaPointerLike, DereferenceSmartPointerToVoid) {
  auto test = [](rendu::meta_any any) {
    RD_ASSERT_TRUE(any.type().is_class());
    RD_ASSERT_FALSE(any.type().is_pointer());
    RD_ASSERT_TRUE(any.type().is_pointer_like());

    auto deref = *any;

    RD_ASSERT_FALSE(deref);
  };

  test(std::shared_ptr<void>{});
  test(std::unique_ptr<void, void (*)(void *)>{nullptr, nullptr});
}

RD_TEST(MetaPointerLike, DereferencePointerToFunction) {
  auto test = [](rendu::meta_any any) {
    RD_ASSERT_TRUE(any.type().is_pointer());
    RD_ASSERT_TRUE(any.type().is_pointer_like());
    RD_ASSERT_NE(any.try_cast < int(*)() > (), nullptr);
    RD_ASSERT_EQ(any.cast < int(*)() > ()(), 42);
  };

  rendu::meta_any func{&test_function};

  test(func);
  test(*func);
  test(**func);
  test(*std::as_const(func));
}

RD_TEST(MetaPointerLike, DereferenceSelfPointer) {
  self_ptr obj{42};
  rendu::meta_any any{rendu::forward_as_meta(obj)};
  rendu::meta_any deref = *any;

  RD_ASSERT_TRUE(deref);
  RD_ASSERT_TRUE(any.type().is_pointer_like());
  RD_ASSERT_EQ(deref.cast<const self_ptr &>().value, obj.value);
  RD_ASSERT_FALSE(deref.try_cast<self_ptr>());
}

RD_TEST(MetaPointerLike, DereferenceProxyPointer) {
  int value = 3;
  proxy_ptr obj{value};
  rendu::meta_any any{obj};
  rendu::meta_any deref = *any;

  RD_ASSERT_TRUE(deref);
  RD_ASSERT_TRUE(any.type().is_pointer_like());
  RD_ASSERT_EQ(*deref.cast<const proxy_ptr &>().value, value);
  RD_ASSERT_TRUE(deref.try_cast<proxy_ptr>());

  *deref.cast<proxy_ptr &>().value = 42;

  RD_ASSERT_EQ(value, 42);
}

RD_TEST(MetaPointerLike, DereferenceArray) {
  rendu::meta_any array{std::in_place_type<int[3]>};
  rendu::meta_any array_of_array{std::in_place_type<int[3][3]>};

  RD_ASSERT_EQ(array.type(), rendu::resolve<int[3]>());
  RD_ASSERT_EQ(array_of_array.type(), rendu::resolve<int[3][3]>());

  RD_ASSERT_FALSE(*array);
  RD_ASSERT_FALSE(*array_of_array);
}

RD_TEST(MetaPointerLike, DereferenceVerifiableNullPointerLike) {
  auto test = [](rendu::meta_any any) {
    RD_ASSERT_TRUE(any);
    RD_ASSERT_FALSE(*any);
  };

  test(rendu::meta_any{static_cast<int *>(nullptr)});
  test(rendu::meta_any{std::shared_ptr<int>{}});
  test(rendu::meta_any{std::unique_ptr<int>{}});
}

RD_DEBUG_TEST(MetaPointerLikeDeathTest, DereferenceOperatorConstType) {
  const int value = 42;
  rendu::meta_any any{&value};
  auto deref = *any;

  RD_ASSERT_TRUE(deref);
  RD_ASSERT_DEATH(deref.cast<int &>() = 0, "");
}

RD_DEBUG_TEST(MetaPointerLikeDeathTest, DereferenceOperatorConstAnyConstType) {
  const int value = 42;
  const rendu::meta_any any{&value};
  auto deref = *any;

  RD_ASSERT_TRUE(deref);
  RD_ASSERT_DEATH(deref.cast<int &>() = 0, "");
}

RD_DEBUG_TEST(MetaPointerLikeDeathTest, DereferencePointerToConstOverload) {
  auto test = [](rendu::meta_any any) {
    auto deref = *any;

    RD_ASSERT_TRUE(deref);
    RD_ASSERT_DEATH(deref.cast<int &>() = 42, "");
  };

  test(adl_wrapped_shared_ptr<const int>{42});
  test(spec_wrapped_shared_ptr<const int>{42});
}
