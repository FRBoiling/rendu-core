/*
* Created by boil on 2023/11/13.
*/

#ifndef RENDU_MY_HEADER_H
#define RENDU_MY_HEADER_H

// 头文件，假设为my_header.h
#include <iostream>
#include <string>

// 声明一个函数模板变量，名为tostr，作为std::to_string的别名
template<typename T>
std::string tostr(T value) {
  return std::to_string(value);
}

#endif //RENDU_MY_HEADER_H
