/*
* Created by boil on 2023/9/28.
*/

#ifndef RENDU_CONCURRENT_MAP_H
#define RENDU_CONCURRENT_MAP_H

#include "define.h"

RD_NAMESPACE_BEGIN

    template<typename Key,typename Value>
    class ConcurrentMap {

    public:
      void TryGetValue(Key key, Value*& value) {

      }

      bool TryAdd(Key key, Value& value) {
        return true;
      }

      void Remove(Key key, Value& value) {

      }
    };

RD_NAMESPACE_END

#endif //RENDU_CONCURRENT_MAP_H
