/*
* Created by boil on 2023/11/8.
*/

#ifndef RENDU_LOG_COLOR_H
#define RENDU_LOG_COLOR_H

#include "common/define.h"

RD_NAMESPACE_BEGIN

  namespace LogColor {

#ifdef _WIN32
#define CLEAR_COLOR 7
    static const WORD LOG_CONST_TABLE[][3] = {
            {0x97, 0x09 , 'T'},//蓝底灰字，黑底蓝字，window console默认黑底
            {0xA7, 0x0A , 'D'},//绿底灰字，黑底绿字
            {0xB7, 0x0B , 'I'},//天蓝底灰字，黑底天蓝字
            {0xE7, 0x0E , 'W'},//黄底灰字，黑底黄字
            {0xC7, 0x0C , 'E'} };//红底灰字，黑底红字

    bool SetConsoleColor(WORD Color)
    {
        HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
        if (handle == 0)
            return false;

        BOOL ret = SetConsoleTextAttribute(handle, Color);
        return(ret == TRUE);
    }
#else


    static const char *LOG_CONST_TABLE[][2] = {
      {"\033[44;37m", "\033[34m"},
      {"\033[42;37m", "\033[32m"},
      {"\033[46;37m", "\033[36m"},
      {"\033[43;37m", "\033[33m"},
      {"\033[41;37m", "\033[31m"},
      {"\033[45;37m", "\033[35m"}};

#define SET_COLOR(level) LOG_CONST_TABLE[level][1]

#define CLEAR_COLOR "\033[0m"

#endif

  }

RD_NAMESPACE_END

#endif //RENDU_LOG_COLOR_H
