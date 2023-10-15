/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_BUFFER_LIKE_STRING_H
#define RENDU_BUFFER_LIKE_STRING_H

#include "a_buffer.h"
#include "utils/object_statistic.h"

RD_NAMESPACE_BEGIN

  class BufferLikeString : public ABuffer {
  public:
    BufferLikeString();

    ~BufferLikeString() override = default;

    explicit BufferLikeString(std::string str);

    explicit BufferLikeString(const char *str);

    BufferLikeString(BufferLikeString &&that) noexcept;

    BufferLikeString(const BufferLikeString &that) noexcept;

  public:
    BufferLikeString &operator=(std::string str) {
      _str = std::move(str);
      _erase_head = 0;
      _erase_tail = 0;
      return *this;
    }

    BufferLikeString &operator=(const char *str) {
      _str = str;
      _erase_head = 0;
      _erase_tail = 0;
      return *this;
    }

    BufferLikeString &operator=(BufferLikeString &&that) {
      _str = std::move(that._str);
      _erase_head = that._erase_head;
      _erase_tail = that._erase_tail;
      that._erase_head = 0;
      that._erase_tail = 0;
      return *this;
    }

    BufferLikeString &operator=(const BufferLikeString &that) {
      _str = that._str;
      _erase_head = that._erase_head;
      _erase_tail = that._erase_tail;
      return *this;
    }

    char *Data() const override {
      return (char *) _str.data() + _erase_head;
    }

    size_t Size() const override {
      return _str.size() - _erase_tail - _erase_head;
    }

    BufferLikeString &Erase(size_t pos = 0, size_t n = std::string::npos);

    BufferLikeString &Append(const BufferLikeString &str);

    BufferLikeString &Append(const std::string &str);

    BufferLikeString &Append(const char *data) {
      return Append(data, strlen(data));
    }

    BufferLikeString &Append(const char *data, size_t len);

    void PushBack(char c);

    BufferLikeString &Insert(size_t pos, const char *s, size_t n);

    BufferLikeString &Assign(const char *data);

    BufferLikeString &Assign(const char *data, size_t len);

    void Clear();

    char &operator[](size_t pos) {
      if (pos >= Size()) {
        throw std::out_of_range("BufferLikeString::operator[] out_of_range");
      }
      return Data()[pos];
    }

    const char &operator[](size_t pos) const {
      return (*const_cast<BufferLikeString *>(this))[pos];
    }

    size_t Capacity() const;

    void Reserve(size_t size);

    void Resize(size_t size, char c = '\0');

    bool IsEmpty() const;

    std::string Substr(size_t pos, size_t n = std::string::npos) const;

  private:
    void moveData() {
      if (_erase_head) {
        _str.erase(0, _erase_head);
        _erase_head = 0;
      }
    }

  private:
    size_t _erase_head;
    size_t _erase_tail;
    std::string _str;
    //对象个数统计
    ObjectStatistic<BufferLikeString> _statistic;
  };


RD_NAMESPACE_END

#endif //RENDU_BUFFER_LIKE_STRING_H
