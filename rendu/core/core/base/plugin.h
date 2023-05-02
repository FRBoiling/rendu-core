/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_CORE_BASE_PLUGIN_H_
#define RENDU_CORE_BASE_PLUGIN_H_


namespace rendu {
    class Host;

    class Plugin {
    public:
        virtual void Mount(Host &host) = 0;
    };
}

#endif //RENDU_CORE_BASE_PLUGIN_H_
