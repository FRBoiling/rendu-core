/*
* Created by boil on 2023/5/2.
*/

#ifndef RENDU_ENV_PLUGIN_H
#define RENDU_ENV_PLUGIN_H

#include "base/plugin.h"

namespace rendu {

    class EnvPlugin final : public Plugin {
    public:
        void Mount(Host &host) override;
    };
    
}

#endif //RENDU_ENV_PLUGIN_H
