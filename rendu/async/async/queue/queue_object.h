/*
* Created by boil on 2024/7/9.
*/

#ifndef RENDU_ASYNC_ASYNC_QUEUE_QUEUE_OBJECT_H_
#define RENDU_ASYNC_ASYNC_QUEUE_QUEUE_OBJECT_H_

#include "queue_define.h"
#include "thread/thread_object.h"
#include <queue>

RD_ASYNC_NAMESPACE_BEGIN

class QueueObject : public ThreadObject {
protected:
  std::mutex mutex_;
  std::condition_variable cv_;
};

RD_ASYNC_NAMESPACE_END


#endif//RENDU_ASYNC_ASYNC_QUEUE_QUEUE_OBJECT_H_
