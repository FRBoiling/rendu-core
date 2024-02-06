/*
* Created by boil on 2024/2/4.
*/

#include "memory_stream.h"
#include <algorithm> // 用于std::min
#include <cstring> // 用于std::memcpy
#include <stdexcept> // 用于异常

IO_NAMESPACE_BEGIN

MemoryStream::MemoryStream()
    : _origin(0), _position(0), _length(0),
      _capacity(1024), _expandable(true),
      _writable(true), _exposable(false),
      _isOpen(true) {
}

MemoryStream::MemoryStream(int capacity)
    : _origin(0), _position(0), _length(0),
      _capacity(capacity), _expandable(true),
      _writable(true), _exposable(false),
      _isOpen(true) {
  _buffer.reserve(_capacity); // 预留capacity字节的容量
}

MemoryStream::MemoryStream(std::span<std::byte> buffer, size_t index, size_t count, bool writable /*= true*/, bool publiclyVisible /*= false*/)
    : _origin(index), _position(index), _length(count),
      _capacity(buffer.size()), _expandable(false), // 使用buffer的实际大小作为_capacity
      _writable(writable), _exposable(publiclyVisible),
      _isOpen(true) {
  auto begin = buffer.begin() + index;
  auto end = buffer.begin() + index + count;
  _buffer.assign(begin, end);
}

MemoryStream::~MemoryStream() {
  // 如果有需要清理的资源，可以在这里实现
}

bool MemoryStream::CanRead() const {
  return _isOpen;
}

bool MemoryStream::CanWrite() const {
  return _isOpen && _writable;
}

bool MemoryStream::CanSeek() const {
  return _isOpen;
}

size_t MemoryStream::GetLength() const {
  return static_cast<size_t>(_length);
}

void MemoryStream::SetLength(size_t length) {
  if (!_isOpen || !_writable || (!_expandable && length > _buffer.size())) {
    throw std::runtime_error("Stream is not open, writable or expandable");
  }
  _length = length;
  _capacity = length;
  if (_position > _length) {
    _position = _length;
  }
}

size_t MemoryStream::GetPosition() const {
  return static_cast<size_t>(_position);
}

void MemoryStream::SetPosition(size_t position) {
  if (position > static_cast<size_t>(_capacity)) {
    throw std::out_of_range("Position is out of range.");
  }
  _position = position;
}

void MemoryStream::Seek(size_t offset, SeekOrigin origin) {
  int newPos = _position;
  switch (origin) {
    case SeekOrigin::Begin:
      newPos = _origin + offset;
      break;
    case SeekOrigin::Current:
      newPos += offset;
      break;
    case SeekOrigin::End:
      newPos = _length + offset;
      break;
  }
  if (newPos < 0) {
    throw std::out_of_range("Seek position cannot be negative.");
  }
  _position = newPos;
}


void MemoryStream::Write(std::span<std::byte> buffer, size_t offset, size_t count) {
  if (!CanWrite()) {
    throw std::runtime_error("Stream can not write.");
  }
  if (offset + count > buffer.size()) {
    throw std::out_of_range("Write operation exceeds buffer bounds.");
  }
  size_t requiredCapacity = _position + count;

  if (!_expandable && (requiredCapacity > _capacity)) {
    throw std::runtime_error("Stream is not expandable and data exceeds capacity.");
  }
  if (requiredCapacity > _buffer.size()) {
    if (!_expandable) {
      throw std::runtime_error("Stream is not expandable.");
    }
    _capacity = std::max(2 * _capacity, requiredCapacity); // 获取新的容量值
    _buffer.reserve(_capacity); // 预留新容量的空间
  }
  // 将数据写入缓冲区
  _buffer.insert(_buffer.begin() + _position, buffer.begin() + offset, buffer.begin() + offset + count);
  _position += count;
  _length = std::max(_length, _position);
}

size_t MemoryStream::Read(std::span<std::byte>& buffer, size_t offset, size_t count) {
  if (!_isOpen) {
    throw std::runtime_error("Stream is not open.");
  }
  if (offset + count > buffer.size()) {
    throw std::out_of_range("Read operation exceeds buffer bounds.");
  }

  size_t actualCount = std::min(count, _length - _position);
  std::copy(_buffer.begin() + _position, _buffer.begin() + _position + actualCount, buffer.begin() + offset);

  _position += actualCount;
  return actualCount;
}

IO_NAMESPACE_END