/*
 * @Author: Jnsforgit jiangnen123456@qq.com
 * @Date: 2024-09-10 17:16:54
 * @LastEditors: Jnsforgit jiangnen123456@qq.com
 * @LastEditTime: 2024-09-10 17:16:55
 * @FilePath: /backend/backend/c++/anyserver/src/common/utils.cpp
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
#include "common.h"

int Utils::SetNoneBlocking(SOCKET_T fd)
{
    int oldflag = fcntl(fd, F_GETFL);
    int newflag = oldflag | O_NONBLOCK;

    return fcntl(fd, F_SETFL, newflag);
}

char* _strnstr(const char *haystack, const char *needle, size_t len) 
{
    size_t i, j;
    
    for(i = 0; i < len; i++) 
    {
        for (j = 0; needle[j] && haystack[i + j] == needle[j]; j++)
        {

        }

        if (needle[j] == 0)
        {
            return (char*)haystack + i;
        }
    }

    return NULL;
}
