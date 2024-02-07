/*
* Created by boil on 2023/10/15.
*/

#include "circular_buffer.h"


IO_NAMESPACE_BEGIN

CircularBuffer::CircularBuffer() : m_first_index(0),
                                   m_last_index(0),
                                   m_lastBuffer(nullptr) {
  AddLast();
}

CircularBuffer::~CircularBuffer() {
  while (!m_bufferQueue.empty()) {
    delete m_bufferQueue.front();
    m_bufferQueue.pop();
  }

  while (!m_bufferCache.empty()) {
    delete m_bufferCache.front();
    m_bufferCache.pop();
  }
  m_lastBuffer = nullptr;
}

void CircularBuffer::AddLast() {
  if (m_bufferCache.empty()) {
    // 缓冲池里没有的话，就新建一个
    m_bufferCache.push(new std::vector<byte>(ChunkSize));
  }
  // 从缓冲池尾部取出一个buffer，并将其移到buffer队列的队尾
  std::vector<byte> *buffer = m_bufferCache.back();
  m_bufferCache.pop();
  m_bufferQueue.push(buffer);
  m_lastBuffer = buffer;
}

void CircularBuffer::RemoveFirst() {
  if (!m_bufferQueue.empty()) {
    // 从buffer队列队首取出一个buffer，并将其移到buffer缓冲池的队尾
    std::vector<byte> *buffer = m_bufferQueue.front();
    m_bufferQueue.pop();
    m_bufferCache.push(buffer);
  }
}

std::vector<byte> &CircularBuffer::GetFirst() {
  if (m_bufferQueue.empty()) {
    AddLast();
  }
  return *m_bufferQueue.front();
}

std::vector<byte> &CircularBuffer::GetLast() {
  if (m_bufferQueue.size() == 0) {
    AddLast();
  }
  return *m_lastBuffer;
}


void CircularBuffer::SetFirstIndex(int index) {
  m_first_index = index;
}

int CircularBuffer::GetFirstIndex() const {
  return m_first_index;
}

Long CircularBuffer::GetLastIndex() const {
  return m_last_index;
}

void CircularBuffer::SetLastIndex(Long index) {
  m_last_index = index;
}

int CircularBuffer::GetLength() const {
  int c = 0;
  if (m_bufferQueue.size() == 0) {
    c = 0;
  } else {
    c = (m_bufferQueue.size() - 1) * ChunkSize + GetLastIndex() - GetFirstIndex();
  }
  if (c < 0) {
    RD_ERROR("CircularBuffer count < 0: {}, {}, {}", m_bufferQueue.size(), GetLastIndex(), GetFirstIndex());
  }
  return c;
}

bool CircularBuffer::CanRead() const {
  return true;
}
bool CircularBuffer::CanWrite() const {
  return true;
}
bool CircularBuffer::CanSeek() const {
  return false;
}

void CircularBuffer::Read(Stream *stream, int count) {
  if (count > GetLength()) {
    throw Exception("bufferList length < count, {} {}", GetLength(), count);
  }

  int alreadyCopyCount = 0;
  while (alreadyCopyCount < count) {
    int n = count - alreadyCopyCount;
    if (ChunkSize - m_first_index > n) {
      stream->Write(GetFirst(), m_first_index, n);
      m_first_index += n;
      alreadyCopyCount += n;
    } else {
      stream->Write(GetFirst(), m_first_index, ChunkSize - m_first_index);
      alreadyCopyCount += ChunkSize - m_first_index;
      m_first_index = 0;
      RemoveFirst();
    }
  }
}

int CircularBuffer::Read(std::vector<byte> &buffer, int offset, int count) {
  if (sizeof(buffer) < offset + count) {
    throw Exception("bufferList length < count, buffer length: {} {} {}", sizeof(buffer), offset, count);
  }

  int length = GetLength();
  if (length < count) {
    count = (int) length;
  }

  int alreadyCopyCount = 0;
  while (alreadyCopyCount < count) {
    int n = count - alreadyCopyCount;
    if (ChunkSize - m_first_index > n) {
      std::copy(GetFirst().begin() + m_first_index, GetFirst().begin() + m_first_index + n, buffer.begin() + alreadyCopyCount + offset);
      m_first_index += n;
      alreadyCopyCount += n;
    } else {
      std::copy(GetFirst().begin() + m_first_index, GetFirst().begin() + m_first_index + ChunkSize - m_first_index, buffer.begin() + alreadyCopyCount + offset);
      alreadyCopyCount += ChunkSize - m_first_index;
      m_first_index = 0;
      RemoveFirst();
    }
  }

  return count;
}


void CircularBuffer::Write(const std::vector<byte> &buffer, int offset, int count) {
  int alreadyCopyCount = 0;
  while (alreadyCopyCount < count) {
    if (m_last_index == ChunkSize) {
      AddLast();
      m_last_index = 0;
    }

    int n = count - alreadyCopyCount;
    if (ChunkSize - m_last_index > n) {
      std::copy(buffer.begin() + alreadyCopyCount + offset, buffer.begin() + alreadyCopyCount + offset + n, GetLast().begin() + m_last_index);
      m_last_index += count - alreadyCopyCount;
      alreadyCopyCount += n;
    } else {
      std::copy(buffer.begin() + alreadyCopyCount + offset, buffer.begin() + alreadyCopyCount + offset + ChunkSize - m_last_index, GetLast().begin() + m_last_index);
      alreadyCopyCount += ChunkSize - m_last_index;
      m_last_index = ChunkSize;
    }
  }
}


void CircularBuffer::Write(const std::span<byte> buffer) {
  int offset = 0;
  int count = buffer.size();

  int alreadyCopyCount = 0;
  while (alreadyCopyCount < count) {
    if (m_last_index == ChunkSize) {
      AddLast();
      m_last_index = 0;
    }

    int n = count - alreadyCopyCount;
    if (ChunkSize - m_last_index > n) {
      std::copy(buffer.begin() + alreadyCopyCount + offset, buffer.begin() + alreadyCopyCount + offset + n, GetLast().begin() + m_last_index);
      m_last_index += count - alreadyCopyCount;
      alreadyCopyCount += n;
    } else {
      std::copy(buffer.begin() + alreadyCopyCount + offset, buffer.begin() + alreadyCopyCount + offset + ChunkSize - m_last_index, GetLast().begin() + m_last_index);
      alreadyCopyCount += ChunkSize - m_last_index;
      m_last_index = ChunkSize;
    }
  }
}

int CircularBuffer::Read(std::span<byte> buffer) {
  int offset = 0;
  int count = buffer.size();

  if (buffer.size() < offset + count) {
    throw Exception("bufferList length < count, buffer length: {} {} {}", buffer.size(), offset, count);
  }

  int length = GetLength();
  if (length < count) {
    count = length;
  }

  int alreadyCopyCount = 0;
  while (alreadyCopyCount < count) {
    int n = count - alreadyCopyCount;
    if (ChunkSize - m_first_index > n) {
      std::copy(GetFirst().begin() + m_first_index, GetFirst().begin() + m_first_index + n, buffer.begin() + alreadyCopyCount + offset);
      m_first_index += n;
      alreadyCopyCount += n;
    } else {
      std::copy(GetFirst().begin() + m_first_index, GetFirst().begin() + m_first_index + ChunkSize - m_first_index, buffer.begin() + alreadyCopyCount + offset);
      alreadyCopyCount += ChunkSize - m_first_index;
      m_first_index = 0;
      RemoveFirst();
    }
  }

  return count;
}

IO_NAMESPACE_END