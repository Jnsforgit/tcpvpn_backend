/*
 * @Author: Jnsforgit jiangnen123456@qq.com
 * @Date: 2024-09-14 11:40:04
 * @LastEditors: Jnsforgit jiangnen123456@qq.com
 * @LastEditTime: 2024-09-14 11:40:05
 * @FilePath: /backend/backend/c++/anyserver/src/core/poller.cpp
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
#include "common.h"
#include "core.h"

/******************************** public ********************************/
Poller::Poller(int maxevents)
{
    m_maxevents_once = maxevents;
    m_poll_fd = epoll_create(655360);   /* size参数现在并不起作用，只是给内核一个提示，告诉内核应该如何为内部数据结构划分初始大小 */
    m_ep_eves = new struct epoll_event[m_maxevents_once];
}

Poller::~Poller()
{
    close(m_poll_fd);
}

bool Poller::Wait(std::function<void (SOCKET_T, uint32_t)> const &handle, int timeout)
{
    int nums = epoll_wait(m_poll_fd, &m_ep_eves[0], m_maxevents_once, timeout);
    if (0 > nums)
    {
        LOGE("error epoll_wait:[%s]!", ERRSTR);
        return false;
    }

    for (int i = 0; i < nums; i++)
    {
        handle(m_ep_eves[i].data.fd, m_ep_eves[i].events);
    }

    return true;
}

bool Poller::Add(int fd, uint32_t ev)
{
    if (0 > fd|| 0 > m_poll_fd)
    {
        LOGE("Epoll add error:[%d : %d]!", m_poll_fd, fd);
        return false;
    }

    struct epoll_event event;

    memset(&event, 0, sizeof(event));
    event.events = ev;
    event.data.fd = fd;
    epoll_ctl(m_poll_fd, EPOLL_CTL_ADD, fd, &event);

    return true;
}

bool Poller::Mod(int fd, uint32_t ev)
{
    if (0 > fd|| 0 > m_poll_fd)
    {
        LOGE("Epoll mod error:[%d : %d]!", m_poll_fd, fd);
        return false;
    }

    struct epoll_event event;

    memset(&event, 0, sizeof(event));
    event.events = ev;
    event.data.fd = fd;
    epoll_ctl(m_poll_fd, EPOLL_CTL_MOD, fd, &event);

    return true;
}

bool Poller::Del(int fd)
{
    if (0 > fd|| 0 > m_poll_fd)
    {
        LOGE("Epoll mod del:[%d : %d]!", m_poll_fd, fd);
        return false;
    }

    epoll_ctl(m_poll_fd, EPOLL_CTL_DEL, fd, NULL);

    return true;
}

/******************************** private ********************************/
