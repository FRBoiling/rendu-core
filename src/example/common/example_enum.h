/*
* Created by boil on 2022/9/24.
*/

#pragma once
#include <iostream>
#include "enum/enum_utils.h"

enum NormalEnum{
  One = 1,
  Two = 2,
  Three = 3,
};

enum class ClassEnum{
  One = 11,
  Two = 22,
  Three = 33,
};


void enum_example(){
  std::cout<< Two << std::endl;
  std::cout << rendu::EnumUtils::ToString(ClassEnum::Two) << std::endl;
}
