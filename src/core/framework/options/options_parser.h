/*
* Created by boil on 2022/10/18.
*/

#ifndef RENDU_OPTIONS_PARSER_H_
#define RENDU_OPTIONS_PARSER_H_

#include "define.h"
#include "options.h"

namespace rendu {

  RD_COMMON_API void Show();
  RD_COMMON_API void Parse(int argc, char **argv,Options& options);

}//namespace rendu

#endif //RENDU_OPTIONS_PARSER_H_
