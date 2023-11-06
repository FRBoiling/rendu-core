/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_FIXED_BUFFER_H
#define RENDU_FIXED_BUFFER_H

#include "common/utils/non_copyable.h"
#include "common/utils/string_piece.h"
#include "common/utils/type_cast.h"
#include "common/utils/memory_ops.h"

RD_NAMESPACE_BEGIN

  const int kSmallBuffer = 4000;
  const int kLargeBuffer = 4000 * 1000;

  template<int SIZE>
  class FixedBuffer : NonCopyable {
  public:
    FixedBuffer()
      : cur_(data_) {
      setCookie(cookieStart);
    }

    ~FixedBuffer() {
      setCookie(cookieEnd);
    }

    void append(const char * /*restrict*/ buf, size_t len) {
      // FIXME: append partially
      if (TypeCast::implicit_cast<size_t>(avail()) > len) {
        memcpy(cur_, buf, len);
        cur_ += len;
      }
    }

    const char *data() const { return data_; }

    int length() const { return static_cast<int>(cur_ - data_); }

    // write to data_ directly
    char *current() { return cur_; }

    int avail() const { return static_cast<int>(end() - cur_); }

    void add(size_t len) { cur_ += len; }

    void reset() { cur_ = data_; }

    void bzero() { MemoryOps::memZero(data_, sizeof data_); }

    // for used by GDB
    const char *debugString();

    void setCookie(void (*cookie)()) { cookie_ = cookie; }

    // for used by unit test
    string toString() const { return string(data_, length()); }

    StringPiece toStringPiece() const { return StringPiece(data_, length()); }

  private:
    const char *end() const { return data_ + sizeof data_; }

    // Must be outline function for cookies.
    static void cookieStart();

    static void cookieEnd();

    void (*cookie_)();

    char data_[SIZE];
    char *cur_;
  };

  template
  class FixedBuffer<kSmallBuffer>;

  template
  class FixedBuffer<kLargeBuffer>;

RD_NAMESPACE_END

#endif //RENDU_FIXED_BUFFER_H
