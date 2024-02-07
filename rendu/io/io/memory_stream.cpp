/*
* Created by boil on 2024/2/4.
*/

#include "memory_stream.h"
#include <algorithm>// 用于std::min
#include <stdexcept>// 用于异常

IO_NAMESPACE_BEGIN

#define BUFFER_GROWTH_SIZE 1024

MemoryStream::MemoryStream()
    : _origin(0),
      _capacity(1024), _expandable(true),
      _writable(true), _exposable(false),
      _isOpen(true) {
}

MemoryStream::MemoryStream(int capacity)
    : _origin(0),
      _capacity(capacity), _expandable(true),
      _writable(true), _exposable(false),
      _isOpen(true) {
}

MemoryStream::MemoryStream(std::span<byte> buffer, int index, int count, bool writable /*= true*/, bool publiclyVisible /*= false*/)
    : _origin(index),
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

void MemoryStream::SetLength(int length) {
  if (!_isOpen || !_writable || length > _buffer.size()) {
    throw std::runtime_error("Stream is not open, writable or length out of range");
  }
  m_length = length;
  if (m_position > m_length) {
    m_position = m_length;
  }
}

void MemoryStream::SetPosition(int position) {
  if (m_position > m_length) {
    throw std::out_of_range("Position is out of range.");
  }
  m_position = position;
}

void MemoryStream::Seek(int offset, SeekOrigin origin) {
  ssize_t newPos = 0;// use a signed integer to handle possible negative offset
  switch (origin) {
    case SeekOrigin::Begin:
      newPos = _origin + offset;
      break;
    case SeekOrigin::Current:
      newPos = m_position + offset;
      break;
    case SeekOrigin::End:
      newPos = m_length + offset;
      break;
  }
  if (newPos < 0 || newPos > GetLength()) {
    throw std::out_of_range("Seek position is out of range.");
  }
  m_position = newPos;
}

bool MemoryStream::EnsureCapacity(int required_capacity)
{
  if (required_capacity < 0)
    throw std::runtime_error("EnsureCapacity value <0 .");
  if (required_capacity <= _capacity)
    return false;
  int num = std::max(required_capacity, 256);
  if (num < this->_capacity * 2)
    num = this->_capacity * 2;
  if ((uLong)(this->_capacity * 2) > 2147483591U)
    num = std::max(required_capacity, 2147483591);
  this->_capacity = num;
  return true;
}

void MemoryStream::Write(const std::vector<byte> &buffer, int offset, int count) {
  if (!CanWrite()) {
    throw std::runtime_error("Stream cannot write.");
  }

  if (offset + count > buffer.size()) {
    throw std::out_of_range("Write operation exceeds buffer bounds.");
  }

  int requiredCapacity = m_position + count;
  if (requiredCapacity > _capacity) {
    if (!CheckBufferSizeAndAutoGrowth(requiredCapacity)) {
      throw std::runtime_error("Stream is not expandable and data exceeds capacity.");
    }
  }
  _buffer.insert(_buffer.begin() + m_position, buffer.begin() + offset, buffer.begin() + offset + count);
  m_position += count;
  if (m_position > m_length)
    m_length = m_position;
}

void MemoryStream::Write(const std::span<byte> buffer) {
  if (!CanWrite()) {
    throw std::runtime_error("Stream cannot write.");
  }
  int length = buffer.size();
  int requiredCapacity = m_position + length;
  if (requiredCapacity > _capacity) {
    if (!CheckBufferSizeAndAutoGrowth(requiredCapacity)) {
      throw std::runtime_error("Stream is not expandable and data exceeds capacity.");
    }
  }

  if (length > m_length)
  {
    bool flag = m_position > m_length;
    if (length > _capacity && this.EnsureCapacity(num))
      flag = false;
    if (flag)
      Array.Clear((Array) this._buffer, this._length, num - this._length);
    this._length = num;
  }
  buffer.CopyTo(new Span<byte>(this._buffer, this._position, buffer.Length));
  this._position = num;

  _buffer.insert(_buffer.begin() + m_position, buffer.begin(), buffer.end());
  m_position += count;
  if (m_position > m_length)
    m_length = m_position;
}

int MemoryStream::Read(std::vector<byte> &buffer, int offset, int count) {
  if (!CanRead()) {
    throw std::runtime_error("Stream is not open.");
  }
  if (count > buffer.size() - offset)
    throw std::runtime_error("Offset and count exceed buffer size.");
  int requiredCapacity = offset + count;
  if (requiredCapacity > buffer.size()) {
    throw std::runtime_error("Buffer is not expandable and data exceeds capacity.");
  }

  int read_count = std::min(count, m_length - m_position);
  std::copy(_buffer.begin() + m_position, _buffer.begin() + m_position + read_count, buffer.begin() + offset);
  m_position += read_count;

  return read_count;
}

int MemoryStream::Read(std::span<byte> buffer) {
  if (!CanRead()) {
    throw std::runtime_error("Stream is not open.");
  }
  int length = std::min(buffer.size(), m_length - m_position);
  if (length <= 0) {
    return 0;
  }
  std::copy_n(_buffer.begin() + m_position, length, buffer.begin());
  m_position += length;
  return length;
}

std::vector<byte> &MemoryStream::GetBuffer() {
  if (!_exposable) {
    throw std::runtime_error("Trying to access an unexposed buffer.");
  }
  return _buffer;
};

bool MemoryStream::TryGetBuffer(std::vector<byte> *&buffer) {
  if (!_exposable) {
    return false;
  }
  buffer = &_buffer;
  return true;
}

// 检查是否读到了流的末尾
bool MemoryStream::IsEndOfStream() const {
  return m_position >= m_length;
}

void MemoryStream::CopyTo(Stream &destination, int buff_size) {
  auto mem_dest = dynamic_cast<MemoryStream *>(&destination);
  if (!mem_dest) {
    throw std::invalid_argument("The provided destination is not a MemoryStream.");
  }

  std::vector<byte> buffer(buff_size);
  int read_count;

  // 从源复制数据，直到没有数据可读
  while ((read_count = Read(buffer, 0, buff_size)) > 0) {
    // 写入读取的数据到目标
    mem_dest->Write(std::span<byte>(buffer.data(), read_count));
  }
}

IO_NAMESPACE_END