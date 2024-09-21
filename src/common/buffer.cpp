#include "common.h"

/******************************** public ********************************/

Buffer::Buffer(int bufsize)
{
    m_data = new char[bufsize];

    assert(m_data);
    m_end = m_data + bufsize;
    m_rpos = 0;
    m_wpos = 0;
}

Buffer::~Buffer()
{
    Free();
}

void Buffer::Append(const char *str, size_t len)
{
    if (str == nullptr)
    {
        LOGE("error:invalid str");
        return;
    }

    EnsureWriteable(len);
    
    /* 移动元素 */
    for (size_t i = 0; i < len; ++i)
    {
        m_data[m_wpos + i] = std::move(str[i]);
    }
    m_wpos += len; /* 更新写指针的位置 */
}

void Buffer::Append(const std::string &str)
{
    Append(str.data(), str.length());
}

void Buffer::Append(const void *data, size_t len)
{
    if (data == nullptr)
    {
        LOGE("error:invalid str");
        return;
    }

    Append(static_cast<const char *>(data), len);
}

/* 从sockfd中读取数据到缓冲区 */ 
/* 返回值：读取正确，返回读取到的字节数；读取错误，对端断线，返回-1 */
ssize_t Buffer::RecvFd(int sockfd, std::string &err_str)
{
    char extra_buf[65536] = {0};                /* 额外扩展空间 */ 
    ssize_t writeable = WriteableBytes();       /* buffer剩余空闲 */ 
    struct iovec iov[2];                        /* 分散读写的结构体 */

    /* 使用readv进行读取, 装填结构体 */ 
    iov[0].iov_base = const_cast<char *>(Wptr());
    iov[0].iov_len = writeable;
    iov[1].iov_base = extra_buf;
    iov[1].iov_len = sizeof(extra_buf);

    /* 调用readv */
    int len = readv(sockfd, iov, 2);
    if (len < 0)                        /* 错误发生 */
    {
        err_str = ERRSTR;
    }
    else if (len <= writeable)          /* 读取到的数据，没有写入到extraBuf中 */
    {
        m_wpos += len;
    }
    else
    {
        m_wpos = Size();
        Append(extra_buf, len - writeable);
    }

    return len;
}

ssize_t Buffer::WriteFd(int sockfd, std::string &err_str)
{
    /* 调用send函数，向sockfd中发送数据 */ 
    ssize_t len = 0;
    ssize_t send_bytes = 0;

    while(true)
    {
        len = send(sockfd, m_data + HaveReadBytes(), ReadableBytes() - len, 0);
        if (len == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                err_str = ERRSTR;
                break;
            }

            err_str = ERRSTR;
            return -1;
        }

        if (len == 0)   /* 对端关闭了连接 */ 
        {
            err_str = ERRSTR;
            return -1;
        }

        m_rpos += len;
        send_bytes += len;
    }

    return send_bytes;
}

/* 返回缓冲区的大小 */
size_t Buffer::Size() const
{
    return m_end - m_data;
}

/* 缓存区中可以写入的字节数 */
size_t Buffer::WriteableBytes() const
{
    return Size() - m_wpos;
}

/* 缓存区中可以读取的字节数 */
size_t Buffer::ReadableBytes() const
{
    return m_wpos - m_rpos;
}

/* 缓存区中已经读取的字节数 */
size_t Buffer::HaveReadBytes() const
{
    return m_rpos;
}

/* 缓冲区中已经写入的字节数 */
size_t Buffer::HaveWriteBytes() const
{
    return m_wpos;
}

void Buffer::UpdateRptr(size_t pos)
{
    m_rpos += pos;
}

std::string Buffer::All2str()
{
    m_str.assign(m_data, HaveWriteBytes());
    Init();
    return m_str; // 为什么这里可以返回一个局部变量？
}

/* 缓冲区头 */ 
const char *Buffer::BeginPtr() const
{
    return m_data;
}

/******************************** private ********************************/

/* 检查是否有足够的写入空间 */
void Buffer::EnsureWriteable(size_t len)
{
    size_t res = Size() - m_wpos;
    if (res < len)
    {
        AllocateSpace(len);
    }

    res = Size() - m_wpos;
    if (res < len)
    {
        LOGE("扩容失败");
    }
}

/* 扩容 */
void Buffer::AllocateSpace(size_t len)
{
    size_t newlen = 2 * (HaveWriteBytes() + len);
    char *newdata = new char[newlen];
    for (size_t i = 0; i < m_wpos; ++i)
    {
        newdata[i] = std::move(m_data[i]);
    }
    delete[] m_data;
    m_data = newdata;
    m_end = m_data + newlen;
}

void Buffer::Free()
{
    Init();
    delete[] m_data;
    m_data = nullptr;
}

void Buffer::Init()
{
    bzero(m_data, Size());
    m_rpos = 0;
    m_wpos = 0;
}
