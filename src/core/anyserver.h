/*
 * @Author: Jnsforgit jiangnen123456@qq.com
 * @Date: 2024-09-10 16:58:59
 * @LastEditors: Jnsforgit jiangnen123456@qq.com
 * @LastEditTime: 2024-09-14 11:41:50
 * @FilePath: /backend/backend/c++/anyserver/src/core/anyserver.h
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef __ANYSERVER_H__
#define __ANYSERVER_H__

typedef std::unique_ptr<ConnectionBase> ConnPtr_t;

class Anyserver
{

public:
    Anyserver(uint16_t port=80, std::string host="127.0.0.1");
    ~Anyserver();

    bool Start();

    static const int MAX_FD = 65535;

private:
    void _Uninit();                             /* 卸载服务器资源 */
    bool _Init();                               /* 初始化服务器资源 */
    bool _InitPoller();                         /* 初始化 poller */
    void _InitEventMode(int trig_mode);         /* 设置服务器的epoll工作模式 */
    SOCKET_T _InitSocket(std::string host, uint16_t port);
    
    bool _LoopOnce();                           /* 单次循环 */
    void _OnListen();                           /* listen 事件 */
    void _OnClose(SOCKET_T fd);                 /* close 事件 */
    void _OnRead(SOCKET_T fd);                  /* read 事件 */
    void _OnWrite(SOCKET_T fd);                 /* write 事件 */

    bool m_running              = false;        /* 服务运行标志 */
    uint16_t m_port             = 0;            /* 服务监听端口 */
    std::string m_host;                         /* 服务监听地址 */
    SOCKET_T m_listen_fd        = -1;           /* 服务监听套接字 */

    int m_timeout_ms            = 0;            /* 连接超时事件 */
    uint32_t m_listen_event     = 0;            /* 监听事件 */
    uint32_t m_connect_event    = 0;            /* 连接事件 */

    std::unique_ptr<Poller> m_poller            = nullptr;
    std::unique_ptr<ThreadPool> m_thread_pool   = nullptr;

    /*
     * 用存储位置为栈区的哈哈希表保存客户端的连接信息，这合理吗？
     * 理论上最大支持65535个连接，栈区的内存大小能够满足吗？
     * 破案了。哈希表对象是存放在栈区的，但是对象里的value都是保存在堆区的
     */
    std::unordered_map<int, ConnPtr_t> m_usrs;
};

#endif