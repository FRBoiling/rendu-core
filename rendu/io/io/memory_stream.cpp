/*
* Created by boil on 2024/2/4.
*/

#include "memory_stream.h"
#include <algorithm>// 用于std::min
#include <cstring>  // 用于std::memcpy
#include <stdexcept>// 用于异常


IO_NAMESPACE_BEGIN

#define BUFFER_GROWTH_SIZE 1024

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
}

MemoryStream::MemoryStream(std::span<std::byte> buffer, size_t index, size_t count, bool writable /*= true*/, bool publiclyVisible /*= false*/)
    : _origin(index), _position(index), _length(count),
      _capacity(buffer.size()), _expandable(false),// 使用buffer的实际大小作为_capacity
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
  if (!_isOpen || !_writable || length > _buffer.size()) {
    throw std::runtime_error("Stream is not open, writable or length out of range");
  }
  _length = length;
  if (_position > _length) {
    _position = _length;
  }
}

size_t MemoryStream::GetPosition() const {
  return static_cast<size_t>(_position);
}

void MemoryStream::SetPosition(size_t position) {
  if (position > GetLength()) {
    throw std::out_of_range("Position is out of range.");
  }
  _position = position;
}

void MemoryStream::Seek(size_t offset, SeekOrigin origin) {
  ssize_t newPos = 0;// use a signed integer to handle possible negative offset
  switch (origin) {
    case SeekOrigin::Begin:
      newPos = static_cast<ssize_t>(_origin) + offset;
      break;
    case SeekOrigin::Current:
      newPos = static_cast<ssize_t>(_position) + offset;
      break;
    case SeekOrigin::End:
      newPos = static_cast<ssize_t>(_length) + offset;
      break;
  }
  if (newPos < 0 || newPos > static_cast<ssize_t>(_length)) {
    throw std::out_of_range("Seek position is out of range.");
  }
  _position = static_cast<size_t>(newPos);
}

bool MemoryStream::CheckBufferSizeAndAutoGrowth(size_t required_capacity){
  if (!_expandable) {
    return false;
  }
  while (required_capacity > _capacity){
    _capacity += BUFFER_GROWTH_SIZE;// increase capacity
  }
  if (required_capacity <= _capacity){
    _buffer.resize(_capacity);       // resize the buffer, not just reserve
  }
  return true;
}

void MemoryStream::Write(const std::vector<std::byte>& buffer, size_t offset, size_t count) {
  if (!CanWrite()) {
    throw std::runtime_error("Stream cannot write.");
  }

  if (offset + count > buffer.size()) {
    throw std::out_of_range("Write operation exceeds buffer bounds.");
  }

  size_t requiredCapacity = _position + count;
  if (requiredCapacity > _capacity) {
    if (!CheckBufferSizeAndAutoGrowth(requiredCapacity)) {
      throw std::runtime_error("Stream is not expandable and data exceeds capacity.");
    }
  }
  _buffer.insert(_buffer.begin() + _position, buffer.begin() + offset, buffer.begin() + offset + count);
  _position += count;
  if (_position > _length)
    _length = _position;
}

void MemoryStream::Write(const std::span<std::byte> buffer){
  if (!CanWrite()) {
    throw std::runtime_error("Stream cannot write.");
  }
  int count = buffer.size();
  size_t requiredCapacity = _position + count;
  if (requiredCapacity > _capacity) {
    if (!CheckBufferSizeAndAutoGrowth(requiredCapacity)) {
      throw std::runtime_error("Stream is not expandable and data exceeds capacity.");
    }
  }
  _buffer.insert(_buffer.begin() + _position, buffer.begin(), buffer.end());
  _position += count;
  if (_position > _length)
    _length = _position;
}

size_t MemoryStream::Read(std::vector<std::byte>& buffer, size_t offset, size_t count) {
  if (!CanRead()) {
    throw std::runtime_error("Stream is not open.");
  }
  if (count > buffer.size() - offset)
    throw std::runtime_error("Offset and count exceed buffer size.");
  size_t requiredCapacity = offset + count;
  if (requiredCapacity > buffer.size()) {
    throw std::runtime_error("Buffer is not expandable and data exceeds capacity.");
  }

  size_t read_count = std::min(count, _length - _position);
  std::copy(_buffer.begin() + _position, _buffer.begin() + _position + read_count, buffer.begin() + offset);
  _position += read_count;

  return read_count;
}

size_t MemoryStream::Read(std::span<std::byte> buffer) {
  if (!CanRead()) {
    throw std::runtime_error("Stream is not open.");
  }
  if (buffer.size()> _length - _position){
    throw std::runtime_error("Buffer size exceeds data available for reading.");
  }

  size_t read_count = std::min(buffer.size(), _length - _position);
  std::copy(_buffer.begin() + _position, _buffer.begin() + _position + read_count, buffer.begin());
  _position += read_count;
  return read_count;
}

std::vector<std::byte> MemoryStream::GetBufferData() const {
  if(!_exposable) {
      throw std::runtime_error("Trying to access an unexposed buffer.");
  }

  return _buffer;
}

void MemoryStream::WriteByte(std::byte b) {
  std::vector<std::byte> buffer = {b};
  Write(buffer, 0, 1);
}

int MemoryStream::ReadByte() {
  if (IsEndOfStream()) {
      return -1;
  }
  std::vector<std::byte> buffer(1);
  Read(buffer, 0, 1);
  return static_cast<int>(buffer[0]);
}

// 检查是否读到了流的末尾
bool MemoryStream::IsEndOfStream() const {
  return _position >= _length;
}

void MemoryStream::CopyTo(Stream &destination, size_t buff_size) {
  auto mem_dest = dynamic_cast<MemoryStream*>(&destination);
  if (!mem_dest)
  {
      throw std::invalid_argument("The provided destination is not a MemoryStream.");
  }

  std::vector<std::byte> buffer(buff_size);
  size_t read_count;

  // 从源复制数据，直到没有数据可读
  while ((read_count = Read(buffer, 0, buff_size)) > 0)
  {
      // 写入读取的数据到目标
      mem_dest->Write(std::span<std::byte>(buffer.data(), read_count));
  }
}

IO_NAMESPACE_END