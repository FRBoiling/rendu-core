/*
* Created by boil on 2022/9/4.
*/

#include "options.h"
#include "enum.h"
#include "string_format.h"
#include "argument_parser.h"
#include "log.h"
#include <iostream>
#include <ostream>
#include <git_revision.h>

template<>
struct fmt::formatter<rendu::Options> : formatter<std::string> {
  auto format(rendu::Options options, format_context &ctx) -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(),
                          "[程序类型：{}]-[区：{}][服：{}][进程编号：{}]-[运行模式：{}]\n[配置文件目录：{}]",
                          enum_name(options.m_program_type),
                          options.m_zone_id,
                          options.m_server_id,
                          options.m_process_num,
                          enum_name(options.m_run_mode),
                          options.m_config_path
    );
  }
};

namespace rendu {

  void Show() {
    RD_INFO("\n{}\n{}\n{}\n{}\n{}\n{}\n{}\n 进程名称：{}",
            GitRevision::GetFullVersion(),
            sOptions,
            "*************************************************",
            "             R E N D U    C O R E                ",
            "   https://github.com/FRBoiling/rendu-core.git   ",
            "*************************************************",
            " <Ctrl-C> to stop.", sOptions.m_program_name);
  }

  void Parse(int argc, char **argv, Options &options) {

    ArgumentParser parser("allowed options");
    parser.add_argument("--program", "-p").help("程序类型").required()
        .default_value(enum_flags_name(ProgramType::All));
    parser.add_argument("--zone", "-z").help("区").required()
        .default_value(1).scan<'i', int>();
    parser.add_argument("--server", "-s").help("服").required()
        .default_value(1).scan<'i', int>();
    parser.add_argument("--number", "-n").help("进程编号").required()
        .default_value(1).scan<'i', int>();
    parser.add_argument("--mode", "-m").help("运行模式, 0正式 1开发 2压测").required()
        .default_value(enum_flags_name(RunModeType::Develop));

    parser.add_argument("--config", "-c").help("配置文件目录").required()
        .default_value(std::string("../config"));


    try {
      parser.parse_args(argc, argv);
    }
    catch (const std::runtime_error &err) {
      std::cerr << err.what() << std::endl;
      std::cerr << parser;
      std::exit(1);
    }
    options.m_config_path = parser.get<std::string>("-c");

    options.m_program_type = enum_cast<ProgramType>(parser.get<std::string>("-p")).value();
    options.m_zone_id = parser.get<int>("-z");
    options.m_server_id = parser.get<int>("-s");
    options.m_process_num = parser.get<int>("-n");
    options.m_run_mode = enum_cast<RunModeType>(parser.get<std::string>("-m")).value();

    options.m_program_name = StringFormat("{0}-{1}-{2}-{3}",
                                          enum_name(options.m_program_type),
                                          options.m_zone_id,
                                          options.m_server_id,
                                          options.m_process_num);

  }
} //namespace rendu
