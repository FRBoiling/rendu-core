/*
* Created by boil on 2023/11/13.
*/

#include "path_helper.h"

#if defined(__MACH__) || defined(__APPLE__)

#include <limits>
#include <mach-o/dyld.h> /* _NSGetExecutablePath */

int uv_exepath(char *buffer, int *size) {
  /* realpath(exepath) may be > PATH_MAX so double it to be on the safe side. */
  char abspath[PATH_MAX * 2 + 1];
  char exepath[PATH_MAX + 1];
  uint32_t exepath_size;
  size_t abspath_size;

  if (buffer == nullptr || size == nullptr || *size == 0)
    return -EINVAL;

  exepath_size = sizeof(exepath);
  if (_NSGetExecutablePath(exepath, &exepath_size))
    return -EIO;

  if (realpath(exepath, abspath) != abspath)
    return -errno;

  abspath_size = strlen(abspath);
  if (abspath_size == 0)
    return -EIO;

  *size -= 1;
  if ((size_t) *size > abspath_size)
    *size = abspath_size;

  memcpy(buffer, abspath, *size);
  buffer[*size] = '\0';

  return 0;
}

#endif //defined(__MACH__) || defined(__APPLE__)

RD_NAMESPACE_BEGIN

std::string GetExePath(bool isExe/*= true*/) {
  char buffer[PATH_MAX * 2 + 1] = {0};
  int n = -1;
#if defined(_WIN32)
  n = GetModuleFileNameA(isExe?nullptr:(HINSTANCE)&__ImageBase, buffer, sizeof(buffer));
#elif defined(__MACH__) || defined(__APPLE__)
  n = sizeof(buffer);
  if (uv_exepath(buffer, &n) != 0) {
    n = -1;
  }
#elif defined(__linux__)
  n = readlink("/proc/self/exe", buffer, sizeof(buffer));
#endif

  std::string filePath;
  if (n <= 0) {
    filePath = "./";
  } else {
    filePath = buffer;
  }

#if defined(_WIN32)
  //windows下把路径统一转换层unix风格，因为后续都是按照unix风格处理的
  for (auto &ch : filePath) {
    if (ch == '\\') {
      ch = '/';
    }
  }
#endif //defined(_WIN32)

  return filePath;
}

std::string GetExeDir(bool isExe/*= true*/) {
  auto path = GetExePath(isExe);
  return path.substr(0, path.rfind('/') + 1);
}

std::string GetExeName(bool isExe/*= true*/) {
  auto path = GetExePath(isExe);
  return path.substr(path.rfind('/') + 1);
}

RD_NAMESPACE_END