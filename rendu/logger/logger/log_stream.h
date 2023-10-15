/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_LOG_STREAM_H
#define RENDU_LOG_STREAM_H

#include "base/non_copyable.h"
#include "fixed_buffer.h"
#include "source_file.h"
#include "fmt.h"

RD_NAMESPACE_BEGIN
  class Logger;

  class LogStream : NonCopyable {
    typedef LogStream self;
  public:
    typedef FixedBuffer<kSmallBuffer> Buffer;

    self &operator<<(bool v) {
      buffer_.append(v ? "1" : "0", 1);
      return *this;
    }

    self &operator<<(short);

    self &operator<<(unsigned short);

    self &operator<<(int);

    self &operator<<(unsigned int);

    self &operator<<(long);

    self &operator<<(unsigned long);

    self &operator<<(long long);

    self &operator<<(unsigned long long);

    self &operator<<(const void *);

    self &operator<<(float v) {
      *this << static_cast<double>(v);
      return *this;
    }

    self &operator<<(double);
    // self& operator<<(long double);

    self &operator<<(char v) {
      buffer_.append(&v, 1);
      return *this;
    }

    // self& operator<<(signed char);
    // self& operator<<(unsigned char);

    self &operator<<(const char *str) {
      if (str) {
        buffer_.append(str, strlen(str));
      } else {
        buffer_.append("(null)", 6);
      }
      return *this;
    }

    self &operator<<(const unsigned char *str) {
      return operator<<(reinterpret_cast<const char *>(str));
    }

    self &operator<<(const string &v) {
      buffer_.append(v.c_str(), v.size());
      return *this;
    }

    self &operator<<(const StringPiece &v) {
      buffer_.append(v.data(), v.size());
      return *this;
    }

    self &operator<<(const Buffer &v) {
      *this << v.toStringPiece();
      return *this;
    }

    void append(const char *data, int len) { buffer_.append(data, len); }

    const Buffer &buffer() const { return buffer_; }

    void resetBuffer() { buffer_.reset(); }

  private:
    void staticCheck();

    template<typename T>
    void formatInteger(T);

    Buffer buffer_;

    static const int kMaxNumericSize = 48;
  };


  class T
  {
  public:
    T(const char* str, unsigned len)
      :str_(str),
       len_(len)
    {
      assert(strlen(str) == len_);
    }

    const char* str_;
    const unsigned len_;
  };

  inline LogStream &operator<<(LogStream &s, const Fmt &fmt) {
    s.append(fmt.data(), fmt.length());
    return s;
  }

  inline LogStream& operator<<(LogStream& s, T v)
  {
    s.append(v.str_, v.len_);
    return s;
  }

  inline LogStream &operator<<(LogStream &s, const SourceFile &v) {
    s.append(v.GetData(), v.GetSize());
    return s;
  }


// Format quantity n in SI units (k, M, G, T, P, E).
// The returned string is atmost 5 characters long.
// Requires n >= 0
  string formatSI(int64_t n);

// Format quantity n in IEC (binary) units (Ki, Mi, Gi, Ti, Pi, Ei).
// The returned string is atmost 6 characters long.
// Requires n >= 0
  string formatIEC(int64_t n);

RD_NAMESPACE_END

#endif //RENDU_LOG_STREAM_H
