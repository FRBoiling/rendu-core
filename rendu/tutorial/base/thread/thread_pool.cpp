#include "thread_pool.h"
#include <iostream>
#include "this_thread.h"

 /**
* @brief Constructs a thread pool object.
*
* This constructor initializes a thread pool object with the specified initial thread count and maximum task count.
*
* @param initial_thread_count The initial number of threads in the thread pool.
* @param max_task_count The maximum number of tasks that can be queued in the thread pool.
*/
 ThreadPool::ThreadPool(
     std::size_t initial_thread_count,
     std::size_t max_task_count
     ) :
         max_task_count(max_task_count),
         status(status_t::RUNNING)
 {
   for (std::size_t i = 0; i < initial_thread_count; ++i)
   {   // 创建线程并插入线程列表
     worker_list.emplace_back(this);     // 隐式调用worker_thread::worker_thread(ThreadPool* pool)构造函数
   }
 }

 ThreadPool::~ThreadPool()
 {
   terminate();    // 析构时终止线程池
 }


 /**
* Pauses the thread pool by setting its status to PAUSED and pausing all worker threads.
* If the thread pool is already in a paused state or terminated state, no further action is taken.
*
* @throws std::runtime_error if the thread pool is in an unknown status.
*/
 void ThreadPool::pause_with_status_lock()
 {
   switch (status.load())
   {
     case status_t::TERMINATED: // 线程池已终止
     case status_t::TERMINATING: // 线程池将终止
     case status_t::PAUSED: // 线程池被暂停
     case status_t::SHUTDOWN: // 线程池在等待任务完成，但不再接受新任务
       return;     // 在这些状态下，不需要进一步操作
     case status_t::RUNNING: // 线程池正在运行
       status.store(status_t::PAUSED);    // 设置线程池的状态为暂停
       break;
     default:
       throw std::runtime_error("[ThreadPool::pause][error]: unknown status");
   }
   std::unique_lock<std::shared_mutex> lock(worker_list_mutex);
   for (auto& worker : worker_list)
   {   // 暂停所有线程
     worker.pause();
   }
 }


 /**
* Resumes the thread pool by changing its status to RUNNING and resumes all worker threads.
* If the thread pool is already in a terminated, terminating, or running state, this function does nothing.
* If the thread pool is paused, it changes the status to RUNNING and resumes all worker threads.
* If the thread pool is in a shutdown state, it does nothing.
* If the thread pool is in an unknown state, it throws a std::runtime_error.
*
* @throws std::runtime_error if the thread pool is in an unknown state.
*/
 void ThreadPool::resume_with_status_lock()
 {
   switch (status.load())
   {
     case status_t::TERMINATED: // 线程池已终止
     case status_t::TERMINATING: // 线程池将终止
     case status_t::RUNNING: // 线程池正在运行
       return;     // 在这些状态下，不需要恢复线程
     case status_t::PAUSED: // 线程池被暂停
       status.store(status_t::RUNNING);    // 设置线程池的状态为运行
     case status_t::SHUTDOWN: // 线程池在等待任务完成，但不再接受新任务
       break;
     default:    // 未知状态
       throw std::runtime_error("[ThreadPool::resume][error]: unknown status");
   }
   std::unique_lock<std::shared_mutex> lock(worker_list_mutex);
   for (auto& worker : worker_list)
   {   // 恢复所有线程
     worker.resume();
   }
 }

 /**
* @brief Shuts down the thread pool and waits for all tasks to complete.
*
* This function sets the status of the thread pool to "SHUTDOWN" and waits for all tasks in the task queue to be completed.
* If the thread pool is already terminated, terminating, or in the process of shutting down, this function does nothing.
* If the thread pool is paused, it resumes the execution so that tasks can continue to be executed and consume the tasks in the task queue.
*
* @throws std::runtime_error if the thread pool is in an unknown status.
*/
 void ThreadPool::shutdown_with_status_lock()
 {   // 等待所有任务执行完毕后再终止线程池   // 设置线程池的状态为等待任务完成
   switch (status.load())
   {
     case status_t::TERMINATED: // 线程池已终止
     case status_t::TERMINATING: // 线程池将终止
     case status_t::SHUTDOWN: // 线程池在等待任务完成，但不再接受新任务
       return;     // 在这些状态下，不需要额外操作
     case status_t::PAUSED: // 线程池被暂停
       resume_with_status_lock();  // 恢复线程池运行，以便任务能够继续执行以消耗任务队列中的任务
     case status_t::RUNNING: // 线程池正在运行
       status.store(status_t::SHUTDOWN);    // 设置线程池的状态为等待任务完成
       break;
     default:    // 未知状态
       throw std::runtime_error("[ThreadPool::shutdown][error]: unknown status");
   }
   std::shared_lock<std::shared_mutex> lock(task_queue_mutex);
   while (!task_queue.empty())
   {   // 等待任务队列为空
     task_queue_empty_cv.wait(lock);
   }
   terminate_with_status_lock();
 }

 /**
* Waits for all tasks to complete while holding the status lock.
*
* This function waits for all tasks in the task queue to complete before returning.
* It checks the current status of the thread pool and performs different actions based on the status.
* If the status is TERMINATED, the function returns immediately as there is no need to wait.
* If the status is TERMINATING, PAUSED, SHUTDOWN, or RUNNING, the function continues to wait until the task queue is empty.
* If the status is unknown, a std::runtime_error is thrown.
*
* @throws std::runtime_error if the status is unknown.
*/
 void ThreadPool::wait_with_status_lock()
 {   // 等待所有任务执行完毕
   switch (status.load())
   {
     case status_t::TERMINATED:  // 线程池已终止
       return;     // 在这种状态下，不需要等待
     case status_t::TERMINATING: // 线程池将终止
     case status_t::PAUSED: // 线程池被暂停
     case status_t::SHUTDOWN: // 线程池在等待任务完成，但不再接受新任务
     case status_t::RUNNING: // 线程池正在运行
       break;
     default:
       throw std::runtime_error("[ThreadPool::wait][error]: unknown status");
   }
   std::shared_lock<std::shared_mutex> lock(task_queue_mutex);
   while (!task_queue.empty())
   {   // 等待任务队列为空
     task_queue_empty_cv.wait(lock);
   }
 }

 void ThreadPool::wait()
 {   // 等待所有任务执行完毕
   std::shared_lock<std::shared_mutex> lock(status_mutex); // wait操作不会改变线程池的状态，因此只需要共享锁
   wait_with_status_lock();
 }


 /**
* @brief Terminates the thread pool with a lock on the status.
*
* This function is responsible for terminating the thread pool by setting the status to TERMINATING,
* terminating all worker threads, notifying any blocked threads waiting for tasks, and setting the
* status to TERMINATED once all operations are completed.
*
* @throws std::runtime_error if the status is unknown.
*/
 void ThreadPool::terminate_with_status_lock()
 {
   switch (status.load())
   {
     case status_t::TERMINATED:  // 线程池已终止
       return;     // 在这种状态下，不需要进一步操作
     case status_t::TERMINATING: // 线程池将终止
       break;
     case status_t::RUNNING: // 线程池正在运行
     case status_t::PAUSED: // 线程池被暂停
     case status_t::SHUTDOWN: // 线程池在等待任务完成，但不再接受新任务
       status.store(status_t::TERMINATING);    // 设置线程池的状态为将终止
       break;
     default:    // 未知状态
       throw std::runtime_error("[ThreadPool::terminate][error]: unknown status");
   }
   std::unique_lock<std::shared_mutex> lock(worker_list_mutex);
   for (auto& worker : worker_list)
   {   // 终止所有线程
     worker.terminate();
   }
   task_queue_cv.notify_all(); // 唤醒所有等待任务的阻塞线程
   status.store(status_t::TERMINATED);    // 设置线程池的状态为已终止
 }

 void ThreadPool::shutdown()
 {   // 等待所有任务执行完毕后再终止线程池
   std::unique_lock<std::shared_mutex> lock(status_mutex);
   shutdown_with_status_lock();
 }

 inline void ThreadPool::shutdown_now_with_status_lock()
 {   // 立即终止线程池
   terminate_with_status_lock();
 }

 void ThreadPool::shutdown_now()
 {   // 立即终止线程池
   std::unique_lock<std::shared_mutex> lock(status_mutex);
   shutdown_now_with_status_lock();
 }

 void ThreadPool::pause()
 {   // 暂停线程池
   std::unique_lock<std::shared_mutex> lock(status_mutex);
   pause_with_status_lock();
 }

 void ThreadPool::resume()
 {   // 恢复线程池
   std::unique_lock<std::shared_mutex> lock(status_mutex);
   resume_with_status_lock();
 }

 void ThreadPool::terminate()
 {   // 终止线程池
   std::unique_lock<std::shared_mutex> lock(status_mutex);
   terminate_with_status_lock();
 }



 /**
* @brief Adds threads to the thread pool.
*
* This function adds the specified number of threads to the thread pool.
* The threads are created and added to the worker list.
*
* @param count_to_add The number of threads to add to the thread pool.
*
* @throws std::runtime_error if the thread pool is in an invalid state.
*/
 void ThreadPool::add_thread(std::size_t count_to_add)
 {   // 添加线程
   std::shared_lock<std::shared_mutex> lock_status(status_mutex);
   switch (status.load())
   {
     case status_t::TERMINATED: // 线程池已终止
     case status_t::TERMINATING: // 线程池将终止
     case status_t::PAUSED: // 线程池被暂停
       throw std::runtime_error("[ThreadPool::add_thread][error]: cannot add threads to the thread pool in this state");
     case status_t::RUNNING: // 线程池正在运行
     case status_t::SHUTDOWN: // 线程池在等待任务完成，但不再接受新任务
       break;
     default:
       throw std::runtime_error("[ThreadPool::add_thread][error]: unknown status");
   }
   std::unique_lock<std::shared_mutex> lock_worker_list(worker_list_mutex);
   for (std::size_t i = 0; i < count_to_add; ++i)
   {
     worker_list.emplace_back(this);
   }
 }


 /**
* Removes a specified number of threads from the thread pool.
*
* @param count_to_remove The number of threads to remove from the thread pool.
* @throws std::runtime_error if the thread pool is in an invalid state.
*/
 void ThreadPool::remove_thread(std::size_t count_to_remove)
 {   // 移除min(count_to_remove, worker_list.size())个线程
   std::shared_lock<std::shared_mutex> lock_status(status_mutex);
   switch (status.load())
   {
     case status_t::TERMINATED: // 线程池已终止
     case status_t::TERMINATING: // 线程池将终止
     case status_t::PAUSED: // 线程池被暂停
       throw std::runtime_error("[ThreadPool::remove_thread][error]: cannot remove threads from the thread pool in this state");
     case status_t::RUNNING: // 线程池正在运行
     case status_t::SHUTDOWN: // 线程池在等待任务完成，但不再接受新任务
       break;
     default:
       throw std::runtime_error("[ThreadPool::remove_thread][error]: unknown status");
   }
   std::unique_lock<std::shared_mutex> lock_worker_list(worker_list_mutex);
   count_to_remove = std::min(count_to_remove, worker_list.size());
   auto it = worker_list.end();
   for (std::size_t i = 0; i < count_to_remove; ++i)
   {
     --it;
     it->terminate();
   }
   task_queue_cv.notify_all(); // 唤醒所有等待任务的阻塞线程，以便它们能够检查线程是否被终止
   worker_list.erase(it, worker_list.end());   // 从线程列表中移除工作线程对象并析构
 }


 std::size_t ThreadPool::get_thread_count()
 {   // 获取线程池中线程的数量
   std::shared_lock<std::shared_mutex> lock(worker_list_mutex);
   return worker_list.size();
 }

 std::size_t ThreadPool::get_task_count()
 {   // 获取任务队列中任务的数量
   std::shared_lock<std::shared_mutex> lock(task_queue_mutex);
   return task_queue.size();
 }

 /**
* @brief Constructs a worker thread object for the thread pool.
*
* This constructor initializes a worker thread object for the thread pool. The worker thread is responsible for executing tasks from the task queue.
*
* @param pool A pointer to the thread pool that owns the worker thread.
*/
 ThreadPool::WorkerThread::WorkerThread(ThreadPool *pool)
     : pool(pool),
       status(status_t::RUNNING),
       pause_sem(0),
       thread(
           [this]() {// 线程的执行逻辑
             while (true) {
               std::unique_lock<std::shared_mutex> unique_lock_status(this->status_mutex);
               while (true) {
                 if (!unique_lock_status.owns_lock()) {
                   unique_lock_status.lock();
                 }
                 bool break_flag = false;
                 switch (this->status.load()) {
                   case status_t::TERMINATING:// 线程被设置为将终止
                     this->status.store(status_t::TERMINATED);
                   case status_t::TERMINATED:// 线程已终止
                     return;
                   case status_t::RUNNING:// 线程被设置为运行
                     break_flag = true;
                     break;
                   case status_t::PAUSED:// 线程被设置为暂停
                     unique_lock_status.unlock();
                     this->pause_sem.acquire();// 阻塞线程
                     break;
                   case status_t::BLOCKED:// 线程被设置为等待任务
                   default:               // 未知状态
                     unique_lock_status.unlock();
                     throw std::runtime_error("[ThreadPool::WorkerThread::WorkerThread][error]: undefined status");
                 }
                 if (break_flag) {
                   unique_lock_status.unlock();
                   break;
                 }
               }
               // 在运行状态下，从任务队列中取出一个任务并执行
               std::unique_lock<std::shared_mutex> unique_lock_task_queue(this->pool->task_queue_mutex);// 在取任务前，加锁
               while (this->pool->task_queue.empty()) {                                                 // 如果任务队列为空，则等待条件变量唤醒
                 while (true) {
                   if (!unique_lock_status.owns_lock()) {
                     unique_lock_status.lock();
                   }
                   bool break_flag = false;
                   switch (this->status.load()) {
                     case status_t::TERMINATING:// 线程被设置为将终止
                       this->status.store(status_t::TERMINATED);
                     case status_t::TERMINATED:// 线程已终止
                       return;
                     case status_t::PAUSED:// 线程被设置为暂停
                       unique_lock_status.unlock();
                       unique_lock_task_queue.unlock();
                       this->pause_sem.acquire();// 阻塞线程
                       unique_lock_task_queue.lock();
                       break;
                     case status_t::RUNNING:                 // 线程被设置为运行
                       this->status.store(status_t::BLOCKED);// 设置线程状态为等待任务
                     case status_t::BLOCKED:                 // 线程被设置为等待任务
                       break_flag = true;
                       break;
                     default:// 未知状态
                       unique_lock_status.unlock();
                       unique_lock_task_queue.unlock();
                       throw std::runtime_error("[ThreadPool::WorkerThread::WorkerThread][error]: unknown status");
                   }
                   if (break_flag) {
                     unique_lock_status.unlock();
                     break;
                   }
                 }
                 this->pool->task_queue_cv.wait(unique_lock_task_queue);// 等待条件变量唤醒；
                 while (true) {
                   if (!unique_lock_status.owns_lock()) {
                     unique_lock_status.lock();
                   }
                   bool break_flag = false;
                   switch (this->status.load()) {
                     case status_t::TERMINATING:// 线程被设置为将终止
                       this->status.store(status_t::TERMINATED);
                     case status_t::TERMINATED:// 线程已终止
                       return;
                     case status_t::PAUSED:// 线程被设置为暂停
                       unique_lock_status.unlock();
                       unique_lock_task_queue.unlock();
                       this->pause_sem.acquire();// 阻塞线程
                       unique_lock_task_queue.lock();
                       break;
                     case status_t::BLOCKED:                 // 线程被设置为等待任务
                       this->status.store(status_t::RUNNING);// 设置线程状态为运行
                     case status_t::RUNNING:                 // 线程被设置为运行
                       break_flag = true;
                       break;
                     default:// 未知状态
                       unique_lock_status.unlock();
                       throw std::runtime_error("[ThreadPool::WorkerThread::WorkerThread][error]: unknown status");
                   }
                   if (break_flag) {
                     unique_lock_status.unlock();
                     break;
                   }
                 }
               }
               // 取出一个任务
               try {
                 std::function<void()> task = std::move(this->pool->task_queue.front());
                 this->pool->task_queue.pop();
                 if (this->pool->task_queue.empty()) {// 如果任务队列为空，则通知任务队列空条件变量
                   this->pool->task_queue_empty_cv.notify_all();
                 }
                 unique_lock_task_queue.unlock(); // 取出任务后，释放锁
                 task();                          // 执行任务
               } catch (const std::exception &e) {// 如果任务执行过程中发生异常，则打印异常信息并继续循环
                 std::cerr << e.what() << '\n';
                 continue;
               }
             }
           }) {}


 /**
* @brief Destructor for the WorkerThread class.
*
* This destructor is responsible for terminating the worker thread and joining it if necessary.
* If the thread was previously blocked waiting for a task and is still blocked at the time of destruction,
* it will be woken up using a condition variable to avoid blocking the destruction process.
*/
 ThreadPool::WorkerThread::~WorkerThread() {
   status_t last_status = terminate();
   if (thread.joinable()) {
     if (last_status == status_t::BLOCKED) {// 如果线程之前在等待任务且析构时仍未被唤醒，则通过条件变量唤醒线程，以避免析构过程被阻塞
       pool->task_queue_cv.notify_all();
     }
     thread.join();
   }
 }

 /**
* @brief Terminates the worker thread.
*
* This function is used to terminate the worker thread. It sets the status of the worker thread to TERMINATING and returns the previous status.
* If the worker thread is currently running, it will be paused before termination.
*
* @return The previous status of the worker thread.
*/
 ThreadPool::WorkerThread::status_t ThreadPool::WorkerThread::terminate_with_status_lock() {
   status_t last_status = this->status.load();
   switch (last_status) {
     case status_t::TERMINATED: // 线程已终止
     case status_t::TERMINATING:// 线程将终止
       break;
     case status_t::RUNNING:// 线程正在运行
     case status_t::BLOCKED:// 线程在等待任务
       status.store(status_t::TERMINATING);
       break;
     case status_t::PAUSED:// 线程被暂停
       status.store(status_t::TERMINATING);
       pause_sem.release();
       break;
     default:
       throw std::runtime_error("[ThreadPool::WorkerThread::terminate][error]: unknown status");
   }
   return last_status;
 }

 ThreadPool::WorkerThread::status_t ThreadPool::WorkerThread::terminate() {
   std::unique_lock<std::shared_mutex> lock(status_mutex);
   return terminate_with_status_lock();
 }

 /**
* Pauses the worker thread by changing its status to PAUSED.
* If the thread's status is already TERMINATED, TERMINATING, or PAUSED, the function returns immediately.
* If the thread's status is BLOCKED or RUNNING, the function changes the status to PAUSED.
* If the thread's status is unknown, the function throws a std::runtime_error.
*/
 void ThreadPool::WorkerThread::pause_with_status_lock() {
   switch (status.load()) {
     case status_t::TERMINATED: // 线程已终止
     case status_t::TERMINATING:// 线程将终止
     case status_t::PAUSED:     // 线程被暂停
       return;
     case status_t::BLOCKED:// 线程在等待任务
     case status_t::RUNNING:// 线程正在运行
       status.store(status_t::PAUSED);
       break;
     default:
       throw std::runtime_error("[ThreadPool::WorkerThread::pause][error]: unknown status");
   }
 }

 void ThreadPool::WorkerThread::pause() {
   std::unique_lock<std::shared_mutex> lock(status_mutex);
   pause_with_status_lock();
 }

 /**
* Resumes the worker thread if it is currently paused.
* If the thread is not paused, blocked, running, or terminating, an exception is thrown.
*/
 void ThreadPool::WorkerThread::resume_with_status_lock() {
   switch (status.load()) {
     case status_t::TERMINATED: // 线程已终止
     case status_t::TERMINATING:// 线程将终止
     case status_t::RUNNING:    // 线程正在运行
     case status_t::BLOCKED:    // 线程在等待任务
       return;
     case status_t::PAUSED:// 线程被暂停
       status.store(status_t::RUNNING);
       pause_sem.release();// 唤醒线程
       break;
     default:
       throw std::runtime_error("[ThreadPool::WorkerThread::resume][error]: unknown status");
   }
 }

 void ThreadPool::WorkerThread::resume() {
   std::unique_lock<std::shared_mutex> lock(status_mutex);
   resume_with_status_lock();
 }
