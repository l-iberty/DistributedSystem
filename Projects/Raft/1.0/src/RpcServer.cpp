#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include "RpcServer.h"
#include "RpcInvoker.h"
#include "font_properties.h"


struct ThreadParam
{
    int sockfd;
    struct sockaddr_in addr;
    RaftNode *rfNode;
};

void *recvThread(void *param)
{
    ThreadParam *p = (ThreadParam *) param;
    int cli_sock = p->sockfd;
    struct sockaddr_in cli_addr = p->addr;
    RaftNode *rfNode = p->rfNode;
    char buf[BUFSIZ];

    /* 接收客户端的消息 */
    if (recv(cli_sock, buf, sizeof(buf), 0) < 0)
    {
        perror(STDOUT_RED "#ERROR# recv" STDOUT_NONE);
        return NULL;
    }

    /* 调用 RPC 服务 */
    RpcMessage *msg = (RpcMessage *) buf;
    RpcInvoker::invoke(rfNode, msg);

    /* 将结果返回给客户端 */
    if (send(cli_sock, msg, MESSAGE_SIZE(msg), 0) < 0)
    {
        perror(STDOUT_RED "#ERROR# send" STDOUT_NONE);
    }

    return NULL;
}

void RpcServer::run(void *ptr)
{
    RaftNode *rfNode = (RaftNode *) ptr;

    /* 创建监听套接字 */
    int listen_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (listen_sock < 0)
    {
        perror(STDOUT_RED "#ERROR# socket" STDOUT_NONE);
        return;
    }

    /* 服务端地址结构 */
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(rfNode->serverPort());

    /* 将监听套接字绑定到指定地址 */
    if (bind(listen_sock, (struct sockaddr *) &addr, sizeof(addr)) != 0)
    {
        perror(STDOUT_RED "#ERROR# bind" STDOUT_NONE);
        close(listen_sock);
        return;
    }

    /* 开始监听 */
    if (listen(listen_sock, UINT16_MAX) != 0)
    {
        close(listen_sock);
        perror(STDOUT_RED "#ERROR# listen" STDOUT_NONE);
        return;
    }

    printf("server listening at %d...\n", rfNode->serverPort());

    /* 客户端地址结构 */
    struct sockaddr_in cli_addr;
    socklen_t len = sizeof(cli_addr);

    for (;;)
    {
        /* 接收客户端的连接请求 */
        int cli_sock = accept(listen_sock, (struct sockaddr *) &cli_addr, &len);
        if (cli_sock < 0)
        {
            perror(STDOUT_RED "#ERROR# accept" STDOUT_NONE);
            close(listen_sock);
            return;
        }
        //printf("received a connection from %s:%d\n",
               //inet_ntoa(cli_addr.sin_addr), htons(cli_addr.sin_port));

        /* 创建线程, 处理连接请求 */
        pthread_t tid;
        void *retval;
        ThreadParam param;
        param.sockfd = cli_sock;
        param.addr = cli_addr;
        param.rfNode = rfNode;
        if (pthread_create(&tid, NULL, recvThread, &param) != 0)
        {
            perror(STDOUT_RED "#ERROR# pthread_create" STDOUT_NONE);
        }

        pthread_join(tid, &retval); /* 等待线程结束 */
        close(cli_sock); /* 回收文件描述符, 否则大量的连接会导致 accept 报错: too many open files */
    }
}

