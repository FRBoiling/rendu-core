/*
* Created by boil on 2024/7/7.
*/

#ifndef RENDU_ASYNC_THREAD_THREAD_OBJECT_H_
#define RENDU_ASYNC_THREAD_THREAD_OBJECT_H_

#include "thread_define.h"

RD_ASYNC_NAMESPACE_BEGIN

class ThreadObject : public Object {
protected:
  /**
     * 部分thread中的算子，可以不实现run方法
     * @return
     */
  Status run() override {
    RD_NO_SUPPORT
  }
};

RD_ASYNC_NAMESPACE_END

#endif//RENDU_ASYNC_THREAD_THREAD_OBJECT_H_
