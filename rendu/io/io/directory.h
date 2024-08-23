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

  bool Exists(const String & path) {
    return std::filesystem::exists(path);
      return false;
  }
  
  std::vector<String> GetFiles(const String & path, const String & searchPattern = "*") {
    std::vector<String> files;
    if (Exists(path)) {
      for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_regular_file()) {
          if (searchPattern == "*" || entry.path().filename().string().find(searchPattern)!= String::npos) {
            files.push_back(entry.path().string());
          }
        }
      }
    }
    return files;
  }

  std::vector<String> GetDirectories(const String& path) {
    std::vector<String> directories;
    if (Exists(path)) {
      for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_directory()) {
          directories.push_back(entry.path().string());
        }
      }
    }
    return directories;
  }
  
  
};

RD_IO_NAMESPACE_END

#endif//RENDU_BASIC_BASIC_IO_DIRECTORY_H_
