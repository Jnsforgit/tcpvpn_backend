/*
 * @Author: Jnsforgit jiangnen123456@qq.com
 * @Date: 2024-09-10 16:57:51
 * @LastEditors: Jnsforgit jiangnen123456@qq.com
 * @LastEditTime: 2024-09-15 01:19:33
 * @FilePath: /backend/backend/c++/anyserver/src/core/anyserver.cpp
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */

#include "common.h"
#include "core.h"

/******************************** public ********************************/

/**
 * @brief Construct a new Anyserver:: Anyserver object
 * 
 */
Anyserver::Anyserver(uint16_t port, std::string host)
{
    m_port = port;
    m_host = host;
}

/**
 * @brief Destroy the Anyserver:: Anyserver object
 * 
 */
Anyserver::~Anyserver()
{
    _Uninit();
}

/**
 * @brief 启动服务
 * 
 * @return true 
 * @return false 
 */
bool Anyserver::Start()
{
    m_running = _Init();
    if (!m_running)
    {
        LOGE("socket error : [%s]", ERRSTR);
        _Uninit();
        return false;
    }

    LOGI("Anyserver [%s:%d] init done!", m_host.c_str(), m_port);
    while (m_running)
    {
        if (!_LoopOnce())
        {
            LOGE("run server loop error");
            _Uninit();
            return false;
        }
    }

    return true;
}

/******************************** private ********************************/

/**
 * @brief 初始化服务器套接字
 * 
 * @param host 
 * @param port 
 * @return SOCKET_T 
 */
SOCKET_T Anyserver::_InitSocket(std::string host="0.0.0.0", uint16_t port=80)
{
    /* socket */
    SOCKET_T fd = socket(AF_INET, SOCK_STREAM, 0);
    if (0 > fd)
    {
        LOGE("socket error : [%s]", ERRSTR);
        return -1;
    }

    /* bind */ 
    struct sockaddr_in saddr;

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    inet_pton(AF_INET, host.c_str(), (void *)&saddr.sin_addr.s_addr);
    saddr.sin_port = htons(port);

    /* 在bind之前设置【端口复用】 */
    int reuse = 1;
    int ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    if (ret < 0)
    {
        LOGE("[%d] setsockopt error: [%s]!", fd, ERRSTR);
        closesocket(fd);
        return -1;
    }

    ret = bind(fd, (struct sockaddr *)&saddr, 16);
    if (ret < 0)
    {
        LOGE("[%d] bind error: [%s]!", fd, ERRSTR);
        closesocket(fd);
        return -1;
    }

    /* listen */ 
    ret = listen(fd, 5);
    if (ret < 0)
    {
        LOGE("[%d] listen error : [%s]!", fd, ERRSTR);
        closesocket(fd);
        return -1;
    }

    /* 设置非阻塞 */
    ret = Utils::SetNoneBlocking(fd);
    if (ret < 0)
    {
        LOGE("set [%d] none blocking error: [%s]!", fd, ERRSTR);
        closesocket(fd);
        return -1;
    }

    return fd;
}


void Anyserver::_InitEventMode(int trig_mode)
{
    /* EPOLLRDHUP宏，底层处理socket连接断开的情况 */ 
    m_listen_event = EPOLLRDHUP;
    /*
     * EPOLLONESHOT 和 ET模式不尽相同：
     *      前者是防止一个客户端发送的数据被多个线程分散读取；
     *      后者是避免多次调用epoll_wait提高epoll效率的一种模式 
     */
    m_connect_event = EPOLLRDHUP | EPOLLONESHOT;
    switch (trig_mode)
    {
        case 1:     /* 0000 0001 */
        {
            m_connect_event |= EPOLLET;
        }
        break;
        case 2:     /* 0000 0010 */
        {
            m_listen_event |= EPOLLET;
        }
        break;
        case 3:     /* 0000 0011 */ 
        {
            m_connect_event |= EPOLLET;
            m_listen_event |= EPOLLET;
        }
        break;
        default:    /* 其他就是都LT */ 
        {
            break;
        }
    }
}

/**
 * @brief 初始化 poller
 * 
 * @return true 
 * @return false 
 */
bool Anyserver::_InitPoller()
{
    try
    {
        m_poller = std::make_unique<Poller>();
    }
    catch (std::exception &e)
    {
        LOGE("Poller constructor error: [%s]!", e.what());
        return false;
    }

    return true;
}

/**
 * @brief 初始化服务
 * 
 * @return true 
 * @return false 
 */
bool Anyserver::_Init()
{
    _InitEventMode(3);
    if (!_InitPoller())
    {
        LOGE("Init server poller error!");
        return false;
    }

    m_listen_fd = _InitSocket(m_host, m_port);
    if (0 > m_listen_fd)
    {
        LOGE("Init server socket error!");
        return false;
    }

    int ret = m_poller->Add(m_listen_fd, m_listen_event | EPOLLIN);
    if (ret == 0)
    {
        LOGE("error epoll_add");
        return false;
    }

    m_thread_pool = std::make_unique<ThreadPool>(8, 200);

    return true;
}

/**
 * @brief 释放服务器资源
 * 
 */
void Anyserver::_Uninit()
{
    m_poller = nullptr;
    m_thread_pool = nullptr;
    if (0 <= m_listen_fd)
    {
        closesocket(m_listen_fd);
    }
}


void Anyserver::_OnListen()
{
    struct sockaddr_in caddr;
    socklen_t len = sizeof(caddr);

    memset(&caddr, 0, sizeof(caddr));
    do
    {
        struct timeval t1;

        memset(&t1, 0, sizeof(t1));
        gettimeofday(&t1, NULL);
        int cfd = accept(m_listen_fd, (struct sockaddr *)&caddr, &len);
        if (cfd < 0)
        {
            LOGD("Accept error : [%s]!", ERRSTR)
            return;
        }
        else if (ConnectionBase::g_atomic_connection_cnts >= MAX_FD)       /* 如果客户端数量超过上限 */
        {
            /* 调用send函数向客户端发送错误信息， 更加严谨的应该是返回一个html网页 */
            // SendError(cfd, "Server Busy");
            return;
        }

        /* 将新来的客户端的cfd和caddr加入到服务器中 */
        char ip[INET_ADDRSTRLEN] = {0};
        uint16_t port = ntohs(caddr.sin_port);

        inet_ntop(AF_INET, &caddr.sin_addr, ip, sizeof(ip));

        if (m_usrs.find(cfd) == m_usrs.end())
        {
            m_usrs[cfd] = std::make_unique<ConnectionBase>(cfd, ip, port);
            m_usrs[cfd]->OnTypeChanged = [&](ConnectionBase *new_conn) {
                m_usrs[cfd].reset(new_conn);
            };
        }

        Utils::SetNoneBlocking(cfd);

        m_poller->Add(cfd, m_connect_event | EPOLLIN);
        // if (m_timeout_ms > 0)
        // {
        //     m_timer->addTimer(cfd, m_timeout_ms, std::bind(&Webserver::DelClient, this, m_usrs[cfd]));
        // }

        LOGD("Accept new connection : [%d %s:%d]!", cfd, ip, port);
    } while (m_listen_event & EPOLLET);
}

void Anyserver::_OnClose(SOCKET_T fd)
{
    if (m_usrs.find(fd) == m_usrs.end())        /* 连接已关闭 */
    {
        LOGI("Connection [%d] closed", fd);
        return;
    }

    if (!m_poller->Del(fd))                    /* 先从 epoll 中删除 */
    {
        LOGI("%s:%d epoll_del failed", m_usrs[fd]->Ip(), m_usrs[fd]->Port());
    }

    // if (m_timeout_ms > 0)
    // {
    //     m_timer->delFd(fd);
    // }
    
    LOGD("Connection [%d %s:%u] closing", fd, m_usrs[fd]->Ip().c_str(), m_usrs[fd]->Port());

    m_usrs[fd] = nullptr;   /* 释放连接 */
    m_usrs.erase(fd);
}

void Anyserver::_OnRead(SOCKET_T fd)
{
    struct timeval t1;

    gettimeofday(&t1, NULL);

    auto task = m_usrs[fd]->OnRead();
    if (nullptr == task)                    /* 处理读事件失败 */
    {
        LOGD("[%d] read event error", fd);
        _OnClose(fd);
    }
    else
    {
        m_thread_pool->Append(task);        /* 把业务处理逻辑代码交给线程池去处理 */
    }

    struct timeval t2;
    gettimeofday(&t2, NULL);
    long tt = (t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_usec - t1.tv_usec);
    LOGD("[%d] read event done, using [%ld]us", fd, tt);
}

void Anyserver::_OnWrite(SOCKET_T fd)
{
    // // 调用Httpconnection对象的write函数
    // int errnoNum = 0;
    // ssize_t ret = client->WriteBuffer(&errnoNum);
    // // 更新定时器
    // if (m_timeout_ms > 0)
    // {
    //     m_timer->updateTimer(client->GetFd(), m_timeout_ms);
    // }
    // // 根据发送数据的长度ret来判断服务器发送数据是否正常
    // /*
    // 1. 如果向客户端成功发送了数据：
    //     - 如果客户端请求保持连接keep-alive：
    //         - 保持连接。
    //         - 没有操作后m_timeoutMs后断开
    //     - 不保持连接：
    //         - 调用closeConn关闭连接
    // 2. 发送数据失败？
    //     - Httpconnection中没有装填上应有的数据：
    //         - 服务器自己的问题：
    //             - 可能是请求数据失败，导致接收buffer拿不到数据，因此发送数据也就失败了
    //             - 还可能是装填数据失败
    //             重新打开接收窗口，让读缓冲区接收I/O数据：
    //                 - 不一定能再次拿到数据。毕竟客户端发送了一遍数据后，就不再发送了
    //     - 服务器调用writev函数，发送数据过程中，产生了错误
    //         - 错误要根据错误号判断属于哪种错误。
    //             - EAGAIN：让服务器再一次发送数据
    //         - 其他错误就不用再重发了
    // */
    // bool statusRecord = false;
    // if (ret > 0)
    // {
    //     if (client->IsKeepAlive())
    //     {
    //         statusRecord = true;
    //         m_epoller->Mod(client->GetFd(), m_connect_event | EPOLLIN);
    //         LOG_DEBUG("数据发送完成");
    //     }
    //     else
    //     {
    //         LOG_DEBUG("[%d]数据发送长度大于0，但是客户端选择不保持连接",client->GetFd());
    //         OnClose(client);
    //         statusRecord = false;
    //     }
    // }
    // else if (client->WriteBytes() == 0)
    // {
    //     // 保存的客户端属性中，此时没有数据
    //     if (client->IsKeepAlive())
    //     {
    //         // 对于长连接，如果没有数据，那就重新更新一个客户端中数据
    //         LOG_DEBUG("发送的数据长度为0，重新进入到onProcess阶段");
    //         OnProcess(client);
    //         statusRecord = true;
    //     }
    //     statusRecord = false;
    // }
    // else if (ret < 0)
    // {
    //     if (errnoNum == EAGAIN)
    //     {
    //         // LOG_DEBUG("发送的数据长度小于0，但是错误号为EAGAIN");
    //         m_epoller->Mod(client->GetFd(), m_connect_event | EPOLLOUT);
    //         statusRecord = true;
    //     }
    //     statusRecord = false;
    // }
    // return statusRecord;
}

/**
 * @brief 运行循环一次
 * 
 * @return true 
 * @return false 
 */
bool Anyserver::_LoopOnce()
{
    int wait_time = -1;

    /* 调用epoll监听 */
    return m_poller->Wait([&](SOCKET_T currfd, uint32_t events){
        if (currfd == m_listen_fd)                              /* 判断事件类型：新连接到来？读？写？ */ 
        {
            _OnListen();                                        /* 处理新连接 */
        }
        else if (events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))   /* 对端关闭了连接 */
        {
            _OnClose(currfd);
        }
        else if (events & EPOLLIN)                              /* 读事件: 现在主线程中读取数据到缓冲区中；再由线程池完成业务逻辑  */ 
        {
            _OnRead(currfd);
        }
        else if (events & EPOLLOUT)                             /* 写事件 */
        {
            _OnWrite(currfd);
        }
        else
        {
            LOGW("unexpected event");
        }
    }, wait_time);
}
