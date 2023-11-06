/*
* Created by boil on 2023/9/21.
*/

#ifndef RENDU_CONSOLE_ARGUMENT_PARSER_SYSTEM_H
#define RENDU_CONSOLE_ARGUMENT_PARSER_SYSTEM_H

#include "base/string/singleton.h"
#include "entity/component_system.h"
#include "fwd/core_fwd.h"
#include "options.pb.h"

namespace rendu {

    class ConsoleArgumentParserSystem
        : public Singleton<ConsoleArgumentParserSystem>,
          public ComponentSystem<proto::core::Options>,
          public SystemArgsAwake<int, char **> {
    public:
      void Awake(int argc, char **argv) override;

      void Show();

    private:
      void Parser(int argc, char **argv);

    private:
      proto::core::Options *m_options;
    };

}// namespace rendu

#endif //RENDU_CONSOLE_ARGUMENT_PARSER_SYSTEM_H
