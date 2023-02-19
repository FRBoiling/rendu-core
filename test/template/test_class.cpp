//
// Created by boil on 2023/2/14.
//
#include <test/rdtest.h>
namespace {

template<typename T>
class A {
 public:
  A(T y) : x(y) {}

 private:
  T x;
 public:
  T get_value() {
    return x;
  }

};

RD_TEST(TemplateTest, Class) {
  A<int> a(10);
  RD_EXPECT_EQ(10, a.get_value());
  A<float> b(0.1f);
  RD_EXPECT_EQ(0.1f, b.get_value());
}

// 变量模板
template<typename T>
T zero = 0;

// 别名模板
template<typename T>
using Container = std::vector<T>;

RD_TEST(TemplateTest, Alias) {
  auto i = zero<int> = 11; // 相当于int i = 11;
  Container<int> v{1, 2, 3}; // 相当于std::vector<int> v{1, 2, 3};
  RD_EXPECT_EQ(11, i);
  RD_EXPECT_EQ(2, v[1]);
}
}
