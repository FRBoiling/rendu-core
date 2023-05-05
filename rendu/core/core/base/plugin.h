/*
* Created by boil on 2023/5/5.
*/

#ifndef RENDU_PLUGIN_H
#define RENDU_PLUGIN_H

#include "fwd/core_fwd.h"

namespace rendu {
    class Plugin {
    public:
        virtual void Mount(Host &host) = 0;
    };
}
#endif //RENDU_PLUGIN_H
