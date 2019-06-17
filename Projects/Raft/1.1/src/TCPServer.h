#ifndef RAFT_TCPSERVER_H
#define RAFT_TCPSERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
#include "TCP.h"

class TCPServer : public TCP
{
public:
    TCPServer(uint16_t port);

    ~TCPServer();

    bool listen();

    bool accept();

    bool recvFromClient(void* buf, size_t len);

    bool sendToClient(void* buf, size_t len);

    bool closeConnection();

private:
    int mListenSockfd; // 服务端的监听套接字
    int mClientSockfd; // 客户端连接套接字

    struct sockaddr_in mClientAddr; // 客户端地址结构
    socklen_t mClientAddrLen = sizeof(mClientAddr);
};


#endif /* RAFT_TCPSERVER_H */
