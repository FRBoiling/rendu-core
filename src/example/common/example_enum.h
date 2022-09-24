/*
* Created by boil on 2022/9/24.
*/

#pragma once
#include <iostream>
#include "enum/enum_utils.h"

using namespace rendu;

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
  std::cout<< One << std::endl;
  std::cout<< ClassEnum::One << std::endl;

  std::cout << EnumUtils::ToString(Two) << std::endl;
  std::cout << EnumUtils::ToString(ClassEnum::Two) << std::endl;

  int tt = Three + NormalEnum::Three;

  std::cout<< tt << std::endl;

  auto three = EnumUtils::FromIndex<ClassEnum>(2);
  std::cout << EnumUtils::ToString(three) << std::endl;
  auto index = EnumUtils::ToIndex(ClassEnum::One);

  int n_one = (int)ClassEnum::One;
  ClassEnum e_one = (ClassEnum)11;

  int n_n_one = One;
  NormalEnum e_n_one = (NormalEnum)1;
//  auto one = EnumUtils::FromIndex<NormalEnum>(Three);

}
