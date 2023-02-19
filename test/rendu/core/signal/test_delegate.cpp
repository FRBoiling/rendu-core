/*
* Created by boil on 2023/2/19.
*/

#include <test/rdtest.h>
#include <test/throwing_allocator.h>
#include <core/signal/delegate.h>

int delegate_function(const int &i) {
  return i * i;
}

int curried_by_ref(const int &i, int j) {
  return i + j;
}

int curried_by_ptr(const int *i, int j) {
  return (*i) * j;
}

int non_const_reference(int &i) {
  return i *= i;
}

int move_only_type(std::unique_ptr<int> ptr) {
  return *ptr;
}

struct delegate_functor {
  int operator()(int i) {
    return i + i;
  }

  int identity(int i) const {
    return i;
  }

  static const int static_value = 3;
  const int data_member = 42;
};

struct const_nonconst_noexcept {
  void f() {
    ++cnt;
  }

  void g() noexcept {
    ++cnt;
  }

  void h() const {
    ++cnt;
  }

  void i() const noexcept {
    ++cnt;
  }

  int u{};
  const int v{};
  mutable int cnt{0};
};

RD_TEST(Delegate, Functionalities) {
  rendu::delegate<int(int)> ff_del;
  rendu::delegate<int(int)> mf_del;
  rendu::delegate<int(int)> lf_del;
  delegate_functor functor;

  RD_ASSERT_FALSE(ff_del);
  RD_ASSERT_FALSE(mf_del);
  RD_ASSERT_EQ(ff_del, mf_del);

  ff_del.connect<&delegate_function>();
  mf_del.connect<&delegate_functor::operator()>(functor);
  lf_del.connect([](const void *ptr, int value) { return static_cast<const delegate_functor *>(ptr)->identity(value); }, &functor);

  RD_ASSERT_TRUE(ff_del);
  RD_ASSERT_TRUE(mf_del);
  RD_ASSERT_TRUE(lf_del);

  RD_ASSERT_EQ(ff_del(3), 9);
  RD_ASSERT_EQ(mf_del(3), 6);
  RD_ASSERT_EQ(lf_del(3), 3);

  ff_del.reset();

  RD_ASSERT_FALSE(ff_del);

  RD_ASSERT_EQ(ff_del, rendu::delegate<int(int)>{});
  RD_ASSERT_NE(mf_del, rendu::delegate<int(int)>{});
  RD_ASSERT_NE(lf_del, rendu::delegate<int(int)>{});

  RD_ASSERT_NE(ff_del, mf_del);
  RD_ASSERT_NE(ff_del, lf_del);
  RD_ASSERT_NE(mf_del, lf_del);

  mf_del.reset();

  RD_ASSERT_FALSE(ff_del);
  RD_ASSERT_FALSE(mf_del);
  RD_ASSERT_TRUE(lf_del);

  RD_ASSERT_EQ(ff_del, rendu::delegate<int(int)>{});
  RD_ASSERT_EQ(mf_del, rendu::delegate<int(int)>{});
  RD_ASSERT_NE(lf_del, rendu::delegate<int(int)>{});

  RD_ASSERT_EQ(ff_del, mf_del);
  RD_ASSERT_NE(ff_del, lf_del);
  RD_ASSERT_NE(mf_del, lf_del);
}

RD_DEBUG_TEST(DelegateDeathTest, InvokeEmpty) {
rendu::delegate<int(int)> del;

RD_ASSERT_FALSE(del);
RD_ASSERT_DEATH(del(42), "");
RD_ASSERT_DEATH(std::as_const(del)(42), "");
}

RD_TEST(Delegate, DataMembers) {
  rendu::delegate<double()> delegate;
  delegate_functor functor;

  delegate.connect<&delegate_functor::data_member>(functor);

  RD_ASSERT_EQ(delegate(), 42);
}

RD_TEST(Delegate, Comparison) {
  rendu::delegate<int(int)> lhs;
  rendu::delegate<int(int)> rhs;
  delegate_functor functor;
  delegate_functor other;
  const int value = 0;

  RD_ASSERT_EQ(lhs, rendu::delegate<int(int)>{});
  RD_ASSERT_FALSE(lhs != rhs);
  RD_ASSERT_TRUE(lhs == rhs);
  RD_ASSERT_EQ(lhs, rhs);

  lhs.connect<&delegate_function>();

  RD_ASSERT_EQ(lhs, rendu::delegate<int(int)>{rendu::connect_arg<&delegate_function>});
  RD_ASSERT_TRUE(lhs != rhs);
  RD_ASSERT_FALSE(lhs == rhs);
  RD_ASSERT_NE(lhs, rhs);

  rhs.connect<&delegate_function>();

  RD_ASSERT_EQ(rhs, rendu::delegate<int(int)>{rendu::connect_arg<&delegate_function>});
  RD_ASSERT_FALSE(lhs != rhs);
  RD_ASSERT_TRUE(lhs == rhs);
  RD_ASSERT_EQ(lhs, rhs);

  lhs.connect<&curried_by_ref>(value);

  RD_ASSERT_EQ(lhs, (rendu::delegate<int(int)>{rendu::connect_arg<&curried_by_ref>, value}));
  RD_ASSERT_TRUE(lhs != rhs);
  RD_ASSERT_FALSE(lhs == rhs);
  RD_ASSERT_NE(lhs, rhs);

  rhs.connect<&curried_by_ref>(value);

  RD_ASSERT_EQ(rhs, (rendu::delegate<int(int)>{rendu::connect_arg<&curried_by_ref>, value}));
  RD_ASSERT_FALSE(lhs != rhs);
  RD_ASSERT_TRUE(lhs == rhs);
  RD_ASSERT_EQ(lhs, rhs);

  lhs.connect<&curried_by_ptr>(&value);

  RD_ASSERT_EQ(lhs, (rendu::delegate<int(int)>{rendu::connect_arg<&curried_by_ptr>, &value}));
  RD_ASSERT_TRUE(lhs != rhs);
  RD_ASSERT_FALSE(lhs == rhs);
  RD_ASSERT_NE(lhs, rhs);

  rhs.connect<&curried_by_ptr>(&value);

  RD_ASSERT_EQ(rhs, (rendu::delegate<int(int)>{rendu::connect_arg<&curried_by_ptr>, &value}));
  RD_ASSERT_FALSE(lhs != rhs);
  RD_ASSERT_TRUE(lhs == rhs);
  RD_ASSERT_EQ(lhs, rhs);

  lhs.connect<&delegate_functor::operator()>(functor);

  RD_ASSERT_EQ(lhs, (rendu::delegate<int(int)>{rendu::connect_arg<&delegate_functor::operator()>, functor}));
  RD_ASSERT_TRUE(lhs != rhs);
  RD_ASSERT_FALSE(lhs == rhs);
  RD_ASSERT_NE(lhs, rhs);

  rhs.connect<&delegate_functor::operator()>(functor);

  RD_ASSERT_EQ(rhs, (rendu::delegate<int(int)>{rendu::connect_arg<&delegate_functor::operator()>, functor}));
  RD_ASSERT_EQ(lhs.target(), rhs.target());
  RD_ASSERT_EQ(lhs.data(), rhs.data());
  RD_ASSERT_FALSE(lhs != rhs);
  RD_ASSERT_TRUE(lhs == rhs);
  RD_ASSERT_EQ(lhs, rhs);

  lhs.connect<&delegate_functor::operator()>(other);

  RD_ASSERT_EQ(lhs, (rendu::delegate<int(int)>{rendu::connect_arg<&delegate_functor::operator()>, other}));
  RD_ASSERT_EQ(lhs.target(), rhs.target());
  RD_ASSERT_NE(lhs.data(), rhs.data());
  RD_ASSERT_TRUE(lhs != rhs);
  RD_ASSERT_FALSE(lhs == rhs);
  RD_ASSERT_NE(lhs, rhs);

  lhs.connect([](const void *ptr, int val) { return static_cast<const delegate_functor *>(ptr)->identity(val) * val; }, &functor);

  RD_ASSERT_NE(lhs, (rendu::delegate<int(int)>{[](const void *, int val) { return val + val; }, &functor}));
  RD_ASSERT_NE(lhs.target(), rhs.target());
  RD_ASSERT_EQ(lhs.data(), rhs.data());
  RD_ASSERT_TRUE(lhs != rhs);
  RD_ASSERT_FALSE(lhs == rhs);
  RD_ASSERT_NE(lhs, rhs);

  rhs.connect([](const void *ptr, int val) { return static_cast<const delegate_functor *>(ptr)->identity(val) + val; }, &functor);

  RD_ASSERT_NE(rhs, (rendu::delegate<int(int)>{[](const void *, int val) { return val * val; }, &functor}));
  RD_ASSERT_TRUE(lhs != rhs);
  RD_ASSERT_FALSE(lhs == rhs);
  RD_ASSERT_NE(lhs, rhs);

  lhs.reset();

  RD_ASSERT_EQ(lhs, (rendu::delegate<int(int)>{}));
  RD_ASSERT_TRUE(lhs != rhs);
  RD_ASSERT_FALSE(lhs == rhs);
  RD_ASSERT_NE(lhs, rhs);

  rhs.reset();

  RD_ASSERT_EQ(rhs, (rendu::delegate<int(int)>{}));
  RD_ASSERT_FALSE(lhs != rhs);
  RD_ASSERT_TRUE(lhs == rhs);
  RD_ASSERT_EQ(lhs, rhs);
}

RD_TEST(Delegate, ConstNonConstNoExcept) {
  rendu::delegate<void()> delegate;
  const_nonconst_noexcept functor;

  delegate.connect<&const_nonconst_noexcept::f>(functor);
  delegate();

  delegate.connect<&const_nonconst_noexcept::g>(functor);
  delegate();

  delegate.connect<&const_nonconst_noexcept::h>(functor);
  delegate();

  delegate.connect<&const_nonconst_noexcept::i>(functor);
  delegate();

  RD_ASSERT_EQ(functor.cnt, 4);
}

RD_TEST(Delegate, DeductionGuide) {
  const_nonconst_noexcept functor;
  int value = 0;

  rendu::delegate func{rendu::connect_arg<&delegate_function>};
  rendu::delegate curried_func_with_ref{rendu::connect_arg<&curried_by_ref>, value};
  rendu::delegate curried_func_with_const_ref{rendu::connect_arg<&curried_by_ref>, std::as_const(value)};
  rendu::delegate curried_func_with_ptr{rendu::connect_arg<&curried_by_ptr>, &value};
  rendu::delegate curried_func_with_const_ptr{rendu::connect_arg<&curried_by_ptr>, &std::as_const(value)};
  rendu::delegate member_func_f{rendu::connect_arg<&const_nonconst_noexcept::f>, functor};
  rendu::delegate member_func_g{rendu::connect_arg<&const_nonconst_noexcept::g>, functor};
  rendu::delegate member_func_h{rendu::connect_arg<&const_nonconst_noexcept::h>, &functor};
  rendu::delegate member_func_h_const{rendu::connect_arg<&const_nonconst_noexcept::h>, &std::as_const(functor)};
  rendu::delegate member_func_i{rendu::connect_arg<&const_nonconst_noexcept::i>, functor};
  rendu::delegate member_func_i_const{rendu::connect_arg<&const_nonconst_noexcept::i>, std::as_const(functor)};
  rendu::delegate data_member_u{rendu::connect_arg<&const_nonconst_noexcept::u>, functor};
  rendu::delegate data_member_v{rendu::connect_arg<&const_nonconst_noexcept::v>, &functor};
  rendu::delegate data_member_v_const{rendu::connect_arg<&const_nonconst_noexcept::v>, &std::as_const(functor)};
  rendu::delegate lambda{+[](const void *, int) { return 0; }};

  static_assert(std::is_same_v<typename decltype(func)::type, int(const int &)>);
  static_assert(std::is_same_v<typename decltype(curried_func_with_ref)::type, int(int)>);
  static_assert(std::is_same_v<typename decltype(curried_func_with_const_ref)::type, int(int)>);
  static_assert(std::is_same_v<typename decltype(curried_func_with_ptr)::type, int(int)>);
  static_assert(std::is_same_v<typename decltype(curried_func_with_const_ptr)::type, int(int)>);
  static_assert(std::is_same_v<typename decltype(member_func_f)::type, void()>);
  static_assert(std::is_same_v<typename decltype(member_func_g)::type, void()>);
  static_assert(std::is_same_v<typename decltype(member_func_h)::type, void()>);
  static_assert(std::is_same_v<typename decltype(member_func_h_const)::type, void()>);
  static_assert(std::is_same_v<typename decltype(member_func_i)::type, void()>);
  static_assert(std::is_same_v<typename decltype(member_func_i_const)::type, void()>);
  static_assert(std::is_same_v<typename decltype(data_member_u)::type, int()>);
  static_assert(std::is_same_v<typename decltype(data_member_v)::type, const int()>);
  static_assert(std::is_same_v<typename decltype(data_member_v_const)::type, const int()>);
  static_assert(std::is_same_v<typename decltype(lambda)::type, int(int)>);

  RD_ASSERT_TRUE(func);
  RD_ASSERT_TRUE(curried_func_with_ref);
  RD_ASSERT_TRUE(curried_func_with_const_ref);
  RD_ASSERT_TRUE(curried_func_with_ptr);
  RD_ASSERT_TRUE(curried_func_with_const_ptr);
  RD_ASSERT_TRUE(member_func_f);
  RD_ASSERT_TRUE(member_func_g);
  RD_ASSERT_TRUE(member_func_h);
  RD_ASSERT_TRUE(member_func_h_const);
  RD_ASSERT_TRUE(member_func_i);
  RD_ASSERT_TRUE(member_func_i_const);
  RD_ASSERT_TRUE(data_member_u);
  RD_ASSERT_TRUE(data_member_v);
  RD_ASSERT_TRUE(data_member_v_const);
  RD_ASSERT_TRUE(lambda);
}

RD_TEST(Delegate, ConstInstance) {
  rendu::delegate<int(int)> delegate;
  const delegate_functor functor;

  RD_ASSERT_FALSE(delegate);

  delegate.connect<&delegate_functor::identity>(functor);

  RD_ASSERT_TRUE(delegate);
  RD_ASSERT_EQ(delegate(3), 3);

  delegate.reset();

  RD_ASSERT_FALSE(delegate);
  RD_ASSERT_EQ(delegate, rendu::delegate<int(int)>{});
}

RD_TEST(Delegate, NonConstReference) {
  rendu::delegate<int(int &)> delegate;
  delegate.connect<&non_const_reference>();
  int value = 3;

  RD_ASSERT_EQ(delegate(value), value);
  RD_ASSERT_EQ(value, 9);
}

RD_TEST(Delegate, MoveOnlyType) {
  rendu::delegate<int(std::unique_ptr<int>)> delegate;
  auto ptr = std::make_unique<int>(3);
  delegate.connect<&move_only_type>();

  RD_ASSERT_EQ(delegate(std::move(ptr)), 3);
  RD_ASSERT_FALSE(ptr);
}

RD_TEST(Delegate, CurriedFunction) {
  rendu::delegate<int(int)> delegate;
  const auto value = 3;

  delegate.connect<&curried_by_ref>(value);

  RD_ASSERT_TRUE(delegate);
  RD_ASSERT_EQ(delegate(1), 4);

  delegate.connect<&curried_by_ptr>(&value);

  RD_ASSERT_TRUE(delegate);
  RD_ASSERT_EQ(delegate(2), 6);
}

RD_TEST(Delegate, Constructors) {
  delegate_functor functor;
  const auto value = 2;

  rendu::delegate<int(int)> empty{};
  rendu::delegate<int(int)> func{rendu::connect_arg<&delegate_function>};
  rendu::delegate<int(int)> ref{rendu::connect_arg<&curried_by_ref>, value};
  rendu::delegate<int(int)> ptr{rendu::connect_arg<&curried_by_ptr>, &value};
  rendu::delegate<int(int)> member{rendu::connect_arg<&delegate_functor::operator()>, functor};

  RD_ASSERT_FALSE(empty);

  RD_ASSERT_TRUE(func);
  RD_ASSERT_EQ(9, func(3));

  RD_ASSERT_TRUE(ref);
  RD_ASSERT_EQ(5, ref(3));

  RD_ASSERT_TRUE(ptr);
  RD_ASSERT_EQ(6, ptr(3));

  RD_ASSERT_TRUE(member);
  RD_ASSERT_EQ(6, member(3));
}

RD_TEST(Delegate, VoidVsNonVoidReturnType) {
  delegate_functor functor;

  rendu::delegate<void(int)> func{rendu::connect_arg<&delegate_function>};
  rendu::delegate<void(int)> member{rendu::connect_arg<&delegate_functor::operator()>, &functor};
  rendu::delegate<void(int)> cmember{rendu::connect_arg<&delegate_functor::identity>, &std::as_const(functor)};

  RD_ASSERT_TRUE(func);
  RD_ASSERT_TRUE(member);
  RD_ASSERT_TRUE(cmember);
}

RD_TEST(Delegate, UnboundDataMember) {
  rendu::delegate<int(const delegate_functor &)> delegate;
  delegate.connect<&delegate_functor::data_member>();
  delegate_functor functor;

  RD_ASSERT_EQ(delegate(functor), 42);
}

RD_TEST(Delegate, UnboundMemberFunction) {
  rendu::delegate<int(delegate_functor *, const int &i)> delegate;
  delegate.connect<&delegate_functor::operator()>();
  delegate_functor functor;

  RD_ASSERT_EQ(delegate(&functor, 3), 6);
}

RD_TEST(Delegate, TheLessTheBetter) {
  rendu::delegate<int(int, char)> bound;
  rendu::delegate<int(delegate_functor &, int, char)> unbound;
  delegate_functor functor;

  // int delegate_function(const int &);
  bound.connect<&delegate_function>();

  RD_ASSERT_EQ(bound(3, 'c'), 9);

  // int delegate_functor::operator()(int);
  bound.connect<&delegate_functor::operator()>(functor);

  RD_ASSERT_EQ(bound(3, 'c'), 6);

  // int delegate_functor::operator()(int);
  bound.connect<&delegate_functor::identity>(&functor);

  RD_ASSERT_EQ(bound(3, 'c'), 3);

  // int delegate_functor::operator()(int);
  unbound.connect<&delegate_functor::operator()>();

  RD_ASSERT_EQ(unbound(functor, 3, 'c'), 6);
}