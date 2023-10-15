///*
//* Created by boil on 2023/10/24.
//*/
//
//#ifndef RENDU_EVENT_LOOP_H
//#define RENDU_EVENT_LOOP_H
//
//#include "define.h"
//#include "event_handler.h"
//#include "file_event.h"
//
//RD_NAMESPACE_BEGIN
//
//  class IEventPoller;
//
//  class EventLoop {
//  public:
//    EventLoop() : m_poller(nullptr), m_api_state(nullptr), m_maxfd(0) {}
//
//    ~EventLoop() = default;
//
//  public:
//    EventLoop *Create(int setsize);
//
//  public:
//    int Loop();
//
//    int set_size;
//
//  public:
//    IEventPoller *m_poller;
//
//    ApiState *m_api_state;
//    int m_maxfd;   /* highest file descriptor currently registered */
//
//    FileEvent *m_events;
//    FiredEvent *m_fired;
//  };
//
//RD_NAMESPACE_END
//
//#endif //RENDU_EVENT_LOOP_H
