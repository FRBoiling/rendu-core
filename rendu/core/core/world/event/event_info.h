/*
* Created by boil on 2023/10/13.
*/

#ifndef RENDU_EVENT_INFO_H
#define RENDU_EVENT_INFO_H

#include "../../entity/scene_type.h"
#include "core_define.h"

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

#endif //RENDU_EVENT_INFO_H
