//
// Created by boil on 2023/2/14.
//
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>

std::vector<int> get_random_numbers(unsigned int seed) {
//  std::srand(seed);
  std::vector<int> random_numbers;
  for (int i = 0; i < 5; ++i) {
    random_numbers.push_back(std::rand() % 10000 + 1);
  }
  return random_numbers;
}

int main() {
  auto now = std::time(nullptr);
  auto a1 = get_random_numbers(now);
  auto a2 = get_random_numbers(now);
  return 0;
}

