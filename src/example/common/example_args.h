/*
* Created by boil on 2022/9/24.
*/
#pragma once
#include "argument_parser.h"
using namespace rendu;

void args_example(int argc, char **argv){
  ArgumentParser program("test");

  program.add_argument("numbers")
      .nargs(3)
      .scan<'i', int>();

  program.add_argument("-a")
      .default_value(false)
      .implicit_value(true);

  program.add_argument("-b")
      .default_value(false)
      .implicit_value(true);

  program.add_argument("-c")
      .nargs(2)
      .scan<'g', float>();

  program.add_argument("--files")
      .nargs(3);

  program.add_argument("-g").required()
      .default_value(0)
      .scan<'i', int>();

  try {
    program.parse_args(argc, argv);
  }
  catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(1);
  }
//  auto numbers = program.get<std::vector<int>>("numbers");        // {1, 2, 3}
  auto a = program.get<bool>("-a");                               // true
  auto b = program.get<bool>("-b");                               // true
  auto c = program.get<std::vector<float>>("-c");                 // {4.14f, 5.718f}
  auto files = program.get<std::vector<std::string>>("--files");  // {"a.txt", "b.txt", "c.txt"}
  auto g = program.get<int>("-g");             // 6

  std::cout<<program;
}

