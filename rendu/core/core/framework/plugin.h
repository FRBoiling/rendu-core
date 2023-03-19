/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_CORE_CORE_FRAMEWORK_PLUGIN_H_
#define RENDU_CORE_CORE_FRAMEWORK_PLUGIN_H_


namespace rendu {

class main_loop;
class plugin {
 public:
  virtual void mount(main_loop &game_loop) = 0;
};

}

#endif //RENDU_CORE_CORE_FRAMEWORK_PLUGIN_H_
