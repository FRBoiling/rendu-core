/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_ACTION_H
#define RENDU_ACTION_H

namespace input {
    struct action_trigger {
        bool active;
        bool performed;
        bool cancelled;
    };

    struct action_axis {
        float x;
        float y;
    };

    template <typename T>
    struct action_passthrough {
        T raw;
    };
}

#endif //RENDU_ACTION_H
