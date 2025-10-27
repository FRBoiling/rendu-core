//
// Created by 沸腾 on 2025/10/24.
//

#ifndef RENDU_BANNER_H
#define RENDU_BANNER_H

#include "common.h"
#include "define.h"

BEGIN_NAMESPACE_COMMON
    namespace Banner
    {
       RC_COMMON_API void Show(char const* applicationName, void(*log)(char const* text), void(*logExtraInfo)(char const* text));
    }
END_NAMESPACE_COMMON



#endif //RENDU_BANNER_H