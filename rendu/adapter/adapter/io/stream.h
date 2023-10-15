/*
* Created by boil on 2023/10/17.
*/

#ifndef RENDU_STREAM_H
#define RENDU_STREAM_H

#include "define.h"

RD_NAMESPACE_BEGIN

    class Stream {

    public:
      void Write(byte *buffer, int offset, int count);

    };

RD_NAMESPACE_END

#endif //RENDU_STREAM_H
