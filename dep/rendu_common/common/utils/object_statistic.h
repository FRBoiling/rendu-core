/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_OBJECT_STATISTIC_H
#define RENDU_OBJECT_STATISTIC_H

#include "common/define.h"

RD_NAMESPACE_BEGIN

  template<class C>
  class ObjectStatistic {
  public:
    ObjectStatistic() {
      ++GetCounter();
    }

    ~ObjectStatistic() {
      --GetCounter();
    }

    static size_t Count() {
      return GetCounter().load();
    }

  private:
    static std::atomic<size_t> &GetCounter();
  };

#define StatisticImp(Type)  \
    template<> \
    std::atomic<size_t>& ObjectStatistic<Type>::GetCounter(){ \
        static std::atomic<size_t> instance(0); \
        return instance; \
    }

RD_NAMESPACE_END

#endif //RENDU_OBJECT_STATISTIC_H
