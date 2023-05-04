/*
* Created by boil on 2023/5/2.
*/

#ifndef RENDU_CONFIG_PLUGIN_H
#define RENDU_CONFIG_PLUGIN_H

#include "base/plugin.h"

namespace rendu {

    class ConfigPlugin final : public Plugin{
    public:
        void Mount(Host &host) override;
    };

}
#endif //RENDU_CONFIG_PLUGIN_H
