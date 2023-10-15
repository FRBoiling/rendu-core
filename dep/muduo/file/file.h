/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_FILE_H
#define RENDU_FILE_H

#include "read_small_file.h"

RD_NAMESPACE_BEGIN

// read the file content, returns errno if error happens.
  template<typename String>
  int readFile(StringArg filename,
               int maxSize,
               String *content,
               int64_t *fileSize = nullptr,
               int64_t *modifyTime = nullptr,
               int64_t *createTime = nullptr) {
    ReadSmallFile file(filename);
    return file.readToString(maxSize, content, fileSize, modifyTime, createTime);
  }


#include <unistd.h>

  size_t fwrite_unlocked(const void *ptr, size_t size, size_t n, FILE *stream);

RD_NAMESPACE_END

#endif //RENDU_FILE_H
