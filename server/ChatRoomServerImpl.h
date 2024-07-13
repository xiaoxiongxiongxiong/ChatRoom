#ifndef _CHATROOM_SERVER_IMPL_H_
#define _CHATROOM_SERVER_IMPL_H_

#include <cstdio>
#include <cstdbool>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_map>

class CClientUnitImpl;

class CEpollServerImpl final
{
public:
    static CEpollServerImpl & getInstance()
    {
        static CEpollServerImpl instance;
        return instance;
    }

    // 创建服务器
    bool create(const std::string & ip, uint16_t port, int cnt);

    // 销毁服务器
    void destroy();

    // 广播消息
    bool broadcast(const std::string & msg);

private:
    CEpollServerImpl() = default;
    ~CEpollServerImpl() = default;
    // 删除三大默认成员函数，赋值、复制和地址运算
    CEpollServerImpl(const CEpollServerImpl &) = delete;
    CEpollServerImpl & operator=(const CEpollServerImpl &) = delete;
    CEpollServerImpl * operator&() = delete;

    // 设置连接选项
    void setSocketOptions();

    // 添加客户端
    bool addClient(int & fd);

    // 删除客户端
    bool delClient(int fd);

    // 处理客户端消息
    bool handleClientMessage(int fd);

    // 消息处理线程
    void dealMessageThr();

private:
    // 是否监听中
    bool _listening = false;

    // 服务器监听地址
    std::string _server_ip;
    // 服务器监听端口号
    uint16_t _server_port = 0;
    // 最大连接数量
    int _max_client_cnt = 16;

    // 服务器句柄
    int _server_fd = -1;
    // epoll句柄
    int _epoll_fd = -1;

    std::thread _thr;
    std::mutex _mtx;
    std::condition_variable _cond;

    // 客户端
    std::unordered_map<int, CClientUnitImpl *> _clients;
};

#endif
