/*
* Created by boil on 2024/8/22.
*/

#ifndef RENDU_TUTORIAL_BASIC_STRING_FORMAT_H_
#define RENDU_TUTORIAL_BASIC_STRING_FORMAT_H_

#include "basic/utils/string_format.h"

void string_format_example() {

  // 格式化字符串
  std::string str = "hello";
  str += ", world!";
  printf("%s\n", str.c_str());
  std::string str2 = "\"async_example begin... {}\"";
  printf("%s\n", format(str2.c_str(), 1111).c_str());
}


#endif//RENDU_TUTORIAL_BASIC_STRING_FORMAT_H_
