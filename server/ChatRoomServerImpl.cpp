#include "ChatRoomServerImpl.h"
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include "log.h"
#include "ChatRoomClientImpl.h"

#define OS_EVENTS_MAX 16

bool CEpollServerImpl::create(const std::string & ip, const uint16_t port, const int cnt)
{
    if (_listening)
    {
        log_msg_warn("Already listening ...");
        return false;
    }

    addrinfo * ais = nullptr;
    addrinfo hints = {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    int ret = ::getaddrinfo(ip.c_str(), std::to_string(port).c_str(), &hints, &ais);
    if (0 != ret || nullptr == ais)
    {
        log_msg_error("getaddrinfo failed for %s.", gai_strerror(ret));
        return false;
    }

    sockaddr addr = *(ais->ai_addr);
    socklen_t addr_len = ais->ai_addrlen;

    _server_fd = ::socket(ais->ai_family, ais->ai_socktype, ais->ai_protocol);
    ::freeaddrinfo(ais);
    if (-1 == _server_fd)
    {
        const int code = errno;
        log_msg_error("Create socket failed for %s.", strerror(code));
        return false;
    }

    setSocketOptions();

    struct epoll_event ev = {};
    ev.data.fd = _server_fd;
    ev.events = EPOLLIN;

    if (::bind(_server_fd, &addr, addr_len) < 0)
    {
        const int code = errno;
        log_msg_error("Bind ip: %s, port: %d failed for %s.", ip.c_str(), port, strerror(code));
        goto ERR;
    }

    if (::listen(_server_fd, cnt) < 0)
    {
        const int code = errno;
        log_msg_error("Listen ip: %s, port: %d failed for %s.", ip.c_str(), port, strerror(code));
        goto ERR;
    }

    _epoll_fd = epoll_create(cnt);
    if (-1 == _epoll_fd)
    {
        log_msg_error("Create epoll fd failed.");
        goto ERR;
    }

    if (-1 == epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _server_fd, &ev))
    {
        const int code = errno;
        log_msg_error("Add server fd to epoll fd failed for %s.", strerror(code));
        goto ERR;
    }

    _server_ip = ip;
    _server_port = port;
    _max_client_cnt = cnt;
    _listening = true;
    _thr = std::thread{ &CEpollServerImpl::dealMessageThr, this };

    log_msg_info("Listen %s: %d successful ...", ip.c_str(), port);

    return true;

ERR:
    if (-1 != _server_fd)
    {
        ::close(_server_fd);
        _server_fd = -1;
    }

    if (-1 != _epoll_fd)
    {
        ::close(_epoll_fd);
        _epoll_fd = -1;
    }

    return false;
}

void CEpollServerImpl::destroy()
{
    if (!_listening)
        return;

    if (_thr.joinable())
    {
        _listening = false;
        _thr.join();
    }

    ::close(_server_fd);
    _server_fd = -1;

    ::close(_epoll_fd);
    _epoll_fd = -1;
}

bool CEpollServerImpl::broadcast(const std::string & msg)
{
    std::unique_lock<std::mutex> lck(_mtx);
    for (const auto & client : _clients)
    {
        client.second->sendMessage(msg);
    }

    return true;
}

void CEpollServerImpl::setSocketOptions()
{
    // 设置地址复用，防止不立即释放
    int reuse = 1;
    if (::setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        const int code = errno;
        log_msg_error("setsockopt SO_REUSEADDR failed for %s", strerror(code));
    }

    // 设置端口复用
    if (::setsockopt(_server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0)
    {
        const int code = errno;
        log_msg_error("setsockopt SO_REUSEPORT failed for %s", strerror(code));
    }
}

bool CEpollServerImpl::addClient(int & fd)
{
    // 处理客户端连接
    struct sockaddr_in addr = {};
    socklen_t addr_len = sizeof(addr);
    fd = ::accept(_server_fd, reinterpret_cast<sockaddr *>(&addr), &addr_len);
    if (-1 == fd)
    {
        const int code = errno;
        log_msg_error("Client connect failed for %s.", strerror(code));
        return false;
    }

    char ip[INET_ADDRSTRLEN] = { 0 };
    if (nullptr == inet_ntop(addr.sin_family, &addr.sin_addr, ip, INET_ADDRSTRLEN))
    {
        const int code = errno;
        log_msg_error("Get client addr failed for %s.", strerror(code));
        ::close(fd);
        return false;
    }
    auto port = ntohs(addr.sin_port);

    auto * impl = new(std::nothrow) CClientUnitImpl(fd, ip, port);
    if (nullptr == impl)
    {
        log_msg_error("Create instance for client %d failed!", fd);
        ::close(fd);
        return false;
    }

    int flags = fcntl(fd, F_GETFL, 0);
    if (-1 == fcntl(fd, F_SETFL, flags | O_NONBLOCK))
    {
        const int code = errno;
        log_msg_error("Set O_NONBLOCK for %d failed for %s", fd, strerror(code));
    }

    std::unique_lock<std::mutex> lck(_mtx);
    _clients.emplace(fd, impl);

    return true;
}

bool CEpollServerImpl::delClient(const int fd)
{
    std::unique_lock<std::mutex> lck(_mtx);
    auto found = _clients.find(fd);
    if (found != _clients.end())
    {
        (*found).second->close("");
        _clients.erase(found);
    }
    return true;
}

bool CEpollServerImpl::handleClientMessage(const int fd)
{
    std::unique_lock<std::mutex> lck(_mtx);
    auto found = _clients.find(fd);
    if (found == _clients.end())
    {
        log_msg_warn("Unknown client %d.", fd);
        return false;
    }

    return (*found).second->recvMessage();
}

void CEpollServerImpl::dealMessageThr()
{
    epoll_event evs[OS_EVENTS_MAX] = {};
    while (_listening)
    {
        int ret = epoll_wait(_epoll_fd, evs, _max_client_cnt, 1000);
        if (-1 == ret)
        {
            log_msg_error("epoll_wait failed");
            _listening = false;
            return;
        }

        for (int i = 0; i < ret; ++i)
        {
            if (_server_fd == evs[i].data.fd)
            {
                int client_fd = -1;
                if (!addClient(client_fd))
                    continue;

                epoll_event ev = {};
                ev.data.fd = client_fd;
                ev.events = EPOLLIN | EPOLLET;
                if (-1 == epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &ev))
                {
                    log_msg_error("Add client fd: %d failed, remote it", client_fd);
                    delClient(client_fd);
                }
            }
            else
            {
                if (evs[i].data.fd < 0)
                    continue;
                handleClientMessage(evs[i].data.fd);
            }
        }
    }
}
