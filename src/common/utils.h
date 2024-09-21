#ifndef __UTILS_H__
#define __UTILS_H__

class Utils
{
public:
    static int SetNoneBlocking(SOCKET_T fd);
};

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

template<class T> 
static std::vector<std::string> split(const std::string& s, const T& separator, bool ignore_empty=false, bool split_empty=false) 
{
    struct 
    {
        auto len(const std::string&             s) { return s.length(); }
        auto len(const std::string::value_type* p) { return p ? std::char_traits<std::string::value_type>::length(p) : 0; }
        auto len(const std::string::value_type  c) { return c == std::string::value_type() ? 0 : 1; /*return 1;*/ }
    } util;
    
    if (s.empty()) 
    { 
        /// empty string ///
        if (!split_empty || util.len(separator)) return {""};
        return {};
    }
    
    auto v = std::vector<std::string>();
    auto n = static_cast<std::string::size_type>(util.len(separator));
    if (n == 0) 
    {    
        /// empty separator ///
        if (!split_empty) return {s};
        for (auto&& c : s) v.emplace_back(1, c);
        return v;
    }
    
    auto p = std::string::size_type(0);
    while (1) 
    {      
        /// split with separator ///
        auto pos = s.find(separator, p);
        if (pos == std::string::npos) {
        if (ignore_empty && p - n + 1 == s.size()) break;
        v.emplace_back(s.begin() + p, s.end());
        break;
        }
        if (!ignore_empty || p != pos)
        v.emplace_back(s.begin() + p, s.begin() + pos);
        p = pos + n;
    }

    return v;
}

#endif

