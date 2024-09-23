#ifndef __CONNECTION_H__
#define __CONNECTION_H__

typedef std::function<void (void)> ConnTask_t;

class ConnectionBase
{
    typedef enum  
    {
        CONN_TCP = 0, 
        CONN_HTTP, 
        CONN_UDP, 
    }ConnectionType_e; 

public:
    ConnectionBase(SOCKET_T fd, char *ip, uint16_t port);
    ~ConnectionBase();

    ConnTask_t OnRead();

    SOCKET_T Fd() { return m_fd; }
    std::string Ip() { return m_host; }
    uint16_t Port() { return m_port; }

    static std::atomic<int> g_atomic_connection_cnts;

private:

    Buffer m_rbuf;
    Buffer m_wbuf;

    bool m_closed = true;
    SOCKET_T m_fd = -1;
    uint16_t m_port = 0;
    std::string m_host;
    ConnectionType_e m_type = CONN_TCP;
};

#endif
