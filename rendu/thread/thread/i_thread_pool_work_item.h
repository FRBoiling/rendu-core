/*
* Created by boil on 2024/3/19.
*/

#ifndef RENDU_THREAD_THREAD_I_THREAD_POOL_WORK_ITEM_H_
#define RENDU_THREAD_THREAD_I_THREAD_POOL_WORK_ITEM_H_

#include "thread_define.h"

THREAD_NAMESPACE_BEGIN

class IThreadPoolWorkItem {
public:
  virtual void Execute() = 0;
};

THREAD_NAMESPACE_END

#endif//RENDU_THREAD_THREAD_I_THREAD_POOL_WORK_ITEM_H_
