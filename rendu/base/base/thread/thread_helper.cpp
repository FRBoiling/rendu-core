/*
* Created by boil on 2023/11/14.
*/

#include "thread_helper.h"
#include "assert/assert_helper.h"

RD_NAMESPACE_BEGIN

void SetThreadName(const char *name) {
  assert(name);
#if defined(__linux) || defined(__linux__) || defined(__MINGW32__)
  pthread_setname_np(pthread_self(), limitString(name, 16).data());
#elif defined(__MACH__) || defined(__APPLE__)
  pthread_setname_np(LimitString(name, 32).data());
#elif defined(_MSC_VER)
  // SetThreadDescription was added in 1607 (aka RS1). Since we can't guarantee the user is running 1607 or later, we need to ask for the function from the kernel.
  using SetThreadDescriptionFunc = HRESULT(WINAPI *)(_In_ HANDLE hThread, _In_ PCWSTR lpThreadDescription);
  static auto setThreadDescription = reinterpret_cast<SetThreadDescriptionFunc>(::GetProcAddress(::GetModuleHandle("Kernel32.dll"), "SetThreadDescription"));
  if (setThreadDescription) {
    // Convert the thread name to Unicode
    wchar_t threadNameW[MAX_PATH];
    size_t numCharsConverted;
    errno_t wcharResult = mbstowcs_s(&numCharsConverted, threadNameW, name, MAX_PATH - 1);
    if (wcharResult == 0) {
      HRESULT hr = setThreadDescription(::GetCurrentThread(), threadNameW);
      if (!SUCCEEDED(hr)) {
        int i = 0;
        i++;
      }
    }
  } else {
    // For understanding the types and values used here, please see:
    // https://docs.microsoft.com/en-us/visualstudio/debugger/how-to-set-a-thread-name-in-native-code

    const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push, 8)
    struct THREADNAME_INFO {
      DWORD dwType = 0x1000;// Must be 0x1000
      LPCSTR szName;        // Pointer to name (in user address space)
      DWORD dwThreadID;     // Thread ID (-1 for caller thread)
      DWORD dwFlags = 0;    // Reserved for future use; must be zero
    };
#pragma pack(pop)

    THREADNAME_INFO info;
    info.szName = name;
    info.dwThreadID = (DWORD) -1;

    __try {
      RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), reinterpret_cast<const ULONG_PTR *>(&info));
    } __except (GetExceptionCode() == MS_VC_EXCEPTION ? EXCEPTION_CONTINUE_EXECUTION : EXCEPTION_EXECUTE_HANDLER) {
    }
  }
#else
  thread_name = name ? name : "";
#endif
}

STRING GetThreadName() {
#if ((defined(__linux) || defined(__linux__)) && !defined(ANDROID)) || (defined(__MACH__) || defined(__APPLE__)) || (defined(ANDROID) && __ANDROID_API__ >= 26) || defined(__MINGW32__)
  STRING ret;
  ret.resize(32);
  auto tid = pthread_self();
  pthread_getname_np(tid, (char *) ret.data(), ret.size());
  if (ret[0]) {
    ret.resize(strlen(ret.data()));
    return ret;
  }
  return std::to_string((uint64_t) tid);
#elif defined(_MSC_VER)
  using GetThreadDescriptionFunc = HRESULT(WINAPI *)(_In_ HANDLE hThread, _In_ PWSTR * ppszThreadDescription);
  static auto getThreadDescription = reinterpret_cast<GetThreadDescriptionFunc>(::GetProcAddress(::GetModuleHandleA("Kernel32.dll"), "GetThreadDescription"));

  if (!getThreadDescription) {
    std::ostringstream ss;
    ss << std::this_thread::get_id();
    return ss.str();
  } else {
    PWSTR data;
    HRESULT hr = getThreadDescription(GetCurrentThread(), &data);
    if (SUCCEEDED(hr) && data[0] != '\0') {
      char threadName[MAX_PATH];
      size_t numCharsConverted;
      errno_t charResult = wcstombs_s(&numCharsConverted, threadName, data, MAX_PATH - 1);
      if (charResult == 0) {
        LocalFree(data);
        std::ostringstream ss;
        ss << threadName;
        return ss.str();
      } else {
        if (data) {
          LocalFree(data);
        }
        return to_string((uint64_t) GetCurrentThreadId());
      }
    } else {
      if (data) {
        LocalFree(data);
      }
      return to_string((uint64_t) GetCurrentThreadId());
    }
  }
#else
  if (!thread_name.empty()) {
    return thread_name;
  }
  std::ostringstream ss;
  ss << std::this_thread::get_id();
  return ss.str();
#endif
}

bool SetThreadAffinity(int i) {
#if (defined(__linux) || defined(__linux__)) && !defined(ANDROID)
  cpu_set_t mask;
  CPU_ZERO(&mask);
  if (i >= 0) {
    CPU_SET(i, &mask);
  } else {
    for (auto j = 0u; j < thread::hardware_concurrency(); ++j) {
      CPU_SET(j, &mask);
    }
  }
  if (!pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask)) {
    return true;
  }
  LOG_WARN << "pthread_setaffinity_np failed: " << get_uv_errmsg();
#endif
  return false;
}

STRING LimitString(const char *name, size_t max_size) {
  STRING str = name;
  if (str.size() + 1 > max_size) {
    auto erased = str.size() + 1 - max_size + 3;
    str.replace(5, erased, "...");
  }
  return str;
}


RD_NAMESPACE_END