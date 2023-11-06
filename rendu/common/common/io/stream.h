/*
* Created by boil on 2023/10/17.
*/

#ifndef RENDU_COMMON_STREAM_H
#define RENDU_COMMON_STREAM_H

#include "common/define.h"

COMMON_NAMESPACE_BEGIN

    class Stream {

    public:
      void Write(BYTE *buffer, int offset, int count);

    };

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_STREAM_H
