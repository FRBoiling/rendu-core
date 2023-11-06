/*
* Created by boil on 2023/10/15.
*/

#ifndef RENDU_COMMON_DICTIONARY_H
#define RENDU_COMMON_DICTIONARY_H

#include "common/define.h"
#include <unordered_map>

COMMON_NAMESPACE_BEGIN

    template<typename TKey, typename TValue>
    class Dictionary : std::unordered_map<TKey, TValue> {
    public:
      bool TryGetValue(TKey key, TValue &value) {
        return false;
      }

      void Add(TKey key, TValue value) {

      }

      void Clear() {
        //TODO:BOIL 注意指针变量的clear
      }

      void Clean() {
        //TODO:BOIL 注意指针变量的clear
      }

      void Remove(long i) {

      }
    };

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_DICTIONARY_H
