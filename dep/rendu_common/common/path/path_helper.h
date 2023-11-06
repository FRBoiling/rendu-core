/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_PATH_HELPER_H
#define RENDU_PATH_HELPER_H

#include "common/define.h"

RD_NAMESPACE_BEGIN

  class PathHelper {
  public:
    static string exePath(bool isExe = true);
    static string exeDir(bool isExe = true);
    static string exeName(bool isExe = true);
  };


RD_NAMESPACE_END

#endif //RENDU_PATH_HELPER_H
