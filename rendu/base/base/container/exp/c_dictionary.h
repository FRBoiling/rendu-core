/*
* Created by boil on 2023/11/13.
*/

#ifndef RENDU_BASE_BASE_CONTAINER_EXP_C_DICTIONARY_H_
#define RENDU_BASE_BASE_CONTAINER_EXP_C_DICTIONARY_H_

#include "base/base_define.h"
#include <unordered_map>

RD_NAMESPACE_BEGIN

template<typename TKey, typename TValue>
class CDictionary : std::unordered_map<TKey, TValue> {
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

#endif//RENDU_BASE_BASE_CONTAINER_EXP_C_DICTIONARY_H_
