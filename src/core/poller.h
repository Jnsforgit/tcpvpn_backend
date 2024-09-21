/*
 * @Author: Jnsforgit jiangnen123456@qq.com
 * @Date: 2024-09-14 11:54:27
 * @LastEditors: Jnsforgit jiangnen123456@qq.com
 * @LastEditTime: 2024-09-14 11:56:34
 * @FilePath: /backend/backend/c++/anyserver/src/core/poller.h
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef __POLLER_H__
#define __POLLER_H__

class Poller
{
public:
    explicit Poller(int maxevents = 1024);
    ~Poller();

    bool Wait(std::function<void (SOCKET_T, uint32_t)> const &handle, int timeout = -1);
    bool Add(int fd, uint32_t ev);
    bool Mod(int fd, uint32_t ev);
    bool Del(int fd);

private:

    int m_poll_fd = -1;                         /* Epoller对象中的epollfd文件描述符 */
    int m_maxevents_once = -1;
    struct epoll_event* m_ep_eves = nullptr;    /* 一次性接收的事件集合 */ 
};

#endif
