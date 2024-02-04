/*
* Created by boil on 2023/11/3.
*/

#ifndef RENDU_BASE_SINGLETON_H
#define RENDU_BASE_SINGLETON_H

#include "non_copyable.h"
#include "number/number_helper.h"
#include <pthread.h>
#include <stdlib.h>

RD_NAMESPACE_BEGIN

class ASingleton : public NonCopyable {
public:
  virtual ~ASingleton() { isDisposed = true; }

  virtual void Register() = 0;// 纯虚函数

protected:
  ASingleton() : isDisposed(false) {}

  bool isDisposed;
};

template<class T>
class Singleton : public ASingleton {
public:
  static T &GetInstance() {
    return instance;
  }

  Singleton(const Singleton &) = delete;
  Singleton &operator=(const Singleton) = delete;

  virtual ~Singleton() = default;

protected:
  static T instance;

  Singleton() = default;// 默认构造函数，用于创建实例

  void Register() override {
  }
};


RD_NAMESPACE_END

#endif//RENDU_BASE_SINGLETON_H
