#include "TCPServer.h"
#include "font_properties.h"

TCPServer::TCPServer(uint16_t port) : mListenSockfd(-1), mClientSockfd(-1)
{
    // 创建监听套接字
    mListenSockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (mListenSockfd < 0)
    {
        perror(STDOUT_RED "#ERROR# socket" STDOUT_NONE);
        return;
    }

    /* 服务端地址结构 */
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(port);

    /* 将监听套接字绑定到指定地址 */
    if (bind(mListenSockfd, (struct sockaddr*) &addr, sizeof(addr)) != 0)
    {
        perror(STDOUT_RED "#ERROR# bind" STDOUT_NONE);
    }
}

TCPServer::~TCPServer()
{
    close(mListenSockfd);
}

bool TCPServer::listen()
{
    return (::listen(mListenSockfd, UINT16_MAX) == 0);
}

bool TCPServer::accept()
{
    mClientSockfd = ::accept(mListenSockfd, (struct sockaddr*) &mClientAddr, &mClientAddrLen);
    return (mClientSockfd >= 0);
}

bool TCPServer::recvFromClient(void* buf, size_t len)
{
    return recv(mClientSockfd, buf, len);
}

bool TCPServer::sendToClient(void* buf, size_t len)
{
    return send(mClientSockfd, buf, len);
}

bool TCPServer::closeConnection()
{
    return close(mClientSockfd);
}
