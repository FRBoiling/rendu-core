#ifndef RENDU_ENGINE_EVENTS_H_
#define RENDU_ENGINE_EVENTS_H_

namespace rendu::Events::Engine
{
    struct Init {};
    struct Config {};
    struct Execute {};

    struct Tick {
        float deltaTime;
    };

    struct Update {
        float fixedTime;
    };

    struct Render {
        float deltaTime;
    };

    struct Finalize {};
    struct Shutdown {};
}

#endif // RENDU_ENGINE_EVENTS_H_
