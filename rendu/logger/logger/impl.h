/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_IMPL_H
#define RENDU_IMPL_H

#include "log_level.h"
#include "time/time_zone.h"
#include "time/timestamp.h"
#include "log_stream.h"
#include "source_file.h"

RD_NAMESPACE_BEGIN

  class Impl {
  public:
    Impl(LogLevel level, int old_errno, const SourceFile &file, int line);

    void formatTime();

    void finish();

    Timestamp time_;
    LogStream stream_;
    LogLevel level_;
    int line_;
    SourceFile basename_;
  };

RD_NAMESPACE_END

#endif //RENDU_IMPL_H
