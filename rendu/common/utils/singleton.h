/*
* Created by boil on 2023/9/19.
*/

#ifndef RENDU_SINGLETON_H
#define RENDU_SINGLETON_H

#include <typeindex>
#include "define.h"

RD_NAMESPACE_BEGIN

    template<typename T>
    class Singleton {
    public:
      static T &Instance() {
        static T instance;
        return instance;
      }

      Singleton(T &&) = delete;

      Singleton(const T &) = delete;

      void operator=(const T &) = delete;

    protected:
      Singleton() = default;

      virtual ~Singleton() = default;
    };

RD_NAMESPACE_END

#endif //RENDU_SINGLETON_H
