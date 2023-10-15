/*
* Created by boil on 2023/10/17.
*/

#include "socket.h"
#include "socket_async_event_args.h"

#ifdef _WIN32
static int _wsaErrorToErrno(int err) {
    switch (err) {
        case WSAEWOULDBLOCK:
            return EWOULDBLOCK;
        case WSAEINPROGRESS:
            return EINPROGRESS;
        case WSAEALREADY:
            return EALREADY;
        case WSAENOTSOCK:
            return ENOTSOCK;
        case WSAEDESTADDRREQ:
            return EDESTADDRREQ;
        case WSAEMSGSIZE:
            return EMSGSIZE;
        case WSAEPROTOTYPE:
            return EPROTOTYPE;
        case WSAENOPROTOOPT:
            return ENOPROTOOPT;
        case WSAEPROTONOSUPPORT:
            return EPROTONOSUPPORT;
        case WSAEOPNOTSUPP:
            return EOPNOTSUPP;
        case WSAEAFNOSUPPORT:
            return EAFNOSUPPORT;
        case WSAEADDRINUSE:
            return EADDRINUSE;
        case WSAEADDRNOTAVAIL:
            return EADDRNOTAVAIL;
        case WSAENETDOWN:
            return ENETDOWN;
        case WSAENETUNREACH:
            return ENETUNREACH;
        case WSAENETRESET:
            return ENETRESET;
        case WSAECONNABORTED:
            return ECONNABORTED;
        case WSAECONNRESET:
            return ECONNRESET;
        case WSAENOBUFS:
            return ENOBUFS;
        case WSAEISCONN:
            return EISCONN;
        case WSAENOTCONN:
            return ENOTCONN;
        case WSAETIMEDOUT:
            return ETIMEDOUT;
        case WSAECONNREFUSED:
            return ECONNREFUSED;
        case WSAELOOP:
            return ELOOP;
        case WSAENAMETOOLONG:
            return ENAMETOOLONG;
        case WSAEHOSTUNREACH:
            return EHOSTUNREACH;
        case WSAENOTEMPTY:
            return ENOTEMPTY;
        default:
            /* We just return a generic I/O error if we could not find a relevant error. */
            return EIO;
    }
}

static void _updateErrno(int success) {
    errno = success ? 0 : _wsaErrorToErrno(WSAGetLastError());
}

static int _initWinsock() {
    static int s_initialized = 0;
    if (!s_initialized) {
        static WSADATA wsadata;
        int err = WSAStartup(MAKEWORD(2,2), &wsadata);
        if (err != 0) {
            errno = _wsaErrorToErrno(err);
            return 0;
        }
        s_initialized = 1;
    }
    return 1;
}

int win32_getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res) {
    /* Note: This function is likely to be called before other functions, so run init here. */
    if (!_initWinsock()) {
        return EAI_FAIL;
    }

    switch (getaddrinfo(node, service, hints, res)) {
        case 0:                     return 0;
        case WSATRY_AGAIN:          return EAI_AGAIN;
        case WSAEINVAL:             return EAI_BADFLAGS;
        case WSAEAFNOSUPPORT:       return EAI_FAMILY;
        case WSA_NOT_ENOUGH_MEMORY: return EAI_MEMORY;
        case WSAHOST_NOT_FOUND:     return EAI_NONAME;
        case WSATYPE_NOT_FOUND:     return EAI_SERVICE;
        case WSAESOCKTNOSUPPORT:    return EAI_SOCKTYPE;
        default:                    return EAI_FAIL;     /* Including WSANO_RECOVERY */
    }
}

const char *win32_gai_strerror(int errcode) {
    switch (errcode) {
        case 0:            errcode = 0;                     break;
        case EAI_AGAIN:    errcode = WSATRY_AGAIN;          break;
        case EAI_BADFLAGS: errcode = WSAEINVAL;             break;
        case EAI_FAMILY:   errcode = WSAEAFNOSUPPORT;       break;
        case EAI_MEMORY:   errcode = WSA_NOT_ENOUGH_MEMORY; break;
        case EAI_NONAME:   errcode = WSAHOST_NOT_FOUND;     break;
        case EAI_SERVICE:  errcode = WSATYPE_NOT_FOUND;     break;
        case EAI_SOCKTYPE: errcode = WSAESOCKTNOSUPPORT;    break;
        default:           errcode = WSANO_RECOVERY;        break; /* Including EAI_FAIL */
    }
    return gai_strerror(errcode);
}

void win32_freeaddrinfo(struct addrinfo *res) {
    freeaddrinfo(res);
}

SOCKET win32_socket(int domain, int type, int protocol) {
    SOCKET s;

    /* Note: This function is likely to be called before other functions, so run init here. */
    if (!_initWinsock()) {
        return INVALID_SOCKET;
    }

    _updateErrno((s = socket(domain, type, protocol)) != INVALID_SOCKET);
    return s;
}

int win32_ioctl(SOCKET fd, unsigned long request, unsigned long *argp) {
    int ret = ioctlsocket(fd, (long)request, argp);
    _updateErrno(ret != SOCKET_ERROR);
    return ret != SOCKET_ERROR ? ret : -1;
}

int win32_bind(SOCKET sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int ret = bind(sockfd, addr, addrlen);
    _updateErrno(ret != SOCKET_ERROR);
    return ret != SOCKET_ERROR ? ret : -1;
}

int win32_connect(SOCKET sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int ret = connect(sockfd, addr, addrlen);
    _updateErrno(ret != SOCKET_ERROR);

    /* For Winsock connect(), the WSAEWOULDBLOCK error means the same thing as
     * EINPROGRESS for POSIX connect(), so we do that translation to keep POSIX
     * logic consistent.
     * Additionally, WSAALREADY is can be reported as WSAEINVAL to  and this is
     * translated to EIO.  Convert appropriately
     */
    int err = errno;
    if (err == EWOULDBLOCK) {
        errno = EINPROGRESS;
    }
    else if (err == EIO) {
        errno = EALREADY;
    }

    return ret != SOCKET_ERROR ? ret : -1;
}

int win32_getsockopt(SOCKET sockfd, int level, int optname, void *optval, socklen_t *optlen) {
    int ret = 0;
    if ((level == SOL_SOCKET) && ((optname == SO_RCVTIMEO) || (optname == SO_SNDTIMEO))) {
        if (*optlen >= sizeof (struct timeval)) {
            struct timeval *tv = optval;
            DWORD timeout = 0;
            socklen_t dwlen = 0;
            ret = getsockopt(sockfd, level, optname, (char *)&timeout, &dwlen);
            tv->tv_sec = timeout / 1000;
            tv->tv_usec = (timeout * 1000) % 1000000;
        } else {
            ret = WSAEFAULT;
        }
        *optlen = sizeof (struct timeval);
    } else {
        ret = getsockopt(sockfd, level, optname, (char*)optval, optlen);
    }
    if (ret != SOCKET_ERROR && level == SOL_SOCKET && optname == SO_ERROR) {
        /* translate SO_ERROR codes, if non-zero */
        int err = *(int*)optval;
        if (err != 0) {
            err = _wsaErrorToErrno(err);
            *(int*)optval = err;
        }
    }
    _updateErrno(ret != SOCKET_ERROR);
    return ret != SOCKET_ERROR ? ret : -1;
}

int win32_setsockopt(SOCKET sockfd, int level, int optname, const void *optval, socklen_t optlen) {
    int ret = 0;
    if ((level == SOL_SOCKET) && ((optname == SO_RCVTIMEO) || (optname == SO_SNDTIMEO))) {
        const struct timeval *tv = optval;
        DWORD timeout = tv->tv_sec * 1000 + tv->tv_usec / 1000;
        ret = setsockopt(sockfd, level, optname, (const char*)&timeout, sizeof(DWORD));
    } else {
        ret = setsockopt(sockfd, level, optname, (const char*)optval, optlen);
    }
    _updateErrno(ret != SOCKET_ERROR);
    return ret != SOCKET_ERROR ? ret : -1;
}

int win32_close(SOCKET fd) {
    int ret = closesocket(fd);
    _updateErrno(ret != SOCKET_ERROR);
    return ret != SOCKET_ERROR ? ret : -1;
}

ssize_t win32_recv(SOCKET sockfd, void *buf, size_t len, int flags) {
    int ret = recv(sockfd, (char*)buf, (int)len, flags);
    _updateErrno(ret != SOCKET_ERROR);
    return ret != SOCKET_ERROR ? ret : -1;
}

ssize_t win32_send(SOCKET sockfd, const void *buf, size_t len, int flags) {
    int ret = send(sockfd, (const char*)buf, (int)len, flags);
    _updateErrno(ret != SOCKET_ERROR);
    return ret != SOCKET_ERROR ? ret : -1;
}

int win32_poll(struct pollfd *fds, nfds_t nfds, int timeout) {
    int ret = WSAPoll(fds, nfds, timeout);
    _updateErrno(ret != SOCKET_ERROR);
    return ret != SOCKET_ERROR ? ret : -1;
}

int win32_redisKeepAlive(SOCKET sockfd, int interval_ms) {
    struct tcp_keepalive cfg;
    DWORD bytes_in;
    int res;

    cfg.onoff = 1;
    cfg.keepaliveinterval = interval_ms;
    cfg.keepalivetime = interval_ms;

    res = WSAIoctl(sockfd, SIO_KEEPALIVE_VALS, &cfg,
                   sizeof(struct tcp_keepalive), NULL, 0,
                   &bytes_in, NULL, NULL);

    return res == 0 ? 0 : _wsaErrorToErrno(res);
}

#endif /* _WIN32 */

RD_NAMESPACE_BEGIN

    Socket::Socket(AddressFamily addressFamily, SocketType socketType, ProtocolType protocolType)
        : m_addressFamily(std::underlying_type<AddressFamily>::type(addressFamily)),
          m_socketType(std::underlying_type<SocketType>::type(socketType)),
          m_protocolType(std::underlying_type<ProtocolType>::type(protocolType)) {
      // 创建socket
      m_socket_fd = socket(m_addressFamily, m_socketType, m_protocolType);
    }

    Socket::~Socket() {

    }

    IPEndPoint *Socket::GetRemoteEndPoint() {
      return nullptr;
    }

    void Socket::SetNoDelay(bool b) {

    }

    void Socket::Close() {
      close(m_socket_fd);
    }

    bool Socket::AcceptAsync(SocketAsyncEventArgs *pArgs) {
      if (pArgs == nullptr){
        pArgs = new SocketAsyncEventArgs();
      }
//      accept(m_socket_fd, pArgs->m_AcceptSocket->m_socket_fd, pArgs->m_AcceptSocket->m_addressFamily, pArgs->m_AcceptSocket->m_protocolType);
      if (pArgs->m_AcceptSocket != nullptr){
        return false;
      }
      return true;
    }

    bool Socket::ConnectAsync(SocketAsyncEventArgs *pArgs) {
      return false;
    }

    bool Socket::SendAsync(SocketAsyncEventArgs *pArgs) {
      return false;
    }

    bool Socket::ReceiveAsync(SocketAsyncEventArgs *pArgs) {
      return false;
    }

    void Socket::Bind(IPEndPoint &point) {
      // 设置服务器地址
      sockaddr_in serverAddress{};
      serverAddress.sin_family = AF_INET;
      serverAddress.sin_addr.s_addr = INADDR_ANY;
      serverAddress.sin_port = htons(point.GetPort());
      // 绑定socket到指定地址和端口
      if (bind(m_socket_fd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
//        //TODO:BOIl
      }
//      int s = -1, rv;
//      const char *_port = std::to_string(point.port()).c_str();  /* strlen("65535") */
//      const char *_bindaddr = point.address().to_string().c_str();
//      addrinfo hints{}, *servinfo, *p;
//      memset(&hints, 0, sizeof(hints));
//      hints.ai_family = m_addressFamily;
//      hints.ai_socktype = m_socketType;
//      hints.ai_flags = AI_PASSIVE;    /* No effect if bindaddr != NULL */
//      if (_bindaddr && !strcmp("*", _bindaddr))
//        _bindaddr = NULL;
//      if (m_addressFamily == AF_INET6 && _bindaddr && !strcmp("::*", _bindaddr))
//        _bindaddr = NULL;
//
//      if ((rv = getaddrinfo(_bindaddr, _port, &hints, &servinfo)) != 0) {
//        //TODO:BOIl
////        anetSetError(err, "%s", gai_strerror(rv));
////        return ANET_ERR;
//      }
//      // 绑定socket到指定地址和端口
//      for (p = servinfo; p != NULL; p = p->ai_next) {
//        if (m_socket_fd == -1)
//          continue;
//        if (bind(m_socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
//          Close();
//        }
//      }
    }

    void Socket::Listen(int backlog) {
      listen(m_socket_fd, backlog);
    }

    void Socket::Accept() {
      m_socket_fd = accept(m_socket_fd, NULL, NULL);
    }

    void Socket::Connect() {
//      m_socket_fd = connect(m_socket_fd, m_address.get(), m_address.size());
    }

RD_NAMESPACE_END