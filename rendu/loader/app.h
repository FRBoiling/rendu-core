/*
* Created by boil on 2023/9/6.
*/

#ifndef RENDU_APP_H
#define RENDU_APP_H

#include "core/core.hpp"


class App {
public:
  static void Start(int argc, char **argv);

  static void Update();

  static void LateUpdate();
};


#endif//RENDU_APP_H
