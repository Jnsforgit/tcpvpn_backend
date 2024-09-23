#ifndef __UTILS_H__
#define __UTILS_H__

class Utils
{
public:
    static int SetNoneBlocking(SOCKET_T fd);
};

char* _strnstr(const char *haystack, const char *needle, size_t len);

#endif

