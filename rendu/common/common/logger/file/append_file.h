/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_COMMON_APPEND_FILE_H
#define RENDU_COMMON_APPEND_FILE_H

#include "base/string/string_arg.h"
#include "base/utils/non_copyable.h"

RD_NAMESPACE_BEGIN

// not thread safe
  class AppendFile : NonCopyable {
  public:
    explicit AppendFile(StringArg filename);

    ~AppendFile();

    void append(const char *logline, size_t len);

    void flush();

    off_t writtenBytes() const { return writtenBytes_; }

  private:

    size_t write(const char *logline, size_t len);

    FILE *fp_;
    char buffer_[64 * 1024];
    off_t writtenBytes_;
  };

RD_NAMESPACE_END

#endif //RENDU_COMMON_APPEND_FILE_H
