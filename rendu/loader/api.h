/*
* Created by boil on 2023/9/6.
*/

#ifndef RENDU_API_H
#define RENDU_API_H

#include "define.h"

RD_NAMESPACE_BEGIN

    class Api {
    public :
      static void Start(int argc, char **argv);

      static void Update();

      static void LateUpdate();
    };

RD_NAMESPACE_END

#endif //RENDU_API_H
