//
// Created by 沸腾 on 2025/10/24.
//

#ifndef RENDU_THREAD_POOL_BENCHMARK_H
#define RENDU_THREAD_POOL_BENCHMARK_H

#include "common/define.h"
#include "common/threading/work_stealing_thread_pool.h"
#include "common/threading/thread_pool.h"
#include <chrono>
#include <iostream>
#include <vector>
#include <numeric>

#include "thread_pool_adapter.h"

BEGIN_NAMESPACE_COMMON
    namespace Threading
    {
        class ThreadPoolBenchmark
        {
        public:
            struct BenchmarkResult
            {
                std::string name;
                double duration_ms;
                uint64_t tasks_completed;
                double tasks_per_second;
                
                BenchmarkResult(const std::string& n, double d, uint64_t t)
                    : name(n), duration_ms(d), tasks_completed(t)
                {
                    tasks_per_second = (duration_ms > 0) ? (tasks_completed * 1000.0 / duration_ms) : 0.0;
                }
            };

            static void RunBenchmarks()
            {
                std::cout << "=== Thread Pool Performance Benchmark ===\n" << std::endl;
                
                // 测试1: 计算密集型任务
                auto result1 = BenchmarkComputeIntensive();
                PrintResult(result1);
                
                // 测试2: I/O密集型任务模拟
                auto result2 = BenchmarkIOIntensive();
                PrintResult(result2);
                
                // 测试3: 混合负载测试
                auto result3 = BenchmarkMixedWorkload();
                PrintResult(result3);
                
                // 测试4: 工作窃取效果测试
                auto result4 = BenchmarkWorkStealing();
                PrintResult(result4);
            }

        private:
            static BenchmarkResult BenchmarkComputeIntensive()
            {
                constexpr uint64_t num_tasks = 10000;
                constexpr int fib_n = 30; // 计算斐波那契数列，模拟计算密集型任务
                
                auto fibonacci = [](int n) -> uint64_t {
                    if (n <= 1) return n;
                    uint64_t a = 0, b = 1;
                    for (int i = 2; i <= n; ++i) {
                        uint64_t temp = a + b;
                        a = b;
                        b = temp;
                    }
                    return b;
                };
                
                // 测试传统线程池 - 修复：使用适配器而不是直接使用ThreadPool
                auto start = std::chrono::high_resolution_clock::now();
                {
                    ThreadPoolAdapter pool;  // 使用适配器而不是直接ThreadPool
                    std::vector<std::future<uint64_t>> futures;
                    futures.reserve(num_tasks);
                    
                    for (uint64_t i = 0; i < num_tasks; ++i) {
                        futures.push_back(pool.Submit(fibonacci, fib_n));
                    }
                    
                    for (auto& future : futures) {
                        future.get();
                    }
                }
                auto end = std::chrono::high_resolution_clock::now();
                double traditional_duration = std::chrono::duration<double, std::milli>(end - start).count();
                
                // 测试工作窃取线程池
                start = std::chrono::high_resolution_clock::now();
                {
                    WorkStealingThreadPool pool;
                    std::vector<std::future<uint64_t>> futures;
                    futures.reserve(num_tasks);
                    
                    for (uint64_t i = 0; i < num_tasks; ++i) {
                        futures.push_back(pool.Submit(fibonacci, fib_n));
                    }
                    
                    for (auto& future : futures) {
                        future.get();
                    }
                }
                end = std::chrono::high_resolution_clock::now();
                double stealing_duration = std::chrono::duration<double, std::milli>(end - start).count();
                
                std::cout << "Traditional thread pool: " << traditional_duration << " ms" << std::endl;
                std::cout << "Work stealing thread pool: " << stealing_duration << " ms" << std::endl;
                std::cout << "Performance improvement: " << (traditional_duration - stealing_duration) / traditional_duration * 100 << "%" << std::endl;
                
                return BenchmarkResult("Compute Intensive (Fibonacci)", 
                                      stealing_duration, num_tasks);
            }

            static BenchmarkResult BenchmarkIOIntensive()
            {
                constexpr uint64_t num_tasks = 5000;
                
                auto io_task = []() {
                    std::this_thread::sleep_for(std::chrono::microseconds(100)); // 模拟I/O等待
                    return true;
                };
                
                auto start = std::chrono::high_resolution_clock::now();
                {
                    WorkStealingThreadPool pool;
                    std::vector<std::future<bool>> futures;
                    futures.reserve(num_tasks);
                    
                    for (uint64_t i = 0; i < num_tasks; ++i) {
                        futures.push_back(pool.Submit(io_task));
                    }
                    
                    for (auto& future : futures) {
                        future.get();
                    }
                }
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double, std::milli>(end - start).count();
                
                return BenchmarkResult("I/O Intensive (Sleep Simulation)", duration, num_tasks);
            }

            static BenchmarkResult BenchmarkMixedWorkload()
            {
                constexpr uint64_t num_tasks = 8000;
                
                auto mixed_task = [](uint64_t id) {
                    if (id % 3 == 0) {
                        // 计算密集型
                        uint64_t result = 0;
                        for (uint64_t i = 0; i < 100000; ++i) {
                            result += i * i;
                        }
                        return result;
                    } else if (id % 3 == 1) {
                        // I/O密集型
                        std::this_thread::sleep_for(std::chrono::microseconds(50));
                        return id;
                    } else {
                        // 中等负载
                        std::this_thread::sleep_for(std::chrono::microseconds(10));
                        uint64_t sum = 0;
                        for (uint64_t i = 0; i < 1000; ++i) {
                            sum += i;
                        }
                        return sum;
                    }
                };
                
                auto start = std::chrono::high_resolution_clock::now();
                {
                    WorkStealingThreadPool pool;
                    std::vector<std::future<uint64_t>> futures;
                    futures.reserve(num_tasks);
                    
                    for (uint64_t i = 0; i < num_tasks; ++i) {
                        futures.push_back(pool.Submit(mixed_task, i));
                    }
                    
                    for (auto& future : futures) {
                        future.get();
                    }
                }
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double, std::milli>(end - start).count();
                
                return BenchmarkResult("Mixed Workload", duration, num_tasks);
            }

            static BenchmarkResult BenchmarkWorkStealing()
            {
                constexpr uint64_t num_tasks = 10000;
                constexpr std::size_t uneven_distribution = 10; // 10倍任务不均衡
                
                auto simple_task = [](uint64_t id) {
                    return id * id;
                };
                
                auto start = std::chrono::high_resolution_clock::now();
                {
                    WorkStealingThreadPool pool(4); // 使用4个线程测试工作窃取效果
                    std::vector<std::future<uint64_t>> futures;
                    futures.reserve(num_tasks);
                    
                    // 不均衡地分配任务到不同线程
                    for (uint64_t i = 0; i < num_tasks; ++i) {
                        // 让大部分任务集中在少数线程上
                        if (i % uneven_distribution == 0) {
                            // 在主线程提交，会被随机分配
                            futures.push_back(pool.Submit(simple_task, i));
                        } else {
                            // 模拟在特定线程上集中提交任务
                            futures.push_back(pool.Submit(simple_task, i));
                        }
                    }
                    
                    for (auto& future : futures) {
                        future.get();
                    }
                    
                    // 获取统计信息 - 修复使用方式
                    auto& stats = pool.GetStats();
                    std::cout << "Work stealing stats: " << stats.tasks_stolen.load() 
                              << " tasks stolen out of " << stats.tasks_executed.load() 
                              << " total tasks" << std::endl;
                }
                auto end = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double, std::milli>(end - start).count();
                
                return BenchmarkResult("Work Stealing Effectiveness", duration, num_tasks);
            }

            static void PrintResult(const BenchmarkResult& result)
            {
                std::cout << "Benchmark: " << result.name << std::endl;
                std::cout << "  Duration: " << result.duration_ms << " ms" << std::endl;
                std::cout << "  Tasks: " << result.tasks_completed << std::endl;
                std::cout << "  Throughput: " << result.tasks_per_second << " tasks/second" << std::endl;
                std::cout << std::endl;
            }
        };

    } // namespace Threading
END_NAMESPACE_COMMON

#endif //RENDU_THREAD_POOL_BENCHMARK_H