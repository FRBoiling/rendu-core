//
// Created by boil on 2023/3/10.
//

#include "vector_example.h"
#include "set_example.h"
#include "map_example.h"
#include "unordered_map.h"
#include <iostream>

#include <queue>

class Data {
public:
  explicit Data(int value) : m_value(value) {
    std::cout << "Data():" << m_value << std::endl;
  }

  ~Data() {
    std::cout << "~Data():" << m_value << std::endl;
  }

  Data(const Data &d) {
    std::cout << "Data(const Data& d)" << std::endl;
    m_value = d.m_value;
  }


  Data(Data &&d)  noexcept {
    std::cout << "Data(const Data& d)" << std::endl;
    m_value = d.m_value;
  }

private:
  int m_value;
};

[[nodiscard]] Data Dequeue(std::queue<Data>& queue) noexcept {
  Data element = queue.front();
  queue.pop();
  return element;
}

Data return_urvo_value() {
  return Data{3};
}

Data return_nrvo_value() {
  Data local_obj(4);
  return local_obj;
}

int main() {
  auto x = return_urvo_value();
  auto y = return_nrvo_value();

  std::queue<Data> queue_test;
  queue_test.emplace(1);
  queue_test.emplace(2);
  queue_test.pop();
//
  auto t = Dequeue(queue_test);

  vector_example();
  set_example();
  map_example();
  unordered_map_example();
  std::cout << "Hello container" << std::endl;
}
