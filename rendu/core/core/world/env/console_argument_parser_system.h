/*
* Created by boil on 2023/9/21.
*/

#ifndef RENDU_CONSOLE_ARGUMENT_PARSER_SYSTEM_H
#define RENDU_CONSOLE_ARGUMENT_PARSER_SYSTEM_H

#include "define.h"

#include "options.pb.h"
#include "entity/component_system.h"
#include "base/singleton.h"

namespace rendu {

    class ConsoleArgumentParserSystem
        : public Singleton<ConsoleArgumentParserSystem>,
          public ComponentSystem<proto::core::Options>,
          public SystemArgsAwake<int, char **> {
    public:
      void Awake(int argc, char **argv) override;

    private:
      proto::core::Options &Parser(int argc, char **argv);

      void Show(proto::core::Options &options);

    private:
      proto::core::Options *m_options;
    };

}// namespace rendu

#endif //RENDU_CONSOLE_ARGUMENT_PARSER_SYSTEM_H
