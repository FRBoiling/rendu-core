/*
* Created by boil on 2024/8/23.
*/

#ifndef RENDU_BASIC_BASIC_IO_PATH_H_
#define RENDU_BASIC_BASIC_IO_PATH_H_

#include "io_define.h"

RD_IO_NAMESPACE_BEGIN

class Path {
public:
  String GetFileName(String path);
  String GetExtension(String path);
  String GetFileNameWithoutExtension(String path);

  String GetDirectoryName(String path);
  String GetFullPath(String path);
  String GetRelativePath(String path);

  bool Compare(String path1, String path2);
};

RD_IO_NAMESPACE_END

#endif//RENDU_BASIC_BASIC_IO_PATH_H_
