/*
* Created by boil on 2023/11/13.
*/

#ifndef RENDU_BASE_PATH_PATH_HELPER_H_
#define RENDU_BASE_PATH_PATH_HELPER_H_

#include "string/string_helper.h"

RD_NAMESPACE_BEGIN

 std::string GetExePath(bool isExe = true);
 std::string GetExeDir(bool isExe = true);
 std::string GetExeName(bool isExe = true);

RD_NAMESPACE_END

#endif//RENDU_BASE_PATH_PATH_HELPER_H_
