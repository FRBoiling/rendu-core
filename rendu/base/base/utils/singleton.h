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
  virtual ~ASingleton() { }
};

template<class T>
class Singleton : public ASingleton {
public:
  static T &GetInstance() {
    static T instance;
    return instance;
  }

  virtual ~Singleton() = default;

protected:
  Singleton() =default;
};


RD_NAMESPACE_END

#endif//RENDU_BASE_SINGLETON_H
