/*
* Created by boil on 2024/2/4.
*/

#ifndef RENDU_IO_IO_STREAM_H_
#define RENDU_IO_IO_STREAM_H_

#include "io_define.h"

IO_NAMESPACE_BEGIN

class Stream {
public:
  virtual void Write(BYTE *buffer, INT32 offset, INT32 count);
  virtual INT32 Read(BYTE *buffer, INT32 offset, INT32 count);

private:
};

IO_NAMESPACE_END

#endif//RENDU_IO_IO_STREAM_H_
