/*
* Created by boil on 2023/9/21.
*/

#include "console_argument_parser_system.h"
#include <argparse/argparse.hpp>
#include "log/logger.h"
#include "world/world.h"
#include "world/logger/logger_system.h"

using namespace rendu;


template<>
struct fmt::formatter<proto::core::Options> : formatter<std::string> {
  auto format(proto::core::Options& options, format_context &ctx) -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(),
                          "[程序类型：{}]-[区：{}][服：{}][进程编号：{}]-[运行模式：{}]\n[配置文件目录：{}]",
                          proto::core::AppType_descriptor()->FindValueByNumber(options.apptype())->name(),
                          options.process(),
                          proto::core::DevelopModeType_descriptor()->FindValueByNumber(options.develop())->name(),
                          options.loglevel(),
                          options.console(),
                          options.startconfig()
    );
  }
};

void ConsoleArgumentParserSystem::Show(proto::core::Options& options) {
  RD_INFO("\n{}\n{}\n{}\n{}\n{}\n{}\n 进程名称：{}",
          options,
          "*************************************************",
          "             R E N D U    C O R E                ",
          "   https://github.com/FRBoiling/rendu-core.git   ",
          "*************************************************",
          " <Ctrl-C> to stop.",
          "1"
  );
}


void ConsoleArgumentParserSystem::Awake(int argc,char **argv) {
   Show(Parser(argc,argv));
}

proto::core::Options& ConsoleArgumentParserSystem::Parser(int argc, char **argv) {

  argparse::ArgumentParser parser("allowed options");
  parser.add_argument("--AppType", "-a").help("程序类型").required()
      .default_value(proto::core::AppType::Server);
  parser.add_argument("--StartConfig", "-s").help("配置文件目录").required()
      .default_value(std::string("StartConfig/Localhost"));
  parser.add_argument("--Process", "-p").help("进程号").required()
      .default_value(1).scan<'i', int>();
  parser.add_argument("--Develop", "-d").help("运行模式, 0正式 1开发 2压测").required()
      .default_value(proto::core::DevelopModeType::Develop);
  parser.add_argument("--LogLevel", "-l").help("日志等级").required()
      .default_value(1).scan<'i', int>();
  parser.add_argument("--Console", "-c").help("命令行").required()
      .default_value(1).scan<'i', int>();
  try {
    parser.parse_args(argc, argv);
  }
  catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << parser;
    std::exit(1);
  }
  auto entity = World::Instance().GetEntity();
  m_options = &m_entityPool->emplace_or_replace<proto::core::Options>(entity);
  m_options->set_apptype(parser.get<proto::core::AppType>("-a"));
  m_options->set_startconfig(parser.get<std::string>("-s"));
  m_options->set_process(parser.get<int>("-p"));
  m_options->set_develop(parser.get<proto::core::DevelopModeType>("-d"));
  m_options->set_loglevel(parser.get<int>("-l"));
  m_options->set_console(parser.get<int>("-c"));
  return *m_options;
}






