/*
* Created by boil on 2023/9/19.
*/

#ifndef RENDU_WORLD_H
#define RENDU_WORLD_H

#include "core_define.h"
#include "fwd/core_fwd.h"
#include "i_singleton_awake.h"
#include <typeindex>
#include <unordered_map>

CORE_NAMESPACE_BEGIN

class World {
public:
  World();
  ;

private:
  ~World();
  ;

private:
  CStack<STRING> stack;
  CDictionary<STRING, ASingleton> singletons;

public:
  static World &Instance();

public:
  template<class T, typename... Args>
  T AddSingleton(Args... args) {
    static_assert(std::is_base_of<ASingleton, T>::value, "T must inherit from ASingleton");
    static_assert(std::is_base_of<ISingletonAwake<Args...>, T>::value, "T must inherit from ISingletonAwake");

    T *singleton = new T();
    singleton->Awake(args...);
    // 注意, 需要自己实现 AddSingleton 这个函数
    AddSingleton(singleton);

    return singleton;
  }

  template<class T>
  T *AddSingleton() {
    static_assert(std::is_base_of<ASingleton, T>::value, "T must inherit from ASingleton");
    static_assert(std::is_base_of<ISingletonAwake<void>, T>::value, "T must inherit from ISingletonAwake");

    T *singleton = new T();
    singleton->Awake();

    // 注意, 需要自己实现 AddSingleton 这个函数
    AddSingleton(singleton);

    return singleton;
  }

  void AddSingleton(ASingleton singleton) {
//    const std::type_info& t = typeid(singleton);
//    STRING type = t.name();
//    if ("ISingletonReverseDispose" == type ) {
//      stack.Push(type);
//    }
//    singletons[type] = singleton;
//    singleton.Register();
  }
};

CORE_NAMESPACE_END

#endif//RENDU_WORLD_H
