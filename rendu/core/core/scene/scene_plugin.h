/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_CORE_SCENE_SCENE_PLUGIN_H_
#define RENDU_CORE_SCENE_SCENE_PLUGIN_H_

#include "base/plugin.h"
#include "scene/scene_system.h"

namespace rendu {

class scene_plugin final : public plugin {
 private:
  std::shared_ptr<scene_system> m_scene_manager{
      std::make_shared<scene_system>()
  };

 public:
  void mount(host &main_loop) override;

};

}

#endif //RENDU_CORE_SCENE_SCENE_PLUGIN_H_
