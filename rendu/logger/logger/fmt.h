/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_FMT_H
#define RENDU_FMT_H

#include "common/define.h"

RD_NAMESPACE_BEGIN

  class Fmt // : noncopyable
  {
  public:
    template<typename T>
    Fmt(const char *fmt, T val) {
      static_assert(std::is_arithmetic<T>::value == true, "Must be arithmetic type");

      length_ = snprintf(buf_, sizeof buf_, fmt, val);
      assert(static_cast<size_t>(length_) < sizeof buf_);
    }

    const char *data() const { return buf_; }

    int32 length() const { return length_; }

  private:
    char buf_[32]{};
    int32 length_;
  };

RD_NAMESPACE_END

#endif //RENDU_FMT_H
