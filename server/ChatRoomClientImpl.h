#ifndef _CHATROOM_CLIENT_IMPL_H_
#define _CHATROOM_CLIENT_IMPL_H_

#include <cstdbool>
#include <string>

class CClientUnitImpl
{
public:
    CClientUnitImpl(int fd, const std::string & ip, uint16_t port);
    ~CClientUnitImpl();

    // 发送消息
    bool sendMessage(const std::string & msg);

    // 接收消息
    bool recvMessage();

    // 关闭客户端
    bool close(const std::string & msg);

private:
    // 发送心跳
    bool sendHeartBeat();



private:
    // 连接句柄
    int _fd = -1;

    // 远端连接IP
    std::string _ip;
    // 端口号
    uint16_t _port = 0;

    // 接收到的消息
    std::string _msg;
};

#endif
