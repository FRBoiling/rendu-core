/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_CORE_BASE_PLUGIN_H_
#define RENDU_CORE_BASE_PLUGIN_H_

namespace rendu {

class host;

class plugin {
 public:
  virtual void mount(host &game_loop) = 0;
};

}

#endif //RENDU_CORE_BASE_PLUGIN_H_
