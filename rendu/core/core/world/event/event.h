/*
* Created by boil on 2023/10/13.
*/

#ifndef RENDU_EVENT_H
#define RENDU_EVENT_H

#include "define.h"
#include "../../entity/scene_type.h"

RD_NAMESPACE_BEGIN

    class EventInfo {
    public:
      SceneType m_sceneType;

    public:
      explicit EventInfo(SceneType sceneType) {
        m_sceneType = sceneType;
      }
    };

RD_NAMESPACE_END

#endif //RENDU_EVENT_H
