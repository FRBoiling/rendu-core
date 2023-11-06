/*
* Created by boil on 2023/10/26.
*/

#include "a_buffer.h"

RD_NAMESPACE_BEGIN

  StatisticImp(ABuffer)

  string ABuffer::ToString() const {
    return {Data(), Size()};
  }

  size_t ABuffer::GetCapacity() const {
    return Size();
  }
  
RD_NAMESPACE_END


