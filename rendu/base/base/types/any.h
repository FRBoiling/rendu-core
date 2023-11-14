/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_BASE_ANY_H
#define RENDU_BASE_ANY_H

#include "base/utils/demangle.h"

RD_NAMESPACE_BEGIN

// 可以保存任意的对象
class Any {
public:
  using Ptr = std::shared_ptr<Any>;

  Any() = default;
  ~Any() = default;

  Any(const Any &that) = default;
  Any(Any &&that) {
    _type = that._type;
    _data = std::move(that._data);
  }

  Any &operator=(const Any &that) = default;
  Any &operator=(Any &&that) {
    _type = that._type;
    _data = std::move(that._data);
    return *this;
  }

  template <typename T, typename... ArgsType>
  void set(ArgsType &&...args) {
    _type = &typeid(T);
    _data.reset(new T(std::forward<ArgsType>(args)...), [](void *ptr) { delete (T *)ptr; });
  }

  template <typename T>
  void set(std::shared_ptr<T> data) {
    if (data) {
      _type = &typeid(T);
      _data = std::move(data);
    } else {
      reset();
    }
  }

  template <typename T>
  T &get(bool safe = true) {
    if (!_data) {
      throw std::invalid_argument("Any is IsEmpty");
    }
    if (safe && !is<T>()) {
      throw std::invalid_argument("Any::get(): " + demangle(_type->name()) + " unable cast to " + demangle(typeid(T).name()));
    }
    return *((T *)_data.get());
  }

  template <typename T>
  const T &get(bool safe = true) const {
    return const_cast<Any &>(*this).get<T>(safe);
  }

  template <typename T>
  bool is() const {
    return _type && typeid(T) == *_type;
  }

  operator bool() const { return _data.operator bool(); }
  bool empty() const { return !bool(); }

  void reset() {
    _type = nullptr;
    _data = nullptr;
  }

  Any &operator=(nullptr_t) {
    reset();
    return *this;
  }

  STRING type_name() const {
    if (!_type) {
      return "";
    }
    return demangle(_type->name());
  }

private:
  const std::type_info* _type = nullptr;
  std::shared_ptr<void> _data;
};

RD_NAMESPACE_END

#endif //RENDU_BASE_ANY_H
