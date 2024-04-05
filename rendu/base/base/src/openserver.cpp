#include <string.h>
#include "openserver.h"
#include "opentime.h"

namespace open
{

////////////OpenCom//////////////////////
bool OpenCom::start(OpenServer* worker)
{
    assert(worker);
    if (!worker)
    {
        return false;
    }
    worker_ = worker;
    pid_    = worker_->pid();
    pname_  = worker_->name();
    return true;
}
bool OpenCom::startTime(int64_t deadline)
{
    return OpenTimer::StartTime(pid_, deadline);
}
bool OpenCom::startInterval(int64_t interval)
{
    return OpenTimer::StartInterval(pid_, interval);
}

////////////OpenComSocket//////////////////////
void OpenComSocket::onOpenSocketProto(const OpenSocketProto& proto)
{
    if (!proto.data_)
    {
        assert(false);
        return;
    }
    const OpenSocketMsg& msg = *proto.data_;
    switch (msg.type_)
    {
    case OpenSocket::ESocketData:
        onSocketData(msg); break;
    case OpenSocket::ESocketClose:
        onSocketClose(msg); break;
    case OpenSocket::ESocketError:
        onSocketError(msg); break;
    case OpenSocket::ESocketWarning:
        onSocketWarning(msg); break;
    case OpenSocket::ESocketOpen:
        onSocketOpen(msg); break;
    case OpenSocket::ESocketAccept:
        onSocketAccept(msg); break;
    case OpenSocket::ESocketUdp:
        onSocketUdp(msg); break;
    default:
        assert(false);
        break;
    }
}

void OpenComSocket::onSocketData(const OpenSocketMsg& msg)
{
    assert(false);
}

void OpenComSocket::onSocketClose(const OpenSocketMsg& msg)
{
    assert(false);
}

void OpenComSocket::onSocketError(const OpenSocketMsg& msg)
{
    printf("OpenComSocket::onSocketError [%s]:%s\n", OpenThread::ThreadName((int)msg.uid_).c_str(), msg.info());
}

void OpenComSocket::onSocketWarning(const OpenSocketMsg& msg)
{
    printf("OpenComSocket::onSocketWarning [%s]:%s\n", OpenThread::ThreadName((int)msg.uid_).c_str(), msg.info());
}

void OpenComSocket::onSocketOpen(const OpenSocketMsg& msg)
{
    assert(false);
}

void OpenComSocket::onSocketAccept(const OpenSocketMsg& msg)
{
    int accept_fd = msg.ud_;
    const std::string addr = msg.data();
    printf("OpenComSocket::onSocketAccept [%s]:%s\n", OpenThread::ThreadName((int)msg.uid_).c_str(), addr.c_str());
}

void OpenComSocket::onSocketUdp(const OpenSocketMsg& msg)
{
    assert(false);
}

////////////OpenServerPool//////////////////////
OpenServer::OpenServerPool OpenServer::OpenServerPool_;
OpenServer::OpenServerPool::~OpenServerPool()
{
    spinLock_.lock();
    auto iter = mapNameCom_.begin();
    for (; iter != mapNameCom_.end(); iter++)
    {
        delete iter->second;
    }
    mapNameCom_.clear();
    spinLock_.unlock();

    spinSLock_.lock();
    auto iter1 = mapNameServer_.begin();
    for (; iter1 != mapNameServer_.end(); iter1++)
    {
        delete iter1->second;
    }
    mapNameServer_.clear();
    spinSLock_.unlock();
}

bool OpenServer::OpenServerPool::createComs(const std::vector<std::string>& vectComNames, std::vector<OpenCom*>& vectComs)
{
    spinLock_.lock();
    std::vector<OpenCom*> vectOldComs;
    for (size_t i = 0; i < vectComNames.size(); ++i)
    {
        const std::string& comName = vectComNames[i];
        auto iter = mapNameCom_.find(comName);
        if (iter == mapNameCom_.end())
        {
            printf("OpenServerPool::createComs [%s] is not exist!\n", comName.c_str());
            spinLock_.unlock();
            assert(false);
            return false;
        }
        vectOldComs.push_back(iter->second);
    }
    for (size_t i = 0; i < vectOldComs.size(); i++)
    {
        vectComs.push_back(vectOldComs[i]->newCom());
    }
    spinLock_.unlock();
    return true;
}

OpenServer* OpenServer::OpenServerPool::startServer(const std::string& serverName, const std::vector<std::string>& vectComNames)
{
    OpenServer* server = NULL;
    spinSLock_.lock();
    if (mapNameServer_.find(serverName) != mapNameServer_.end())
    {
        spinSLock_.unlock();
        assert(false);
        return server;
    }
    std::vector<OpenCom*> vectComs;
    if (!OpenServerPool_.createComs(vectComNames, vectComs))
    {
        spinSLock_.unlock();
        assert(false);
        return server;
    }
    server = new OpenServer(serverName, vectComs);
    mapNameServer_[serverName] = server;
    spinSLock_.unlock();
    server->start();
    return server;
}

////////////OpenServer//////////////////////
OpenServer::OpenServer(const std::string& name, const std::vector<OpenCom*>& vectComs)
    :OpenThreadWorker(name),
    vectComs_(vectComs)
{
    registers(OpenTimerProto::ProtoType(), (OpenThreadHandle)&OpenServer::onOpenTimerProto);
    registers(OpenMsgProto::ProtoType(), (OpenThreadHandle)&OpenServer::onOpenMsgProto);
    registers(OpenSocketProto::ProtoType(), (OpenThreadHandle)&OpenServer::onOpenSocketProto);
}

OpenServer::~OpenServer()
{
    for (size_t i = 0; i < vectComs_.size(); ++i)
        delete vectComs_[i];

    vectComs_.clear();
}

void OpenServer::onStart()
{
    for (size_t i = 0; i < vectComs_.size(); i++)
    {
        vectComs_[i]->start(this);
    }
}

void OpenServer::onOpenMsgProto(OpenMsgProto& proto)
{
    for (size_t i = 0; i < vectComs_.size(); i++)
    {
        vectComs_[i]->onOpenMsgProto(proto);
    }
}

void OpenServer::onOpenTimerProto(const OpenTimerProto& proto)
{
    for (size_t i = 0; i < vectComs_.size(); i++)
    {
        vectComs_[i]->onOpenTimerProto(proto);
    }
}

void OpenServer::onOpenSocketProto(const OpenSocketProto& proto)
{
    OpenComSocket* comSocket = 0;
    for (size_t i = 0; i < vectComs_.size(); i++)
    {
        comSocket = dynamic_cast<OpenComSocket*>(vectComs_[i]);
        if (comSocket)
        {
            comSocket->onOpenSocketProto(proto);
        }
    }
}
bool OpenServer::startTime(int64_t deadline)
{
    return OpenTimer::StartTime(pid_, deadline);
}
bool OpenServer::startInterval(int64_t interval)
{
    return OpenTimer::StartInterval(pid_, interval);
}

////////////OpenTimer//////////////////////
void OpenTimer::onStart()
{
    auto proto = std::shared_ptr<OpenTimerProto>(new OpenTimerProto);
    proto->type_ = -1;
    send(pid_, proto);
}
void OpenTimer::onOpenTimerProto(const OpenTimerProto& proto)
{
    if (proto.type_ >= 0)
    {
        //printf("OpenTimer::onOpenTimerProto[%d]%s\n", proto.srcPid_, OpenTime::MilliToString(proto.deadline_).data());
        mapTimerEvent_.insert({ proto.deadline_, proto.srcPid_ });
    }
    else
    {
        assert(proto.srcPid_ == pid_);
    }
    timeLoop();
}
void OpenTimer::timeLoop()
{
    int64_t curTime = OpenTime::MilliUnixtime();
    while (canLoop())
    {
        if (!mapTimerEvent_.empty())
        {
            while (!mapTimerEvent_.empty())
            {
                auto iter = mapTimerEvent_.begin();
                if (curTime < iter->first) break;

                auto proto = std::shared_ptr<OpenTimerProto>(new OpenTimerProto);
                proto->curTime_ = curTime;
                proto->deadline_ = iter->first;
                send(iter->second, proto);
                mapTimerEvent_.erase(iter);
            }
        }
        OpenThread::Sleep(timeInterval_);
        curTime += timeInterval_;
    }
}
bool OpenTimer::StartTime(int pid, int64_t deadline)
{
    assert(pid >= 0 && deadline >= 0);
    return TimerPool_.startTime(pid, deadline);
}
bool OpenTimer::StartInterval(int pid, int64_t interval)
{
    assert(pid >= 0 && interval >= 0);
    return TimerPool_.startTime(pid, OpenThread::MilliUnixtime() + interval);
}
////////////TimerPool//////////////////////
OpenTimer::TimerPool::TimerPool()
    :timer_(0)
{
}
OpenTimer::TimerPool::~TimerPool()
{
    if (timer_)
    {
        delete timer_;
        timer_ = 0;
    }
}
void OpenTimer::TimerPool::run()
{
    if (timer_) return;
    timer_ = new OpenTimer("OpenTimer");
    timer_->start();
}
bool OpenTimer::TimerPool::startTime(int pid, int64_t deadline)
{
    if (!timer_)
    {
        assert(false);
        return false;
    }
    auto proto = std::shared_ptr<OpenTimerProto>(new OpenTimerProto);
    proto->deadline_ = deadline;
    proto->srcPid_ = pid;
    bool ret = OpenThread::Send(timer_->pid(), proto);
    assert(ret);
    return ret;
}
OpenTimer::TimerPool OpenTimer::TimerPool_;


////////////OpenApp//////////////////////
void OpenApp::SocketFunc(const OpenSocketMsg* msg)
{
    if (!msg) return;
    if (msg->uid_ >= 0)
    {
        auto proto = std::shared_ptr<OpenSocketProto>(new OpenSocketProto);
        proto->srcPid_ = -1;
        proto->srcName_ = "OpenSocket";
        proto->data_ = std::shared_ptr<OpenSocketMsg>((OpenSocketMsg*)msg);
        if (!OpenThread::Send((int)msg->uid_, proto))
            printf("SocketFunc dispatch faild pid = %d\n", (int)msg->uid_);
    }
    else delete msg;
}

OpenApp OpenApp::OpenApp_;
OpenApp::OpenApp()
    :isRunning_(false)
{
}

OpenApp::~OpenApp()
{
}

void OpenApp::start()
{
    if (isRunning_) return;
    isRunning_ = true;
    OpenSocket::Start(OpenApp::SocketFunc);
}

void OpenApp::wait()
{
    OpenThread::ThreadJoinAll();
}

};