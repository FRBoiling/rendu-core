/*
* Created by boil on 2023/11/2.
*/

#include "append_file.h"
#include "base/string/str_error.h"
#include "file.h"

RD_NAMESPACE_BEGIN

  AppendFile::AppendFile(StringArg filename)
    : fp_(::fopen(filename.c_str(), "ae")),  // 'e' for O_CLOEXEC
      writtenBytes_(0) {
    assert(fp_);
    ::setbuffer(fp_, buffer_, sizeof buffer_);
    // posix_fadvise POSIX_FADV_DONTNEED ?
  }

  AppendFile::~AppendFile() {
    ::fclose(fp_);
  }

  void AppendFile::append(const char *logline, const size_t len) {
    size_t written = 0;

    while (written != len) {
      size_t remain = len - written;
      size_t n = write(logline + written, remain);
      if (n != remain) {
        int err = ferror(fp_);
        if (err) {
          fprintf(stderr, "AppendFile::append() failed %d\n", strerror_tl(err));
          break;
        }
      }
      written += n;
    }

    writtenBytes_ += written;
  }

  void AppendFile::flush() {
    ::fflush(fp_);
  }

  size_t AppendFile::write(const char *logline, size_t len) {
    // #undef fwrite_unlocked
    return fwrite_unlocked(logline, 1, len, fp_);
  }

RD_NAMESPACE_END