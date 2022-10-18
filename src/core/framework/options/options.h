/*
* Created by boil on 2022/9/4.
*/

#ifndef RENDU_OPTIONS_H_
#define RENDU_OPTIONS_H_

#include "program_type.h"
#include "run_mode.h"
#include "singleton.h"
#include <string>

namespace rendu {

  class Options : public Singleton<Options> {
  public:
    std::string m_program_name;

    ProgramType m_program_type;
    int32 m_zone_id;
    int32 m_server_id;
    int32 m_process_num;

    RunModeType m_run_mode;

    std::string m_config_path;
  };

#define sOptions Options::get_inst()
}//namespace rendu



#endif //RENDU_OPTIONS_H_
