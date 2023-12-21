/*
* Created by boil on 2023/11/8.
*/

#ifndef RENDU_LOG_CONSOLE_COLOR_H
#define RENDU_LOG_CONSOLE_COLOR_H

#include "log_define.h"

LOG_NAMESPACE_BEGIN
namespace ConsoleColor {

#ifdef _WIN32
#define CLEAR_COLOR 7
  static const WORD LOG_CONST_TABLE[][2] = {
      {0x97, 0x09}, //蓝底灰字，黑底蓝字，window console默认黑底
      {0xB7, 0x0B}, //天蓝底灰字，黑底天蓝字
      {0xA7, 0x0A}, //绿底灰字，黑底绿字
      {0xE7, 0x0E}, //黄底灰字，黑底黄字
      {0xC7, 0x0C}};//红底灰字，黑底红字

  bool SetConsoleColor(WORD Color) {
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (handle == 0)
      return false;

    BOOL ret = SetConsoleTextAttribute(handle, Color);
    return (ret == TRUE);
  }
#else


  static const char *LOG_CONST_TABLE[][2] = {
      {"\033[44;37m", "\033[37m"},  //蓝底灰字，黑底灰字
      {"\033[46;37m", "\033[36m"},  //天蓝底灰字，黑底天蓝字
      {"\033[42;37m", "\033[32m"},  //绿底灰字，黑底绿字
      {"\033[43;37m", "\033[33m"},  //黄底灰字，黑底黄字
      {"\033[41;37m", "\033[31m"},  //红底灰字，黑底红字
      {"\033[41;30m", "\033[35m"}   //红底黑字，黑底紫字
  };

#define SET_COLOR(level,index) LOG_CONST_TABLE[level][index]

#define CLEAR_COLOR "\033[0m"

#endif

}// namespace ConsoleColor

LOG_NAMESPACE_END

#endif//RENDU_LOG_CONSOLE_COLOR_H
