/*
* Created by boil on 2024/8/23.
*/

#include "path.h"

RD_IO_NAMESPACE_BEGIN

String Path::GetFileNameWithoutExtension(String path) {
  std::size_t lastSlash = path.find_last_of("/\\");
  std::size_t lastDot = path.find_last_of('.');
  if (lastSlash == std::string::npos) {
    lastSlash = -1;
  }
  if (lastDot == std::string::npos || lastDot < lastSlash) {
    return path.substr(lastSlash + 1);
  }
  return path.substr(lastSlash + 1, lastDot - lastSlash - 1);
}

String Path::GetFileName(String path) {
  std::size_t lastSlash = path.find_last_of("/\\");
  if (lastSlash == String::npos) {
    lastSlash = -1;
  }
  return path.substr(lastSlash + 1, lastSlash - 1);
}

String Path::GetExtension(String path) {
  std::size_t lastDot = path.find_last_of('.');
  if (lastDot == std::string::npos) {
    return "";
  }
  return path.substr(lastDot + 1);
}

bool Path::Compare(String path1, String path2){
  return path1.compare(path2) == 0;
}



RD_IO_NAMESPACE_END