/*
* Created by boil on 2024/2/5.
*/

#ifndef RENDU_BASE_BASE_CONTAINER_CONCURRENT_CONCURRENT_MAP_H_
#define RENDU_BASE_BASE_CONTAINER_CONCURRENT_CONCURRENT_MAP_H_

#include "base_define.h"
#include <shared_mutex>
#include <unordered_map>

RD_NAMESPACE_BEGIN

template <typename Key, typename Value>
class ConcurrentMap {
private:
  std::unordered_map<Key, Value> _map;
  mutable std::shared_mutex _mutex;

public:
public:
  std::optional<Value> Get(const Key& key) const {
    std::shared_lock lock(_mutex);
    if (const auto iter = _map.find(key); iter != _map.end()) {
      return iter->second;
    }
    return std::nullopt;
  }

  bool TryGetValue(const Key& key, Value& value) const {
    std::shared_lock lock(_mutex);
    if (auto iter = _map.find(key); iter != _map.end()) {
      value = iter->second;
      return true;
    }
    return false;
  }

  bool TryAdd(const Key& key, const Value& value){
    std::unique_lock lock(_mutex);
    if (_map.count(key) > 0) {
      // If the key already exists, we do not add anything and return false
      return false;
    }
    _map[key] = value;
    return true;
  }


  void Remove(Key& key) {
    std::unique_lock lock(_mutex);
    _map.erase(key);
  }
};

RD_NAMESPACE_END
#endif//RENDU_BASE_BASE_CONTAINER_CONCURRENT_CONCURRENT_MAP_H_
