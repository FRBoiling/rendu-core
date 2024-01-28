
#include <iostream>
#include <format>

int main()
{
  std::string format_str = std::format("There are {} fools in the world",10);
  std::cout << format_str << std::endl;

  return 0;
}
