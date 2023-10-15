/*
* Created by boil on 2023/10/15.
*/

#ifndef RENDU_DICTIONARY_H
#define RENDU_DICTIONARY_H

#include "define.h"
#include <unordered_map>

RD_NAMESPACE_BEGIN

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

RD_NAMESPACE_END

#endif //RENDU_DICTIONARY_H
