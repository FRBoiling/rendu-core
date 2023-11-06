/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_COMMON_READ_SMALL_FILE_H
#define RENDU_COMMON_READ_SMALL_FILE_H

#include "base/string/string_arg.h"
#include "base/utils/non_copyable.h"

RD_NAMESPACE_BEGIN

// read small file < 64KB
  class ReadSmallFile : NonCopyable {
  public:
    ReadSmallFile(StringArg filename);

    ~ReadSmallFile();

    // return errno
    template<typename String>
    int readToString(int maxSize,
                     String *content,
                     int64_t *fileSize,
                     int64_t *modifyTime,
                     int64_t *createTime);

    /// Read at maxium kBufferSize into buf_
    // return errno
    int readToBuffer(int *size);

    const char *buffer() const { return buf_; }

    static const int kBufferSize = 64 * 1024;

  private:
    int fd_;
    int err_;
    char buf_[kBufferSize];
  };

RD_NAMESPACE_END

#endif //RENDU_COMMON_READ_SMALL_FILE_H
