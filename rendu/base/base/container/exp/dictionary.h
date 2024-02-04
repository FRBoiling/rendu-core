/*
* Created by boil on 2023/11/13.
*/

#ifndef RENDU_BASE_CONTAINER_EXP_DICTIONARY_H_
#define RENDU_BASE_CONTAINER_EXP_DICTIONARY_H_

#include "base_define.h"
#include <unordered_map>

RD_NAMESPACE_BEGIN

template<typename TKey, typename TValue>
class CDictionary : std::unordered_map<TKey, TValue> {
public:
  bool TryGetValue(TKey key, TValue &value) {
    if (this->find(key) != this->end()) {
      value = this->find(key)->second;
      return true;
    }
    return false;
  }

  void Add(TKey key, TValue value) {
    this->insert(std::make_pair(key, value));
  }

  void Clear() {
    //TODO:BOIL 注意指针变量的clear
    this->clear();
  }

  void Remove(long i) {
    this->erase(this->begin() + i);
  }

  size_t Count(){
    return this->size();
  }

};

RD_NAMESPACE_END

#endif//RENDU_BASE_CONTAINER_EXP_DICTIONARY_H_
