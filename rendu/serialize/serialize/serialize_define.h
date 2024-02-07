/*
* Created by boil on 2023/11/13.
*/

#ifndef RENDU_SERIALIZE_DEFINE_H_
#define RENDU_SERIALIZE_DEFINE_H_

#include "base/base.hpp"
#include "io/io.hpp"
#include "memory/memory.hpp"

#define SERIALIZE_NAMESPACE_BEGIN \
  namespace rendu {         \
    namespace serialize {

#define SERIALIZE_NAMESPACE_END \
  } /*namespace serialize*/     \
  } /*namespace rendu*/

using namespace rendu;
using namespace rendu::io;
using namespace rendu::memory;

#endif//RENDU_SERIALIZE_DEFINE_H_
