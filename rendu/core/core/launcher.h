/*
* Created by boil on 2023/2/16.
*/

#ifndef RENDU_CORE_LAUNCHER_H_
#define RENDU_CORE_LAUNCHER_H_

#include "host/host_scheduler.h"
#include <csignal>

namespace rendu {

    template<typename T>
    class Singleton
    {
    public:
        static T& GetInstance()
        {
            static T instance;
            return instance;
        }

        Singleton(T&&) = delete;
        Singleton(const T&) = delete;
        void operator= (const T&) = delete;

    protected:
        Singleton() = default;
        virtual ~Singleton() = default;
    };


    class Launcher {
    private:
        Launcher()= default;
    public:
        static Launcher& GetInst(){
            static Launcher instance;
            return instance;
        }

    private:
        Host m_host;
        HostScheduler m_scheduler;
    public:
        template<typename T>
        Host &AddPlugin() {
            return m_host.AddPlugin<T>();
        }


        void Exit(int sigval) {
            m_scheduler.Exit();
        }

        void Run() {
            m_scheduler.Init(&m_host);
            m_scheduler.LaterInit();
            while (m_scheduler.IsRunning()) {
                m_scheduler.Update();
                m_scheduler.LaterUpdate();
            }
            m_scheduler.Stopping();
            m_scheduler.Release();
        }
    };
}

#endif //RENDU_CORE_LAUNCHER_H_
