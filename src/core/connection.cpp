#include "common.h"
#include "core.h"

std::atomic<int> ConnectionBase::g_atomic_connection_cnts(0);

/******************************** public ********************************/
ConnectionBase::ConnectionBase(SOCKET_T fd, char *ip, uint16_t port)
{
    if (fd < 0)
    {
        LOGE("error:invalid fd");
        return;
    }
    g_atomic_connection_cnts++;

    m_host = ip;
    m_fd = fd;
    m_port = port;

    m_rbuf.Init();
    m_wbuf.Init();
    m_closed = false;

    LOGI("[%d] %s:%d ==> [this], usrCnt[%d]", m_fd, ip, m_port, (int)ConnectionBase::g_atomic_connection_cnts);
}

ConnectionBase::~ConnectionBase()
{
    g_atomic_connection_cnts--;
}

ConnTask_t ConnectionBase::OnRead()
{
    int bytes = 0;
    std::string err_str;
    ConnectionBase *base_ptr = this;

    bytes = m_rbuf.RecvFd(m_fd, err_str);
    if (0 >= bytes || 0 < err_str.length())
    {
        LOGI("Conntction [%d %s:%d] recv error: [%s]", m_fd, m_host.c_str(), m_port, err_str.c_str());
        return nullptr;
    }

    return [&](){
        if (CONN_TCP == m_type)
        {
            char *pos = _strnstr(m_rbuf.Rptr(), "\r\n\r\n", m_rbuf.ReadableBytes());
            if (NULL != pos)
            {
                HttpConnection *conn = dynamic_cast<HttpConnection*>(base_ptr);
            }
        }
    };
}

/******************************** private ********************************/
