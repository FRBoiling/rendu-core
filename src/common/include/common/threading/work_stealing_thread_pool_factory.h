//
// Created by 沸腾 on 2025/12/7.
//

#ifndef RENDU_WORK_STEALING_THREAD_POOL_FACTORY_H
#define RENDU_WORK_STEALING_THREAD_POOL_FACTORY_H

#include "work_stealing_thread_pool_interface.h"
#include <thread>

BEGIN_NAMESPACE_COMMON
    namespace Threading
    {
        // 工厂函数声明
        std::unique_ptr<IWorkStealingThreadPool> CreateWorkStealingThreadPool(
            WorkStealingThreadPoolType type = WorkStealingThreadPoolType::Basic,
            std::size_t num_threads = std::thread::hardware_concurrency());


    }

END_NAMESPACE_COMMON

#endif //RENDU_WORK_STEALING_THREAD_POOL_FACTORY_H