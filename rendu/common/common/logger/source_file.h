/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_COMMON_SOURCE_FILE_H
#define RENDU_COMMON_SOURCE_FILE_H

#include "common/common_define.h"

RD_NAMESPACE_BEGIN

// compile time calculation of basename of source file
  class SourceFile {
  public:
    template<int N>
     SourceFile(const char (&arr)[N])
      : data_(arr),
        size_(N - 1) {
      const char *slash = strrchr(data_, '/'); // builtin function
      if (slash) {
        data_ = slash + 1;
        size_ -= static_cast<int>(data_ - arr);
      }
    }

     SourceFile(const char *filename)
      : data_(filename) {
      const char *slash = strrchr(filename, '/');
      if (slash) {
        data_ = slash + 1;
      }
      size_ = static_cast<int>(strlen(data_));
    }

  public:
    const char* GetData() const{
      return data_;
    }

    int GetSize() const{
      return size_;
    }
  private:
    const char *data_;
    int size_;
  };


RD_NAMESPACE_END

#endif //RENDU_COMMON_SOURCE_FILE_H
