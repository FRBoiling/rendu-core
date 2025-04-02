/*
* Created by boil on 2024/8/23.
*/

#ifndef RENDU_BASIC_BASIC_IO_DIRECTORY_H_
#define RENDU_BASIC_BASIC_IO_DIRECTORY_H_

#include "io_define.h"

#include "path.h"
#include <filesystem>

RD_IO_NAMESPACE_BEGIN


class Directory {

  bool Exists(const String & path);
  
  std::vector<String> GetFiles(const String & path, const String & searchPattern = "*");

  std::vector<String> GetDirectories(const String& path);
  
  
};

RD_IO_NAMESPACE_END

#endif//RENDU_BASIC_BASIC_IO_DIRECTORY_H_
