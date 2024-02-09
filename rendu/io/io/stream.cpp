/*
* Created by boil on 2024/2/4.
*/

#include "stream.h"
#include <algorithm>// 用于std::min

IO_NAMESPACE_BEGIN

void Stream::CopyTo(Stream &destination) {
  CopyTo(destination, GetCopyBufferSize());
};

void Stream::CopyTo(Stream &destination, int buff_size) {
}

void Stream::WriteByte(byte b) {
  Write({b}, 0, 1);
};

int Stream::ReadByte() {
  std::vector<byte> oneByteArray(1);
  int r = Read(oneByteArray, 0, 1);
  return r == 0 ? -1 : static_cast<int>(oneByteArray[0]);
};

int Stream::Read(std::span<byte> buffer) {
  return -1;
}

void Stream::Write(const std::span<byte> buffer) {
}

int Stream::GetCopyBufferSize() {
  int bufferSize = DefaultCopyBufferSize;
  if (CanSeek()) {
    Long length = GetLength();
    Long position = GetPosition();
    if (length <= position)// Handles negative overflows
    {

      bufferSize = 1;
    } else {
      Long remaining = length - position;
      if (remaining > 0) {
        bufferSize = std::min((Long) bufferSize, remaining);
      }
    }
  }
  return bufferSize;
}

void Stream::ValidateBufferArguments(const byte *buffer, int offset, int count) {
  if (buffer == nullptr) {
    ThrowHelper::ThrowArgumentNullException(ExceptionArgument::buffer);
  }

  if (offset < 0) {
    ThrowHelper::ThrowArgumentOutOfRangeException(ExceptionArgument::offset, ExceptionResource::ArgumentOutOfRange_NeedNonNegNum);
  }

  if ((uint) count > sizeof(buffer) - offset) {
    ThrowHelper::ThrowArgumentOutOfRangeException(ExceptionArgument::count, ExceptionResource::Argument_InvalidOffLen);
  }
}

IO_NAMESPACE_END