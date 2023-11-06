/*
* Created by boil on 2023/10/26.
*/

#include "buffer_like_string.h"

RD_NAMESPACE_BEGIN
  StatisticImp(BufferLikeString)



BufferLikeString::BufferLikeString(std::string str) {
  _str = std::move(str);
  _erase_head = 0;
  _erase_tail = 0;
}

BufferLikeString::BufferLikeString(const char *str) {
  _str = str;
  _erase_head = 0;
  _erase_tail = 0;
}

BufferLikeString::BufferLikeString(BufferLikeString &&that)  noexcept {
  _str = std::move(that._str);
  _erase_head = that._erase_head;
  _erase_tail = that._erase_tail;
  that._erase_head = 0;
  that._erase_tail = 0;
}

BufferLikeString::BufferLikeString(const BufferLikeString &that) noexcept {
  _str = that._str;
  _erase_head = that._erase_head;
  _erase_tail = that._erase_tail;
}

BufferLikeString &BufferLikeString::Erase(size_t pos, size_t n) {
  if (pos == 0) {
    //移除前面的数据
    if (n != std::string::npos) {
      //移除部分
      if (n > Size()) {
        //移除太多数据了
        throw std::out_of_range("BufferLikeString::erase out_of_range in head");
      }
      //设置起始便宜量
      _erase_head += n;
      Data()[Size()] = '\0';
      return *this;
    }
    //移除全部数据
    _erase_head = 0;
    _erase_tail = _str.size();
    Data()[0] = '\0';
    return *this;
  }

  if (n == std::string::npos || pos + n >= Size()) {
    //移除末尾所有数据
    if (pos >= Size()) {
      //移除太多数据
      throw std::out_of_range("BufferLikeString::erase out_of_range in tail");
    }
    _erase_tail += Size() - pos;
    Data()[Size()] = '\0';
    return *this;
  }

  //移除中间的
  if (pos + n > Size()) {
    //超过长度限制
    throw std::out_of_range("BufferLikeString::erase out_of_range in middle");
  }
  _str.erase(_erase_head + pos, n);
  return *this;
}

BufferLikeString &BufferLikeString::Append(const BufferLikeString &str) {
  return Append(str.Data(), str.Size());
}

BufferLikeString &BufferLikeString::Append(const std::string &str) {
  return Append(str.data(), str.size());
}

BufferLikeString &BufferLikeString::Append(const char *data, size_t len) {
  if (len <= 0) {
    return *this;
  }
  if (_erase_head > _str.capacity() / 2) {
    moveData();
  }
  if (_erase_tail == 0) {
    _str.append(data, len);
    return *this;
  }
  _str.insert(_erase_head + Size(), data, len);
  return *this;
}

void BufferLikeString::PushBack(char c) {
  if (_erase_tail == 0) {
    _str.push_back(c);
    return;
  }
  Data()[Size()] = c;
  --_erase_tail;
  Data()[Size()] = '\0';
}

BufferLikeString &BufferLikeString::Insert(size_t pos, const char *s, size_t n) {
  _str.insert(_erase_head + pos, s, n);
  return *this;
}

BufferLikeString &BufferLikeString::Assign(const char *data, size_t len) {
  if (len <= 0) {
    return *this;
  }
  if (data >= _str.data() && data < _str.data() + _str.size()) {
    _erase_head = data - _str.data();
    if (data + len > _str.data() + _str.size()) {
      throw std::out_of_range("BufferLikeString::assign out_of_range");
    }
    _erase_tail = _str.data() + _str.size() - (data + len);
    return *this;
  }
  _str.assign(data, len);
  _erase_head = 0;
  _erase_tail = 0;
  return *this;
}

BufferLikeString &BufferLikeString::Assign(const char *data) {
  return Assign(data, strlen(data));
}

void BufferLikeString::Clear() {
  _erase_head = 0;
  _erase_tail = 0;
  _str.clear();
}

size_t BufferLikeString::Capacity() const {
  return _str.capacity();
}

void BufferLikeString::Reserve(size_t size) {
  _str.reserve(size);
}

void BufferLikeString::Resize(size_t size, char c) {
  _str.resize(size, c);
  _erase_head = 0;
  _erase_tail = 0;
}

bool BufferLikeString::IsEmpty() const {
  return Size() <= 0;
}

std::string BufferLikeString::Substr(size_t pos, size_t n) const {
  if (n == std::string::npos) {
    //获取末尾所有的
    if (pos >= Size()) {
      throw std::out_of_range("BufferLikeString::substr out_of_range");
    }
    return _str.substr(_erase_head + pos, Size() - pos);
  }

  //获取部分
  if (pos + n > Size()) {
    throw std::out_of_range("BufferLikeString::substr out_of_range");
  }
  return _str.substr(_erase_head + pos, n);
}

RD_NAMESPACE_END
