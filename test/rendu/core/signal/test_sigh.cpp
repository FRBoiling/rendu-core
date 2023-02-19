/*
* Created by boil on 2023/2/19.
*/
#include <test/rdtest.h>
#include <core/signal/sigh.h>

struct sigh_listener {
  static void f(int &v) {
    v = 42;
  }

  bool g(int) {
    k = !k;
    return true;
  }

  bool h(const int &) {
    return k;
  }

  // useless definition just because msvc does weird things if both are empty
  void l() {
    k = true && k;
  }

  bool k{false};
};

struct before_after {
  void add(int v) {
    value += v;
  }

  void mul(int v) {
    value *= v;
  }

  static void static_add(int v) {
    before_after::value += v;
  }

  static void static_mul(before_after &instance, int v) {
    instance.value *= v;
  }

  static inline int value{};
};

struct SigH: ::testing::Test {
  void SetUp() override {
    before_after::value = 0;
  }
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

  mutable int cnt{0};
};

RD_TEST_F(SigH, Lifetime) {
  using signal = rendu::sigh<void(void)>;

  RD_ASSERT_NO_FATAL_FAILURE(signal{});

  signal src{}, other{};

  RD_ASSERT_NO_FATAL_FAILURE(signal{src});
  RD_ASSERT_NO_FATAL_FAILURE(signal{std::move(other)});
  RD_ASSERT_NO_FATAL_FAILURE(src = other);
  RD_ASSERT_NO_FATAL_FAILURE(src = std::move(other));

  RD_ASSERT_NO_FATAL_FAILURE(delete new signal{});
}

RD_TEST_F(SigH, Clear) {
  rendu::sigh<void(int &)> sigh;
  rendu::sink sink{sigh};

  sink.connect<&sigh_listener::f>();

  RD_ASSERT_FALSE(sink.empty());
  RD_ASSERT_FALSE(sigh.empty());

  sink.disconnect(static_cast<const void *>(nullptr));

  RD_ASSERT_FALSE(sink.empty());
  RD_ASSERT_FALSE(sigh.empty());

  sink.disconnect();

  RD_ASSERT_TRUE(sink.empty());
  RD_ASSERT_TRUE(sigh.empty());
}

RD_TEST_F(SigH, Swap) {
  rendu::sigh<void(int &)> sigh1;
  rendu::sigh<void(int &)> sigh2;
  rendu::sink sink1{sigh1};
  rendu::sink sink2{sigh2};

  sink1.connect<&sigh_listener::f>();

  RD_ASSERT_FALSE(sink1.empty());
  RD_ASSERT_TRUE(sink2.empty());

  RD_ASSERT_FALSE(sigh1.empty());
  RD_ASSERT_TRUE(sigh2.empty());

  sigh1.swap(sigh2);

  RD_ASSERT_TRUE(sink1.empty());
  RD_ASSERT_FALSE(sink2.empty());

  RD_ASSERT_TRUE(sigh1.empty());
  RD_ASSERT_FALSE(sigh2.empty());
}

RD_TEST_F(SigH, Functions) {
  rendu::sigh<void(int &)> sigh;
  rendu::sink sink{sigh};
  int v = 0;

  sink.connect<&sigh_listener::f>();
  sigh.publish(v);

  RD_ASSERT_FALSE(sigh.empty());
  RD_ASSERT_EQ(1u, sigh.size());
  RD_ASSERT_EQ(42, v);

  v = 0;
  sink.disconnect<&sigh_listener::f>();
  sigh.publish(v);

  RD_ASSERT_TRUE(sigh.empty());
  RD_ASSERT_EQ(0u, sigh.size());
  RD_ASSERT_EQ(v, 0);
}

RD_TEST_F(SigH, FunctionsWithPayload) {
  rendu::sigh<void()> sigh;
  rendu::sink sink{sigh};
  int v = 0;

  sink.connect<&sigh_listener::f>(v);
  sigh.publish();

  RD_ASSERT_FALSE(sigh.empty());
  RD_ASSERT_EQ(1u, sigh.size());
  RD_ASSERT_EQ(42, v);

  v = 0;
  sink.disconnect<&sigh_listener::f>(v);
  sigh.publish();

  RD_ASSERT_TRUE(sigh.empty());
  RD_ASSERT_EQ(0u, sigh.size());
  RD_ASSERT_EQ(v, 0);

  sink.connect<&sigh_listener::f>(v);
  sink.disconnect(v);
  sigh.publish();

  RD_ASSERT_EQ(v, 0);
}

RD_TEST_F(SigH, Members) {
  sigh_listener l1, l2;
  rendu::sigh<bool(int)> sigh;
  rendu::sink sink{sigh};

  sink.connect<&sigh_listener::g>(l1);
  sigh.publish(42);

  RD_ASSERT_TRUE(l1.k);
  RD_ASSERT_FALSE(sigh.empty());
  RD_ASSERT_EQ(1u, sigh.size());

  sink.disconnect<&sigh_listener::g>(l1);
  sigh.publish(42);

  RD_ASSERT_TRUE(l1.k);
  RD_ASSERT_TRUE(sigh.empty());
  RD_ASSERT_EQ(0u, sigh.size());

  sink.connect<&sigh_listener::g>(&l1);
  sink.connect<&sigh_listener::h>(l2);

  RD_ASSERT_FALSE(sigh.empty());
  RD_ASSERT_EQ(2u, sigh.size());

  sink.disconnect(static_cast<const void *>(nullptr));

  RD_ASSERT_FALSE(sigh.empty());
  RD_ASSERT_EQ(2u, sigh.size());

  sink.disconnect(&l1);

  RD_ASSERT_FALSE(sigh.empty());
  RD_ASSERT_EQ(1u, sigh.size());
}

RD_TEST_F(SigH, Collector) {
  sigh_listener listener;
  rendu::sigh<bool(int)> sigh;
  rendu::sink sink{sigh};
  int cnt = 0;

  sink.connect<&sigh_listener::g>(&listener);
  sink.connect<&sigh_listener::h>(listener);

  auto no_return = [&listener, &cnt](bool value) {
    RD_ASSERT_TRUE(value);
    listener.k = true;
    ++cnt;
  };

  listener.k = true;
  sigh.collect(std::move(no_return), 42);

  RD_ASSERT_FALSE(sigh.empty());
  RD_ASSERT_EQ(cnt, 2);

  auto bool_return = [&cnt](bool value) {
    // gtest and its macro hell are sometimes really annoying...
    [](auto v) { RD_ASSERT_TRUE(v); }(value);
    ++cnt;
    return true;
  };

  cnt = 0;
  sigh.collect(std::move(bool_return), 42);

  RD_ASSERT_EQ(cnt, 1);
}

RD_TEST_F(SigH, CollectorVoid) {
  sigh_listener listener;
  rendu::sigh<void(int)> sigh;
  rendu::sink sink{sigh};
  int cnt = 0;

  sink.connect<&sigh_listener::g>(&listener);
  sink.connect<&sigh_listener::h>(listener);
  sigh.collect([&cnt]() { ++cnt; }, 42);

  RD_ASSERT_FALSE(sigh.empty());
  RD_ASSERT_EQ(cnt, 2);

  auto test = [&cnt]() {
    ++cnt;
    return true;
  };

  cnt = 0;
  sigh.collect(std::move(test), 42);

  RD_ASSERT_EQ(cnt, 1);
}

RD_TEST_F(SigH, Connection) {
  rendu::sigh<void(int &)> sigh;
  rendu::sink sink{sigh};
  int v = 0;

  auto conn = sink.connect<&sigh_listener::f>();
  sigh.publish(v);

  RD_ASSERT_FALSE(sigh.empty());
  RD_ASSERT_TRUE(conn);
  RD_ASSERT_EQ(42, v);

  v = 0;
  conn.release();
  sigh.publish(v);

  RD_ASSERT_TRUE(sigh.empty());
  RD_ASSERT_FALSE(conn);
  RD_ASSERT_EQ(0, v);
}

RD_TEST_F(SigH, ScopedConnection) {
  sigh_listener listener;
  rendu::sigh<void(int)> sigh;
  rendu::sink sink{sigh};

  {
    RD_ASSERT_FALSE(listener.k);

    rendu::scoped_connection conn = sink.connect<&sigh_listener::g>(listener);
    sigh.publish(42);

    RD_ASSERT_FALSE(sigh.empty());
    RD_ASSERT_TRUE(listener.k);
    RD_ASSERT_TRUE(conn);
  }

  sigh.publish(42);

  RD_ASSERT_TRUE(sigh.empty());
  RD_ASSERT_TRUE(listener.k);
}

RD_TEST_F(SigH, ScopedConnectionMove) {
  sigh_listener listener;
  rendu::sigh<void(int)> sigh;
  rendu::sink sink{sigh};

  rendu::scoped_connection outer{sink.connect<&sigh_listener::g>(listener)};

  RD_ASSERT_FALSE(sigh.empty());
  RD_ASSERT_TRUE(outer);

  {
    rendu::scoped_connection inner{std::move(outer)};

    RD_ASSERT_FALSE(listener.k);
    RD_ASSERT_FALSE(outer);
    RD_ASSERT_TRUE(inner);

    sigh.publish(42);

    RD_ASSERT_TRUE(listener.k);
  }

  RD_ASSERT_TRUE(sigh.empty());

  outer = sink.connect<&sigh_listener::g>(listener);

  RD_ASSERT_FALSE(sigh.empty());
  RD_ASSERT_TRUE(outer);

  {
    rendu::scoped_connection inner{};

    RD_ASSERT_TRUE(listener.k);
    RD_ASSERT_TRUE(outer);
    RD_ASSERT_FALSE(inner);

    inner = std::move(outer);

    RD_ASSERT_FALSE(outer);
    RD_ASSERT_TRUE(inner);

    sigh.publish(42);

    RD_ASSERT_FALSE(listener.k);
  }

  RD_ASSERT_TRUE(sigh.empty());
}

RD_TEST_F(SigH, ScopedConnectionConstructorsAndOperators) {
  sigh_listener listener;
  rendu::sigh<void(int)> sigh;
  rendu::sink sink{sigh};

  {
    rendu::scoped_connection inner{};

    RD_ASSERT_TRUE(sigh.empty());
    RD_ASSERT_FALSE(listener.k);
    RD_ASSERT_FALSE(inner);

    inner = sink.connect<&sigh_listener::g>(listener);
    sigh.publish(42);

    RD_ASSERT_FALSE(sigh.empty());
    RD_ASSERT_TRUE(listener.k);
    RD_ASSERT_TRUE(inner);

    inner.release();

    RD_ASSERT_TRUE(sigh.empty());
    RD_ASSERT_FALSE(inner);

    auto basic = sink.connect<&sigh_listener::g>(listener);
    inner = std::as_const(basic);
    sigh.publish(42);

    RD_ASSERT_FALSE(sigh.empty());
    RD_ASSERT_FALSE(listener.k);
    RD_ASSERT_TRUE(inner);
  }

  sigh.publish(42);

  RD_ASSERT_TRUE(sigh.empty());
  RD_ASSERT_FALSE(listener.k);
}

RD_TEST_F(SigH, ConstNonConstNoExcept) {
  rendu::sigh<void()> sigh;
  rendu::sink sink{sigh};
  const_nonconst_noexcept functor;
  const const_nonconst_noexcept cfunctor;
  sink.connect<&const_nonconst_noexcept::f>(functor);
  sink.connect<&const_nonconst_noexcept::g>(&functor);
  sink.connect<&const_nonconst_noexcept::h>(cfunctor);
  sink.connect<&const_nonconst_noexcept::i>(&cfunctor);
  sigh.publish();

  RD_ASSERT_EQ(functor.cnt, 2);
  RD_ASSERT_EQ(cfunctor.cnt, 2);

  sink.disconnect<&const_nonconst_noexcept::f>(functor);
  sink.disconnect<&const_nonconst_noexcept::g>(&functor);
  sink.disconnect<&const_nonconst_noexcept::h>(cfunctor);
  sink.disconnect<&const_nonconst_noexcept::i>(&cfunctor);
  sigh.publish();

  RD_ASSERT_EQ(functor.cnt, 2);
  RD_ASSERT_EQ(cfunctor.cnt, 2);
}

RD_TEST_F(SigH, BeforeFunction) {
  rendu::sigh<void(int)> sigh;
  rendu::sink sink{sigh};
  before_after functor;

  sink.connect<&before_after::add>(functor);
  sink.connect<&before_after::static_add>();
  sink.before<&before_after::static_add>().connect<&before_after::mul>(functor);
  sigh.publish(2);

  RD_ASSERT_EQ(functor.value, 6);
}

RD_TEST_F(SigH, BeforeMemberFunction) {
  rendu::sigh<void(int)> sigh;
  rendu::sink sink{sigh};
  before_after functor;

  sink.connect<&before_after::static_add>();
  sink.connect<&before_after::add>(functor);
  sink.before<&before_after::add>(functor).connect<&before_after::mul>(functor);
  sigh.publish(2);

  RD_ASSERT_EQ(functor.value, 6);
}

RD_TEST_F(SigH, BeforeFunctionWithPayload) {
  rendu::sigh<void(int)> sigh;
  rendu::sink sink{sigh};
  before_after functor;

  sink.connect<&before_after::static_add>();
  sink.connect<&before_after::static_mul>(functor);
  sink.before<&before_after::static_mul>(functor).connect<&before_after::add>(functor);
  sigh.publish(2);

  RD_ASSERT_EQ(functor.value, 8);
}

RD_TEST_F(SigH, BeforeInstanceOrPayload) {
  rendu::sigh<void(int)> sigh;
  rendu::sink sink{sigh};
  before_after functor;

  sink.connect<&before_after::static_mul>(functor);
  sink.connect<&before_after::add>(functor);
  sink.before(functor).connect<&before_after::static_add>();
  sigh.publish(2);

  RD_ASSERT_EQ(functor.value, 6);
}

RD_TEST_F(SigH, BeforeAnythingElse) {
  rendu::sigh<void(int)> sigh;
  rendu::sink sink{sigh};
  before_after functor;

  sink.connect<&before_after::add>(functor);
  sink.before().connect<&before_after::mul>(functor);
  sigh.publish(2);

  RD_ASSERT_EQ(functor.value, 2);
}

RD_TEST_F(SigH, BeforeListenerNotPresent) {
  rendu::sigh<void(int)> sigh;
  rendu::sink sink{sigh};
  before_after functor;

  sink.connect<&before_after::mul>(functor);
  sink.before<&before_after::add>(&functor).connect<&before_after::add>(functor);
  sigh.publish(2);

  RD_ASSERT_EQ(functor.value, 2);
}

RD_TEST_F(SigH, UnboundDataMember) {
  sigh_listener listener;
  rendu::sigh<bool &(sigh_listener &)> sigh;
  rendu::sink sink{sigh};

  RD_ASSERT_FALSE(listener.k);

  sink.connect<&sigh_listener::k>();
  sigh.collect([](bool &value) { value = !value; }, listener);

  RD_ASSERT_TRUE(listener.k);
}

RD_TEST_F(SigH, UnboundMemberFunction) {
  sigh_listener listener;
  rendu::sigh<void(sigh_listener *, int)> sigh;
  rendu::sink sink{sigh};

  RD_ASSERT_FALSE(listener.k);

  sink.connect<&sigh_listener::g>();
  sigh.publish(&listener, 42);

  RD_ASSERT_TRUE(listener.k);
}

RD_TEST_F(SigH, CustomAllocator) {
  std::allocator<void (*)(int)> allocator;
  rendu::sigh<void(int), decltype(allocator)> sigh{allocator};

  RD_ASSERT_EQ(sigh.get_allocator(), allocator);
  RD_ASSERT_FALSE(sigh.get_allocator() != allocator);
  RD_ASSERT_TRUE(sigh.empty());

  rendu::sink sink{sigh};
  sigh_listener listener;
  sink.template connect<&sigh_listener::g>(listener);

  decltype(sigh) copy{sigh, allocator};
  sink.disconnect(listener);

  RD_ASSERT_TRUE(sigh.empty());
  RD_ASSERT_FALSE(copy.empty());

  sigh = copy;

  RD_ASSERT_FALSE(sigh.empty());
  RD_ASSERT_FALSE(copy.empty());

  decltype(sigh) move{std::move(copy), allocator};

  RD_ASSERT_TRUE(copy.empty());
  RD_ASSERT_FALSE(move.empty());

  sink = rendu::sink{move};
  sink.disconnect(&listener);

  RD_ASSERT_TRUE(copy.empty());
  RD_ASSERT_TRUE(move.empty());

  sink.template connect<&sigh_listener::g>(listener);
  copy.swap(move);

  RD_ASSERT_FALSE(copy.empty());
  RD_ASSERT_TRUE(move.empty());

  sink = rendu::sink{copy};
  sink.disconnect();

  RD_ASSERT_TRUE(copy.empty());
  RD_ASSERT_TRUE(move.empty());
}
