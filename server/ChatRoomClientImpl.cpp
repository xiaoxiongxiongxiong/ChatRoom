#include "ChatRoomClientImpl.h"
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include "log.h"

CClientUnitImpl::CClientUnitImpl(const int fd, const std::string & ip, const uint16_t port) :
    _fd(fd), _ip(ip), _port(port)
{
    log_msg_info("Client connected form %s : %d", ip.c_str(), port);
}

CClientUnitImpl::~CClientUnitImpl()
{
    if (_fd > 0)
    {
        ::close(_fd);
        log_msg_warn("Client %s : %d disconnected ...", _ip.c_str(), _port);
    }

    _ip.clear();
    _msg.clear();
}

bool CClientUnitImpl::sendMessage(const std::string & msg)
{
    if (_fd < 0)
    {
        log_msg_warn("Client %s : %d disconnected ...", _ip.c_str(), _port);
        return false;
    }

    auto len = msg.length();
    while (len > 0)
    {
        const auto bytes = ::send(_fd, msg.c_str(), len, 0);
        if (-1 == bytes)
        {
            const int code = errno;
            log_msg_error("Client %s : %d send msg failed for %s.", _ip.c_str(), _port, strerror(code));
            return false;
        }
        len -= static_cast<size_t>(bytes);
    }

    return true;
}

bool CClientUnitImpl::recvMessage()
{
    if (_fd < 0)
    {
        log_msg_warn("Client %s : %d disconnected ...", _ip.c_str(), _port);
        return false;
    }

    while (true)
    {
        char buff[1024] = { 0 };
        const auto len = ::recv(_fd, buff, 1023, 0);
        if (0 == len)
        {
            log_msg_warn("Client %s : %d disconnected ...", _ip.c_str(), _port);
            ::close(_fd);
            _fd = -1;
            return false;
        }
        else if (len > 0)
        {
            log_msg_info("%s:%d says: %s", _ip.c_str(), _port, buff);
            _msg.append(buff, len);
        }
        else
            break;
    }

    return true;
}

bool CClientUnitImpl::close(const std::string & msg)
{
    if (_fd > 0)
    {
        sendMessage(msg);
        ::close(_fd);
        _fd = -1;
    }

    return true;
}

bool CClientUnitImpl::sendHeartBeat()
{
    return true;
}
