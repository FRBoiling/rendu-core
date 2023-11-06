/*
* Created by boil on 2023/10/26.
*/

#include "demangle.h"

RD_NAMESPACE_BEGIN

  string demangle(const char *mangled) {
    int status = 0;
    char *demangled = nullptr;
#if HAS_CXA_DEMANGLE
    demangled = abi::__cxa_demangle(mangled, nullptr, nullptr, &status);
#endif
    string out;
    if (status == 0 && demangled) { // Demangling succeeeded.
      out.append(demangled);
      free(demangled);
    } else {
      out.append(mangled);
    }
    return out;
  }

RD_NAMESPACE_END