/*
* Created by boil on 2023/10/15.
*/

#ifndef RENDU_MEMORY_BUFFER_H
#define RENDU_MEMORY_BUFFER_H

#include "io/memory_stream.h"

RD_NAMESPACE_BEGIN
    enum class SeekOrigin {
      /// <summary>Specifies the beginning of a stream.</summary>
      Begin,
      /// <summary>Specifies the current position within a stream.</summary>
      Current,
      /// <summary>Specifies the end of a stream.</summary>
      End,
    };

    class MemoryBuffer : public MemoryStream {
    public:
      MemoryBuffer(int size) {}

      int capacity() {
        return 0;
      }

      void Seek(int i, SeekOrigin origin) {

      }

      void SetLength(int i) {

      }

      int GetLength() {
        return 0;
      }

      byte *GetBuffer() {
        return nullptr;
      }

      int GetPosition() {
        return 0;
      }
    };
RD_NAMESPACE_END


#endif //RENDU_MEMORY_BUFFER_H
