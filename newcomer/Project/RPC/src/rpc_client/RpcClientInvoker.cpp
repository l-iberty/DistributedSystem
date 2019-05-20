#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include "RpcClientInvoker.h"
#include "RpcProtocol.h"

/**
 * 客户端的每次invoke()调用都会建立一个单独的连接, 所以客户端每次发送和收到
 * 的消息都是一一对应的.
 */
bool RpcClientInvoker::invoke(RpcMessage *msg)
{
    char buf[BUFSIZ];

    if (!connectServer(msg->serverAddr, msg->serverPort))
    {
        return false;
    }

    if (!sendToServer((char *) msg, MESSAGE_LEN(msg)))
    {
        closeConnection();
        return false;
    }

    for (;;)
    {
        bzero(buf, BUFSIZ);
        if (!recvFromServer(buf, BUFSIZ))
        {
            closeConnection();
            return false;
        }
        closeConnection();

        RpcMessage *recvMsg = (RpcMessage *) buf;

        dumpMessage(recvMsg);
        memcpy(msg, recvMsg, MESSAGE_LEN(recvMsg)); /* 传出服务端的调用结果 */
        return true;
    }

    return false;
}

/**
 * @param addr 服务端IPv4地址, 大端序
 * @param port 服务端TCP端口号, 大端序
 * @return 连接是否成功
 */
bool RpcClientInvoker::connectServer(uint32_t addr, uint16_t port)
{
    struct addrinfo hints, *servaddr;

    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    in_addr in;
    in.s_addr = addr;
    if (getaddrinfo(inet_ntoa(in), /* 点分十进制形式的字符串 */
                    std::to_string(htons(port)).c_str(), /* 将大端序的port还原成小端序后转为字符串 */
                    &hints, &servaddr) != 0)
    {
        perror("getaddrinfo");
        return false;
    }

    /* 创建与服务端的连接套接字 */
    mSockfd = socket(servaddr->ai_family, servaddr->ai_socktype, servaddr->ai_protocol);
    if (mSockfd < 0)
    {
        perror("socket");
        return false;
    }

    /* 连接服务端 */
    if (connect(mSockfd, servaddr->ai_addr, servaddr->ai_addrlen) < 0)
    {
        perror("connect");
        return false;
    }

    return true;
}

bool RpcClientInvoker::sendToServer(char *buf, size_t len)
{
    if (send(mSockfd, buf, len, 0) < 0)
    {
        perror("send");
        return false;
    }
    return true;
}

bool RpcClientInvoker::recvFromServer(char *buf, size_t len)
{
    if (recv(mSockfd, buf, len, 0) < 0)
    {
        perror("recv");
        return false;
    }
    return true;
}

bool RpcClientInvoker::closeConnection()
{
    return (close(mSockfd) != -1);
}
