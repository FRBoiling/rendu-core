/*
* Created by boil on 2024/1/29.
*/
#include "current_thread.h"

THREAD_NAMESPACE_BEGIN

namespace CurrentThread {
__thread int t_cachedTid = 0;
__thread char t_tidString[32];
__thread int t_tidStringLength = 6;
__thread const char *t_threadName = "unknown";
static_assert(std::is_same<int, pid_t>::value, "pid_t should be int");

STRING stackTrace(bool demangle) {
  STRING stack;
//  const int max_frames = 200;
//  void *frame[max_frames];
//  int nptrs = ::backtrace(frame, max_frames);
//  char **STRINGs = ::backtrace_symbols(frame, nptrs);
//  if (STRINGs) {
//    size_t len = 256;
//    char *demangled = demangle ? static_cast<char *>(::malloc(len)) : nullptr;
//    for (int i = 1; i < nptrs; ++i)  // skipping the 0-th, which is this function
//    {
//      if (demangle) {
//        // https://panthema.net/2008/0901-stacktrace-demangled/
//        // bin/exception_test(_ZN3Bar4testEv+0x79) [0x401909]
//        char *left_par = nullptr;
//        char *plus = nullptr;
//        for (char *p = STRINGs[i]; *p; ++p) {
//          if (*p == '(')
//            left_par = p;
//          else if (*p == '+')
//            plus = p;
//        }
//
//        if (left_par && plus) {
//          *plus = '\0';
//          int status = 0;
//          char *ret = abi::__cxa_demangle(left_par + 1, demangled, &len, &status);
//          *plus = '+';
//          if (status == 0) {
//            demangled = ret;  // ret could be realloc()
//            stack.append(STRINGs[i], left_par + 1);
//            stack.append(demangled);
//            stack.append(plus);
//            stack.push_back('\n');
//            continue;
//          }
//        }
//      }
//      // Fallback to mangled names
//      stack.append(STRINGs[i]);
//      stack.push_back('\n');
//    }
//    free(demangled);
//    free(STRINGs);
//  }
  return stack;
}

THREAD_NAMESPACE_END

}  // namespace CurrentThread