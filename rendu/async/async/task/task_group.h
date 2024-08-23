/*
* Created by boil on 2024/7/7.
*/

#ifndef RENDU_ASYNC_ASYNC_TASK_TASK_GROUP_H_
#define RENDU_ASYNC_ASYNC_TASK_TASK_GROUP_H_

#include "thread/thread_object.h"
#include "thread_pool/thread_pool_define.h"
#include "utils/non_copyable.h"

RD_NAMESPACE_BEGIN

class TaskGroup : public ThreadObject {
public:
  explicit TaskGroup() = default;
  RD_NON_COPYABLE(TaskGroup)

  /**
     * 直接通过函数来申明taskGroup
     * @param task
     * @param ttl
     * @param onFinished
     */
  explicit TaskGroup(RD_DEFAULT_CONST_FUNCTION_REF task,
                     MSec ttl = RD_MAX_BLOCK_TTL,
                     RD_CALLBACK_CONST_FUNCTION_REF onFinished = nullptr) noexcept {
    this->addTask(task)
        ->setTtl(ttl)
        ->setOnFinished(onFinished);
  }

  /**
     * 添加一个任务
     * @param task
     */
  TaskGroup *addTask(RD_DEFAULT_CONST_FUNCTION_REF task) {
    task_arr_.emplace_back(task);
    return this;
  }

  /**
     * 设置任务最大超时时间
     * @param ttl
     */
  TaskGroup *setTtl(MSec ttl) {
    this->ttl_ = ttl;
    return this;
  }

  /**
     * 设置执行完成后的回调函数
     * @param onFinished
     * @return
     */
  TaskGroup *setOnFinished(RD_CALLBACK_CONST_FUNCTION_REF onFinished) {
    this->on_finished_ = onFinished;
    return this;
  }

  /**
     * 获取最大超时时间信息
     * @return
     */
  MSec getTtl() const {
    return this->ttl_;
  }

  /**
     * 清空任务组
     */
  Void clear() {
    task_arr_.clear();
  }

  /**
     * 获取任务组大小
     * @return
     */
  Size getSize() const {
    auto size = task_arr_.size();
    return size;
  }

private:
  std::vector<RD_DEFAULT_FUNCTION> task_arr_; // 任务消息
  MSec ttl_ = RD_MAX_BLOCK_TTL;               // 任务组最大执行耗时(如果是0的话，则表示不阻塞)
  RD_CALLBACK_FUNCTION on_finished_ = nullptr;// 执行函数任务结束

  friend class ThreadPool;
};

using TaskGroupPtr = TaskGroup *;
using TaskGroupRef = TaskGroup &;

RD_NAMESPACE_END

#endif//RENDU_ASYNC_ASYNC_TASK_TASK_GROUP_H_
