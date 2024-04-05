/***************************************************************************
 * Copyright (C) 2023-, openlinyou, <linyouhappy@outlook.com>
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 ***************************************************************************/

#ifndef HEADER_OPEN_SERVER_H
#define HEADER_OPEN_SERVER_H

#include "opensocket.h"
#include "openthread.h"
#include "opentime.h"
#include <string.h>
#include <stdarg.h>
#include <map>

namespace open
{

////////////OpenSocketProto//////////////////////
struct OpenSocketProto : public OpenThreadProto
{
    std::shared_ptr<OpenSocketMsg> data_;
    static inline int ProtoType() { return 8888; }
    virtual inline int protoType() const { return OpenSocketProto::ProtoType(); }
};

////////////OpenTimerProto//////////////////////
struct OpenTimerProto : public OpenThreadProto
{
    int type_;
    int64_t deadline_;
    int64_t curTime_;
    OpenTimerProto() :OpenThreadProto(), type_(0), deadline_(0), curTime_(0) {}
    static inline int ProtoType() { return 8886; }
    virtual inline int protoType() const { return OpenTimerProto::ProtoType(); }
};

////////////OpenMsgProto//////////////////////
struct OpenMsgProtoMsg
{
    virtual ~OpenMsgProtoMsg() {}
    static inline int MsgId() { return (int)(int64_t)(void*) & MsgId; }
    virtual inline int msgId() const { return OpenMsgProtoMsg::MsgId(); }
};
struct OpenMsgProto : public OpenThreadProto
{
    virtual ~OpenMsgProto() {}
    std::shared_ptr<OpenMsgProtoMsg> msg_;
    static inline int ProtoType() { return 8887; }
    virtual inline int protoType() const { return OpenMsgProto::ProtoType(); }
};

class OpenServer;
////////////OpenCom//////////////////////
class OpenCom
{
    std::string comName_;
protected:
    int pid_;
    std::string pname_;
    OpenServer* worker_;
public:
    OpenCom() :pid_(-1), worker_(0) {}
    inline const std::string& comName() { return comName_; }
    inline void setComName(const std::string& comName) { comName_ = comName; }
    virtual bool start(OpenServer* worker);
    virtual void onOpenMsgProto(OpenMsgProto& proto) {}
    virtual void onOpenTimerProto(const OpenTimerProto& proto) {}
    bool startTime(int64_t deadline);
    bool startInterval(int64_t interval);
    virtual inline OpenCom* newCom() { assert(false); return new OpenCom; }
    void log(const char* fmt, ...);
};

////////////OpenComSocket//////////////////////
class OpenComSocket : public OpenCom
{
protected:
    virtual void onSocketData(const OpenSocketMsg& msg);
    virtual void onSocketClose(const OpenSocketMsg& msg);
    virtual void onSocketError(const OpenSocketMsg& msg);
    virtual void onSocketWarning(const OpenSocketMsg& msg);
    virtual void onSocketOpen(const OpenSocketMsg& msg);
    virtual void onSocketAccept(const OpenSocketMsg& msg);
    virtual void onSocketUdp(const OpenSocketMsg& msg);
public:
    virtual void onOpenSocketProto(const OpenSocketProto& proto);
    virtual inline OpenCom* newCom() { assert(false); return new OpenComSocket; }
};

////////////OpenServer//////////////////////
class OpenServer : public OpenThreadWorker
{
    class SpinLock
    {
    private:
        SpinLock(const SpinLock&) {};
        void operator=(const SpinLock) {};
    public:
        SpinLock() {};
        void lock() { while (flag_.test_and_set(std::memory_order_acquire)); }
        void unlock() { flag_.clear(std::memory_order_release); }
    private:
        std::atomic_flag flag_;
    };
    class OpenServerPool
    {
        SpinLock spinLock_;
        SpinLock spinSLock_;
        std::unordered_map<std::string, OpenCom*> mapNameCom_;
        std::unordered_map<std::string, OpenServer*> mapNameServer_;
    public:
        virtual ~OpenServerPool();
        template <class T>
        bool registerCom(const std::string& comName)
        {
            spinLock_.lock();
            auto iter = mapNameCom_.find(comName);
            if (iter != mapNameCom_.end())
            {
                printf("OpenServerPool::registerCom [%s] is exist!\n", comName.c_str());
                spinLock_.unlock();
                assert(false);
                return false;
            }
            T* t = new T;
            OpenCom* com = dynamic_cast<OpenCom*>(t);
            if (!com)
            {
                printf("OpenServerPool::registerCom [%s] is no OpenCom!\n", comName.c_str());
                spinLock_.unlock();
                assert(false);
                return false;
            }
            com->setComName(comName);
            mapNameCom_[comName] = com;
            spinLock_.unlock();
            return true;
        }
        bool createComs(const std::vector<std::string>& vectComNames, std::vector<OpenCom*>& vectComs);
        OpenServer* startServer(const std::string& serverName, const std::vector<std::string>& vectComNames);
    };
    static OpenServerPool OpenServerPool_;
protected:
    std::vector<OpenCom*> vectComs_;

    virtual void onStart();
    virtual void onOpenMsgProto(OpenMsgProto& proto);
    virtual void onOpenTimerProto(const OpenTimerProto& proto);
    virtual void onOpenSocketProto(const OpenSocketProto& proto);
    OpenServer() : OpenThreadWorker("Unknow") { assert(false); }

    bool startTime(int64_t deadline);
    bool startInterval(int64_t interval);
    void log(const char* fmt, ...);
public:
    OpenServer(const std::string& name, const std::vector<OpenCom*>& vectComs);
    virtual ~OpenServer();

    template <class T>
    const T* getCom()
    {
        T* t = 0;
        for (size_t i = 0; i < vectComs_.size(); i++)
        {
            t = dynamic_cast<T*>(vectComs_[i]);
            if (t) return t;
        }
        return 0;
    }

    static OpenServer* StartServer(const std::string& serverName, const std::vector<std::string>& vectComNames)
    {
        return OpenServerPool_.startServer(serverName, vectComNames);
    }
    static OpenServer* StartServer(const std::string& serverName, const std::initializer_list<std::string>& list)
    {
        const std::vector<std::string> vectComNames = list;
        return OpenServerPool_.startServer(serverName, vectComNames);
    }
    template <class T>
    static bool RegisterCom(const std::string& comName)
    {
        return OpenServerPool_.registerCom<T>(comName);
    }
};

////////////OpenTimer//////////////////////
class OpenTimer :public OpenServer
{
    int64_t timeInterval_;
    std::multimap<int64_t, int> mapTimerEvent_;
    OpenTimer(const std::string& name) : OpenServer(name, {}), timeInterval_(10) {}
protected:
    virtual void onStart();
private:
    void timeLoop();
    struct TimerPool
    {
        OpenTimer* timer_;
        TimerPool();
        ~TimerPool();
        void run();
        bool startTime(int pid, int64_t deadline);
    };
    static TimerPool TimerPool_;
public:
    virtual void onOpenTimerProto(const OpenTimerProto& proto);
    static bool StartTime(int pid, int64_t deadline);
    static bool StartInterval(int pid, int64_t interval);
    static void Run() { TimerPool_.run(); }
};

////////////OpenApp//////////////////////
class OpenApp
{
    bool isRunning_;
    static void SocketFunc(const OpenSocketMsg* msg);
    static OpenApp OpenApp_;
public:
    OpenApp();
    virtual ~OpenApp();
    void start();
    void wait();
    static inline OpenApp& Instance() { return OpenApp_; }
};

};

#endif  //HEADER_OPEN_SERVER_H
