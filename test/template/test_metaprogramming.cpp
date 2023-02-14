//
// Created by Administrator on 2023/2/14.
//
#include "gtest/gtest.h"

template<int N>
class Fac {
public:
  enum { value = N * Fac<N - 1>::value };
};

template<>
class Fac<0> {
public:
  enum { value = 1 };
};

//一个模板元编程一般包括：递归构造的手段、表示状态的模板参数、一个表示终点的特化以及具体实现的算法。
TEST(TemplateTest, Metaprogramming) {
  std::cout << Fac<5>::value; // 120
}