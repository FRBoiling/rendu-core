//
// Created by boil on 2023/2/14.
//
#include <iostream>
#include <set>
#include <map>

void MergeContainer(std::map<int, std::set<int>> &container1, const std::map<int, std::set<int>> &container3);
template<typename T>
T max(T x, T y) {
  return x < y ? y : x;
}

template<> // template<>表示这是一个特化版本
int64_t max<int64_t>(int64_t x, int64_t y) // 加上一个尖括号并指定特化类型，如果可以推断出也可以省略
{
  return x < y ? x : y; // 返回较小值
}
void MergeContainer(std::map<int, std::set<int>> &container1, std::map<int, std::set<int>> &container3) {
  for(auto it = container1.begin(); it!=container1.end(); ++it){
    container3[it->first].insert(it->second.begin(),it->second.end());
  }
}

int main() {
  bool b_ff = false;
  std::cout << b_ff << std::endl;

  std::cout << ::max(10, 20) << std::endl;
  std::cout << ::max<int>(10, 20) << std::endl;
  std::cout << ::max(1.2, 2.2) << std::endl;
  std::cout << ::max<double>(1.2, 2.2) << std::endl;
  std::cout << ::max<int64_t>(10, 20) << std::endl;

  std::cout << "Hello Template" << std::endl;

  std::map<int, std::set<int> > container1;
  std::map<int, std::set<int> > container2;
  std::map<int, std::set<int> > container3;
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 3; ++j) {
      container1[i].insert(j);
      container2[i].insert(j+3);
    }
  }
  MergeContainer(container1, container3);
  MergeContainer(container2, container3);

  std::cout << "Hello container" << std::endl;

}

