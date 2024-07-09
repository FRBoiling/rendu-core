#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#include <functional>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <list>
#include <cstdint>
#include <semaphore>
#include <shared_mutex>
#include <future>

 /**
* @class ThreadPool
* @brief Represents a thread pool for executing tasks concurrently.
*
* The ThreadPool class provides a simple interface for executing tasks concurrently using a pool of worker threads.
* The class allows submitting tasks to the thread pool, pausing and resuming the pool, and shutting down the pool.
*/
 class ThreadPool {
 private:
   class WorkerThread;        // 工作线程类
   enum class status_t : std::int8_t {
     TERMINATED = -1,
     TERMINATING = 0,
     RUNNING = 1,
     PAUSED = 2,
     SHUTDOWN = 3
   };  // 线程池的状态，-1: 线程池已终止；0: 线程池将终止；1: 线程池正在运行；2: 线程池被暂停；3: 线程池在等待任务完成，但不再接受新任务
   std::atomic<status_t> status;
   std::atomic<std::size_t> max_task_count;  // 任务队列中任务的最大数量，0表示没有限制    // 当任务队列中的任务数量将或已经超过此值时，新提交的任务将被拒绝
   std::shared_mutex status_mutex;         // 状态变量的互斥锁
   std::shared_mutex task_queue_mutex;         // 任务队列的互斥锁
   std::shared_mutex worker_list_mutex;        // 线程列表的互斥锁
   std::condition_variable_any task_queue_cv; // 任务队列的条件变量
   std::condition_variable_any task_queue_empty_cv; // 任务队列为空的条件变量
   std::queue<std::function<void()>> task_queue;      // 任务队列，其中存储着待执行的任务
   std::list<WorkerThread> worker_list;     // 线程列表，其中存储着工作线程
   // 禁用拷贝/移动构造函数及赋值运算符
   ThreadPool(const ThreadPool&) = delete;
   ThreadPool(ThreadPool&&) = delete;
   ThreadPool& operator=(const ThreadPool&) = delete;
   ThreadPool& operator=(ThreadPool&&) = delete;
   // 在取得对状态变量的独占访问后，调用以下函数，以确保线程池的状态变更是原子的
   void pause_with_status_lock();
   void resume_with_status_lock();
   void terminate_with_status_lock();
   void wait_with_status_lock();
   void shutdown_with_status_lock();
   void shutdown_now_with_status_lock();
 public:
   ThreadPool(std::size_t initial_thread_count, std::size_t max_task_count = 0);
   ~ThreadPool();

   template<typename F, typename... Args>
   auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>;
   void pause();
   void resume();
   void shutdown();    // 等待所有任务执行完毕后再终止线程池
   void shutdown_now();    // 立即终止线程池       // 会丢弃任务队列中的任务
   void terminate();   // 终止线程池
   void wait();    // 等待所有任务执行完毕
   void add_thread(std::size_t count_to_add);
   void remove_thread(std::size_t count_to_remove);
   void set_max_task_count(std::size_t count_to_set);
   std::size_t get_thread_count();
   std::size_t get_task_count();
 };



 /**
* @class ThreadPool::WorkerThread
* @brief Represents a worker thread in the thread pool.
*
* The WorkerThread class represents a worker thread in the thread pool. Each worker thread is responsible for executing tasks submitted to the thread pool.
*/
 class ThreadPool::WorkerThread
 {
 private:
   enum class status_t : std::int8_t {
     TERMINATED = -1,
     TERMINATING = 0,
     RUNNING = 1,
     PAUSED = 2,
     BLOCKED = 3
   };  // 状态变量类型，-1:线程已终止；0: 线程将终止；1: 线程正在运行; 2: 线程被暂停；3: 线程在阻塞等待新任务
   std::atomic<status_t> status;
   std::binary_semaphore pause_sem;       // 信号量，用于线程暂停时的阻塞
   std::shared_mutex status_mutex;         // 状态变量的互斥锁
   ThreadPool *pool;      // 线程池
   std::thread thread;     // 工作线程
   unsigned long m_id;

   // 禁用拷贝/移动构造函数及赋值运算符
   WorkerThread(const WorkerThread&) = delete;
   WorkerThread(WorkerThread&&) = delete;
   WorkerThread& operator=(const WorkerThread&) = delete;
   WorkerThread& operator=(WorkerThread&&) = delete;
   // 在取得对状态变量的独占访问后，调用以下函数，以确保线程的状态变更是原子的
   status_t terminate_with_status_lock();
   void pause_with_status_lock();
   void resume_with_status_lock();
 public:
   WorkerThread(ThreadPool* pool);
   ~WorkerThread();
   status_t terminate();
   void pause();
   void resume();
 };

 // inline/template function implementations

 // ThreadPool

 /**
* Submits a task to the thread pool for execution.
*
* This function submits a task to the thread pool for execution. The task is a callable object
* that takes arguments specified by the template parameters. The function returns a std::future
* object that can be used to retrieve the result of the task once it has completed.
*
* @tparam F The type of the callable object.
* @tparam Args The types of the arguments to the callable object.
* @param f The callable object to be executed.
* @param args The arguments to be passed to the callable object.
* @return A std::future object representing the result of the task.
* @throws std::runtime_error if the thread pool is in an invalid state or the task queue is full.
*/
 template<typename F, typename... Args>
 auto ThreadPool::submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
 {   // 提交任务
   std::shared_lock<std::shared_mutex> status_lock(status_mutex);  // 为状态变量加共享锁，以确保线程池的状态是稳定的
   switch (status.load())
   {
     case status_t::TERMINATED: // 线程池已终止
       throw std::runtime_error("[ThreadPool::submit][error]: thread pool is terminated");
     case status_t::TERMINATING: // 线程池将终止
       throw std::runtime_error("[ThreadPool::submit][error]: thread pool is terminating");
     case status_t::PAUSED: // 线程池被暂停
       throw std::runtime_error("[ThreadPool::submit][error]: thread pool is paused");
     case status_t::SHUTDOWN: // 线程池在等待任务完成，但不再接受新任务
       throw std::runtime_error("[ThreadPool::submit][error]: thread pool is waiting for tasks to complete, but not accepting new tasks");
     case status_t::RUNNING: // 线程池正在运行
       break;
     default:
       throw std::runtime_error("[ThreadPool::submit][error]: unknown status");
   }

   if (max_task_count > 0 && get_task_count() >= max_task_count)
   {   // 如果任务队列已满，则拒绝提交任务
     throw std::runtime_error("[ThreadPool::submit][error]: task queue is full");
   }
   using return_type = decltype(f(args...));
   auto task = std::make_shared<std::packaged_task<return_type()>>(
       std::bind(std::forward<F>(f), std::forward<Args>(args)...)
   );
   std::future<return_type> res = task->get_future();

   std::unique_lock<std::shared_mutex> lock(task_queue_mutex);
   task_queue.emplace([task](){ (*task)(); }); // 将任务封装为一个lambda表达式并放入任务队列   // 该lambda表达式会调用std::packaged_task对象的operator()方法，从而执行任务
   lock.unlock();
   task_queue_cv.notify_one();
   return res;
 }

 inline void ThreadPool::set_max_task_count(std::size_t count_to_set)
 {   // 设置任务队列中任务的最大数量；如果设置后的最大数量小于当前任务数量，则会拒绝新提交的任务，直到任务数量小于等于最大数量
   max_task_count.store(count_to_set);
 }


#endif // THREAD_POOL_H_