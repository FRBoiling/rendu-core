/*
* Created by boil on 2023/4/9.
*/

#include "base_plugin.h"
#include "game_plugin.h"

int main() {
    BasePlugin::config config = {
            .win_title = "Warmonger Dynasty",
            .cap_fps = 60
    };
    BasePlugin base_plugin(config);
    game_plugin game_plugin;

    game_loop()
            .add_plugin(base_plugin)
            .add_plugin(game_plugin)
            .run();

    return 0;
}
