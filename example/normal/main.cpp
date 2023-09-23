#include <cstdio>

#define DEFINE_TYPE(name, type) \
    typedef struct { \
        type value; \
    } name##_t;

DEFINE_TYPE(Int, int);
DEFINE_TYPE(Double, double);

#define FUNC(name, type) name##_##type
void FUNC(add, int)() {}//add_int()
void FUNC(add, float)() {}//add_float()

#include <argparse/argparse.hpp>

int main(int argc, const char* argv[]) {
  argparse::ArgumentParser program("argparse_example");
  program.add_argument("--name")
      .help("Enter your name")
      .default_value(std::string("World"));

  try {
    program.parse_args(argc, argv);
  }
  catch (const std::runtime_error& err) {
    std::cout << err.what() << std::endl;
    std::cout << program;
    exit(0);
  }

  auto name = program.get<std::string>("--name");
  Int_t i = {10};
  Double_t d = {3.1415926};
  printf("%d %lf\n", i.value, d.value);

  add_int();
  add_float();

  return 0;
}