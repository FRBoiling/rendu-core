/*
* Created by boil on 2023/9/29.
*/

#include <iostream>
#include <thread>
#include <atomic>

#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#include <unistd.h>
#elif __APPLE__

#include "thread.h"
#include <sys/sysctl.h>
#include "common/define.h"
#include "synchronization_context.h"

#endif

COMMON_NAMESPACE_BEGIN
    namespace Environment {

// 定义一个函数，用于获取当前进程可用的处理器数量
        int GetProcessorCount() {
          // 使用 C++ 标准库提供的函数获取系统支持的最大并发线程数
          int max_threads = std::thread::hardware_concurrency();

          // 如果无法获取到最大并发线程数，则返回一个默认值
          if (max_threads == 0) {
            // 根据操作系统类型使用相应的 API 获取处理器数量
#ifdef _WIN32
            SYSTEM_INFO sys_info;
                        GetSystemInfo(&sys_info);
                        max_threads = sys_info.dwNumberOfProcessors;
#elif __linux__
            max_threads = sysconf(_SC_NPROCESSORS_ONLN);
#elif __APPLE__
            int nm[2];
            size_t len = 4;
            uint32_t count;

            nm[0] = CTL_HW;
            nm[1] = HW_AVAILCPU;
            sysctl(nm, 2, &count, &len, NULL, 0);

            if (count < 1) {
              nm[1] = HW_NCPU;
              sysctl(nm, 2, &count, &len, NULL, 0);
              if (count < 1) {
                count = 1;
              }
            }

            max_threads = count;
#else
            // 其他操作系统的处理器数量获取方式
                        max_threads = 1; // 默认值
#endif
          }

          return max_threads;
        }
    }


    std::map<std::thread::id, Thread *> Thread::m_threadsMap;

    void Thread::Sleep(int micro_seconds) {
      std::this_thread::sleep_for(std::chrono::microseconds(micro_seconds));
    }

    SynchronizationContext* Thread::SetSynchronizationContext(SynchronizationContext *synchronizationContext) {
        m_current_context = synchronizationContext;
        return m_current_context;
    }

    SynchronizationContext *Thread::GetSynchronizationContext() {
      return m_current_context;
    }

    Thread *Thread::GetCurrentThread() {
      auto it = m_threadsMap.find(std::this_thread::get_id());
      if (it == m_threadsMap.end()) {
        return Thread::Create();
      }
      return it->second;
    }




COMMON_NAMESPACE_END


