/*
* Created by boil on 2023/10/15.
*/

#ifndef RENDU_MEMORY_BUFFER_H
#define RENDU_MEMORY_BUFFER_H

#include "serialize_define.h"

SERIALIZE_NAMESPACE_BEGIN



class MemoryBuffer : public MemoryStream {
public:
  MemoryBuffer(int size) {}

};

SERIALIZE_NAMESPACE_END


#endif//RENDU_MEMORY_BUFFER_H
