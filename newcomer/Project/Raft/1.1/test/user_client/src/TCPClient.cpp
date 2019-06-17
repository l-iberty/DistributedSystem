#include "TCPClient.h"
#include "font_properties.h"

bool TCPClient::connectServer(const char* addr, uint16_t port)
{
    struct addrinfo hints, * servaddr;

    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(addr,
                    std::to_string(port).c_str(),
                    &hints, &servaddr) != 0)
    {
        perror(STDOUT_RED "#ERROR# getaddrinfo" STDOUT_NONE);
        return false;
    }

    /* 创建与服务端的连接套接字 */
    mSockfd = socket(servaddr->ai_family, servaddr->ai_socktype, servaddr->ai_protocol);
    if (mSockfd < 0)
    {
        perror(STDOUT_RED "#ERROR# socket" STDOUT_NONE);
        return false;
    }

    /* 连接服务端 */
    if (connect(mSockfd, servaddr->ai_addr, servaddr->ai_addrlen) < 0)
    {
        //perror(STDOUT_RED "#ERROR# connect" STDOUT_NONE);
        return false;
    }
    return true;
}

bool TCPClient::sendToServer(void* buf, size_t len)
{
    return send(mSockfd, buf, len);
}

bool TCPClient::recvFromServer(void* buf, size_t len)
{
    return recv(mSockfd, buf, len);
}

bool TCPClient::closeConnection()
{
    return close(mSockfd);
}
