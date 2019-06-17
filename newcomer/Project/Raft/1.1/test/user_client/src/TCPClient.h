#ifndef RAFT_TCPCLIENT_H
#define RAFT_TCPCLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include "TCP.h"

class TCPClient : public TCP
{
public:
    TCPClient() = default;

    ~TCPClient() = default;

    bool connectServer(const char* addr, uint16_t port);

    bool sendToServer(void* buf, size_t len);

    bool recvFromServer(void* buf, size_t len);

    bool closeConnection();

private:
    int mSockfd; // 与服务端的连接套接字
};


#endif /* RAFT_TCPCLIENT_H */
