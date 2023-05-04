/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_PLUGIN_H
#define RENDU_PLUGIN_H

class game_loop;
class Plugin {
public:
    virtual void mount(game_loop &game_loop) = 0;
};

#endif //RENDU_PLUGIN_H
