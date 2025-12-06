//
// Created by 沸腾 on 2025/10/24.
//

#include "common/threading/work_stealing_thread_pool_interface.h"  // 包含统一接口
#include "common/threading/work_stealing_thread_pool.h"
#include "common/threading/work_stealing_thread_pool_io.h"
#include "common/threading/work_stealing_thread_pool_advanced.h"
#include <thread>

BEGIN_NAMESPACE_COMMON
    namespace Threading
    {
        std::unique_ptr<IWorkStealingThreadPool> CreateWorkStealingThreadPool(
            WorkStealingThreadPoolType type,
            std::size_t num_threads)
        {
            switch (type)
            {
                case WorkStealingThreadPoolType::Basic:
                    return std::make_unique<WorkStealingThreadPool>(num_threads);
                
                case WorkStealingThreadPoolType::IO:
                    return std::make_unique<WorkStealingThreadPoolIO>(num_threads);
                
                case WorkStealingThreadPoolType::Advanced:
                    return std::make_unique<AdvancedWorkStealingThreadPool>(num_threads);
                
                default:
                    return std::make_unique<WorkStealingThreadPool>(num_threads);
            }
        }

    } // namespace Threading
END_NAMESPACE_COMMON