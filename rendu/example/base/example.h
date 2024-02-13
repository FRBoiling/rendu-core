/*
* Created by boil on 2023/11/10.
*/
#ifndef RENDU_LOG_EXAMPLE_H
#define RENDU_LOG_EXAMPLE_H

#include "log.hpp"


using namespace rendu::log;

enum class Color { Red, Green, Blue };

class MyClass {
public:
  MyClass(int value) : m_value(value) {}

  void printAddress() {
    RD_TRACE("This pointer address: {}", rendu::Convert::ToString(this).value());
//    std::cout << "This pointer address: " << rendu::Convert::ToString(this).value()<< std::endl;
  }

  int getValue() {
    return m_value;
  }

private:
  int m_value;
};



int example() {
  auto intToString = rendu::Convert::ToString(42);
  assert(intToString == "42");
  std::cout << "42 -> " << *intToString << std::endl;

  // 布尔类型特殊处理
  auto boolToString = rendu::Convert::ToString(true);
  assert(boolToString == "true");
  std::cout << "true -> " << *boolToString << std::endl;

  // 浮点数到字符串
  auto floatToString = rendu::Convert::ToString(3.14f);
  assert(floatToString == "3.140000"); // 注意：转换结果依赖于 std::to_string 的实现
  std::cout << "3.14 -> " << *floatToString << std::endl;

  // 枚举类型到字符串
  auto enumToString = rendu::Convert::ToString(Color::Red);
  assert(enumToString == "0"); // 枚举的底层类型默认为 int，Red 通常为 0
  std::cout << "Color::Red -> " << *enumToString << std::endl;


  // 数组类型到字符串
  int myArray[] = {1, 2, 3};
  auto arrayToString = rendu::Convert::ToString(myArray);
  assert(arrayToString == "[1, 2, 3]");
  std::cout << "{1, 2, 3} -> " << *arrayToString << std::endl;


  // 非算术类型（指针）到字符串
  MyClass obj(42);

  auto ptrToString = rendu::Convert::ToString(&obj);
  std::cout << "Pointer -> " << *ptrToString << std::endl; // 输出将依赖于指针的实际地址

  obj.printAddress();

  return 0;
}


#endif//RENDU_LOG_EXAMPLE_H
