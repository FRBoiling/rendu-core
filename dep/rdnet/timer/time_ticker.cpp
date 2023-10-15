/*
* Created by boil on 2023/10/26.
*/

#include "time_ticker.h"

RD_NAMESPACE_BEGIN

  Ticker::Ticker(uint64_t min_ms, LogContextCapture ctx, bool print_log) : _ctx(std::move(ctx)) {
    if (!print_log) {
      _ctx.Clear();
    }
    _created = _begin = GetCurrentMillisecond();
    _min_ms = min_ms;
  }

  Ticker::~Ticker() {
    uint64_t tm = CreatedTime();
    if (tm > _min_ms) {
      _ctx << "take time: " << tm << "ms" << ", thread may be overloaded";
    } else {
      _ctx.Clear();
    }
  }

  uint64_t Ticker::ElapsedTime() const {
    return GetCurrentMillisecond() - _begin;
  }

  uint64_t Ticker::CreatedTime() const {
    return GetCurrentMillisecond() - _created;
  }

  void Ticker::ResetTime() {
    _begin = GetCurrentMillisecond();
  }


RD_NAMESPACE_END