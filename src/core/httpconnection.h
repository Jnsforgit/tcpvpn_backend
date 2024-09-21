#ifndef __HTTPCONNECTION_H__
#define __HTTPCONNECTION_H__

class HttpConnection : public ConnectionBase
{
public:
    HttpConnection();
    ~HttpConnection();

    bool Parse();

private:

};

#endif
