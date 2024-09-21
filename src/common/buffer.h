#ifndef __BUFFER_H__
#define __BUFFER_H__

const int BUF_SIZE_INIT = 128;  /* 《effective C++》 item 2：尽量以const、enum、inline替换 #define */

class Buffer
{
public:
    Buffer(int bufsize = BUF_SIZE_INIT);
    ~Buffer();
    
    void Append(const char *str, size_t len);               /* 向缓冲区写入数据 */
    void Append(const std::string &str);
    void Append(const void *data, size_t len);

    ssize_t WriteFd(int sockfd, std::string &err_str);      /* 将缓冲区中的数据，发送到socket通信的对端 */
    ssize_t RecvFd(int sockfd, std::string &err_str);       /* 从socket通信的对端，接收数据到缓冲区 */

    size_t Size() const;
    size_t WriteableBytes() const;                          /* 缓存区中可以读取的字节数 */
    size_t ReadableBytes() const;                           /* 缓存区中可以写入的字节数 */
    size_t HaveReadBytes() const;                           /* 缓存区中已经读取的字节数 */
    size_t HaveWriteBytes() const;                          /* 缓冲区中已经写入的字节数 */
    const char *BeginPtr() const;                           /* 缓冲区头。有了缓冲区头+写入的数据长度，便可以在类外使用writev函数进行写入 */
    const char *Rptr() const {return m_data + m_rpos;}      /* 当前的读取指针的位置 */ 
    const char *Wptr() const {return m_data + m_wpos;}      /* 当前的写入指针的位置 */
    void UpdateRptr(size_t pos);                            /* 更新读指针的位置：读指针是对外使用的接口，外部调用读取数据，要及时更新指针的位置 */
    std::string All2str();                                  /* 将缓冲区中的内容输出成string */
    void Init();                                            /* 将指针和变量初始化 */

private:
    
    void EnsureWriteable(size_t len);                       /* 检查空间是否足够 */
    void AllocateSpace(size_t len);                         /* 获得更多内存并拷贝已有元素 */
    void Free();                                            /* 销毁元素并释放内存 */
    
    char *m_data;                                           /* 底层的数据结构，char数组, char数组的头指针 */
    char *m_end;                                            /* char数组的尾指针 */
    std::string m_str;                                      /* string */
    size_t m_rpos;
    size_t m_wpos;
};

#endif
