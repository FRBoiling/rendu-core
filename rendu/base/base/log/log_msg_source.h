/*
* Created by boil on 2023/12/23.
*/

#ifndef RENDU_LOG_MSG_SOURCE_H
#define RENDU_LOG_MSG_SOURCE_H

#include "base_define.h"
#include "string/string_helper.h"

RD_NAMESPACE_BEGIN

struct LogMsgSource {
public:
  LogMsgSource() = default;
  LogMsgSource(const char *filename_in, int line_in, const char *funcname_in)
      : filename_{filename_in}, line_{line_in}, funcname_{funcname_in} {}

  bool empty() const {
    return line_ == 0;
  }

private:
  const char *filename_{nullptr};
  int line_{0};
  const char *funcname_{nullptr};

public:
  inline const char *GetFileName() const{
    auto pos = strrchr(filename_, '/');
#ifdef _WIN32
    if (!pos) {
      pos = strrchr(file, '\\');
    }
#endif
    return pos ? pos + 1 : filename_;
  }

  inline const char *GetFunctionName() const{
#ifdef _WIN32
    auto pos = strrchr(funcname_, ':');
    return pos ? pos + 1 : funcname_;
#else
    return funcname_;
#endif
  }

  inline int GetLine() const {
    return line_;
  }
};

RD_NAMESPACE_END

#endif//RENDU_LOG_MSG_SOURCE_H
