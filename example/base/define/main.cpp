
#include "my_header.h"

int main() {
  // 使用函数模板变量tostr来调用std::to_string函数
  std::string s = tostr(123);
  std::cout << s << std::endl; // 输出123
  return 0;
}
