#include "common/threading/work_stealing_thread_pool.h"

BEGIN_NAMESPACE_COMMON
    namespace Threading
    {
        // 定义线程局部变量
        thread_local std::size_t WorkStealingThreadPool::_thread_index = 0;
        thread_local bool WorkStealingThreadPool::_is_worker_thread = false;
    } // namespace Threading
END_NAMESPACE_COMMON