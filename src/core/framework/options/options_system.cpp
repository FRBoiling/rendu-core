/*
* Created by boil on 2022/9/4.
*/

#include "options_system.h"
#include "enum.h"
#include <iostream>
#include <ostream>
#include <git_revision.h>

using namespace rendu;

template<>
struct fmt::formatter<model::ProgramOption> : formatter<std::string> {
  auto format(model::ProgramOption options, format_context &ctx) -> decltype(ctx.out()) {
    return format_to(ctx.out(),
                     "[程序类型：{}]-[区：{}][服：{}][进程编号：{}]-[运行模式：{}]\n[配置文件目录：{}]",
                     enum_name(options.type()),
                     options.zone_id(),
                     options.server_id(),
                     options.process_num(),
                     enum_name(options.run_mode()),
                     options.config_path()
    );
  }

};

void OptionsSystem::Show() {
  RD_INFO("\n{}\n{}\n{}\n{}\n{}\n{}\n{}\n 进程名称：{}",
          GitRevision::GetFullVersion(),
          sOptions.m_program_option,
          "*************************************************",
          "             R E N D U    C O R E                ",
          "   https://github.com/FRBoiling/rendu-core.git   ",
          "*************************************************",
          " <Ctrl-C> to stop.", sOptions.m_program_option.name());
}

int OptionsSystem::Parse(int argc, char **argv) {

  ArgumentParser parser("allowed options");
  parser.add_argument("--program", "-p").help("程序类型").required()
      .default_value(enum_name(ProgramType::All));
  parser.add_argument("--zone", "-z").help("区").required()
      .default_value(1).scan<'i', int>();
  parser.add_argument("--server", "-s").help("服").required()
      .default_value(1).scan<'i', int>();
  parser.add_argument("--number", "-n").help("进程编号").required()
      .default_value(1).scan<'i', int>();
  parser.add_argument("--mode", "-m").help("运行模式, 0正式 1开发 2压测").required()
      .default_value(enum_name(RunModeType::Online));

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

  m_program_option.set_type(enum_cast<ProgramType&>(parser.get<std::string>("-p")).value());
  m_program_option.set_zone_id(parser.get<int>("-z"));
  m_program_option.set_server_id( parser.get<int>("-s"));
  m_program_option.set_process_num( parser.get<int>("-n"));
  m_program_option.set_run_mode(enum_cast<RunModeType&>(parser.get<std::string>("-m")).value());

  m_program_option.set_config_path(parser.get<std::string>("-c"));

  m_program_option.set_name(rendu::StringFormat("{}-{}-{}-{}",
                                                enum_name(m_program_option.type()),
                                                        m_program_option.zone_id(),
                                                        m_program_option.server_id(),
                                                        m_program_option.process_num()));
  return 1;
}


void OptionsSystem::Register() {

}

void OptionsSystem::Destroy() {

}
