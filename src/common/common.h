/*
 * @Author: Jnsforgit jiangnen123456@qq.com
 * @Date: 2024-09-10 17:05:29
 * @LastEditors: Jnsforgit jiangnen123456@qq.com
 * @LastEditTime: 2024-09-14 11:36:41
 * @FilePath: /backend/backend/c++/anyserver/src/common/common.h
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef __COMMON_H__
#define __COMMON_H__

extern "C"
{
    #include <stdio.h>
    #include <string.h> /* bzero */
    #include <strings.h> /* bzero */
    #include <stdarg.h>
    #include <stdint.h>
    
    #include <errno.h>
    #include <alloca.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <assert.h>
    // #include <pthread.h>
    #include <semaphore.h>
    #include <sys/uio.h> /* readv */
    #include <sys/stat.h>
    #include <sys/time.h>
    #include <sys/mman.h>
    #include <sys/types.h>
    #include <sys/epoll.h>
    #include <sys/socket.h> /* recv */
    #include <sys/syscall.h>

    #include <arpa/inet.h>

    #include <netinet/in.h>
}


#include <iostream>
#include <string>
#include <map>
#include <queue>
#include <mutex>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>
#include <type_traits>
#include <vector>
#include <memory> 
#include <functional>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <condition_variable>

#define closesocket(fd) ::close(fd)
#define ERRSTR ::strerror(errno)

typedef int SOCKET_T;

#include "zlog/zlog.h"
#include "locker.h"
#include "threadpool.h"
#include "utils.h"
#include "buffer.h"

#define LOGD(fmt, ...) dzlog_debug(fmt, ##__VA_ARGS__);
#define LOGI(fmt, ...) dzlog_info(fmt, ##__VA_ARGS__);
#define LOGW(fmt, ...) dzlog_warn(fmt, ##__VA_ARGS__);
#define LOGE(fmt, ...) dzlog_error(fmt, ##__VA_ARGS__);

#endif
