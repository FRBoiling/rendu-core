/*
* Created by boil on 2023/4/11.
*/

#ifndef RENDU_RENDER_SYSTEM_H
#define RENDU_RENDER_SYSTEM_H

#include "base/system.h"

class  RenderSystem final : public  System{
public:
    void run(entt::registry &registry) override;
};


#endif //RENDU_RENDER_SYSTEM_H
