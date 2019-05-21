#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <string>
#include "RpcServer.h"
#include "RpcProtocol.h"
#include "RpcServerInvoker.h"

struct ThreadParam
{
    int sockfd;
    struct sockaddr_in addr;
};

void *recvThread(void *param)
{
    ThreadParam *p = (ThreadParam *) param;
    int cli_sock = p->sockfd;
    struct sockaddr_in cli_addr = p->addr;
    char buf[BUFSIZ];

    /* 接收客户端的消息 */
    if (recv(cli_sock, buf, sizeof(buf), 0) < 0)
    {
        perror("recv");
        return NULL;
    }

    /* 将客户端地址信息附加到 msg 后执行服务调用 */
    RpcMessage *msg = (RpcMessage *) buf;
    msg->clientAddr = cli_addr.sin_addr.s_addr;
    msg->clientPort = cli_addr.sin_port;

    RpcServerInvoker invoker;
    if (!invoker.invoke(msg)) /* 返回值通过 msg 传出 */
    {
        return NULL;
    }

    /* 将结果返回给客户端 */
    if (send(cli_sock, msg, MESSAGE_LEN(msg), 0) < 0)
    {
        perror("send");
    }

    return NULL;
}

/**
 * @param port 服务器的TCP监听端口, 大端序
 */
void RpcServer::run(uint16_t port)
{
    /* 创建监听套接字 */
    int listen_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (listen_sock < 0)
    {
        perror("socket");
        return;
    }

    /* 服务端地址结构 */
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = port;

    /* 将监听套接字绑定到指定地址 */
    if (bind(listen_sock, (struct sockaddr *) &addr, sizeof(addr)) != 0)
    {
        perror("bind");
        close(listen_sock);
        return;
    }

    /* 开始监听 */
    if (listen(listen_sock, UINT16_MAX) != 0)
    {
        close(listen_sock);
        perror("listen");
        return;
    }

    printf("server listening at %d...\n", htons(port));

    /* 客户端地址结构 */
    struct sockaddr_in cli_addr;
    socklen_t len = sizeof(cli_addr);

    for (;;)
    {
        /* 接收客户端的连接请求 */
        int cli_sock = accept(listen_sock, (struct sockaddr *) &cli_addr, &len);
        if (cli_sock < 0)
        {
            perror("accept");
            close(listen_sock);
            return;
        }
        printf("received a connection from %s:%d\n",
               inet_ntoa(cli_addr.sin_addr), htons(cli_addr.sin_port));

        /* 创建线程, 处理连接请求 */
        pthread_t tid;
        void *retval;
        ThreadParam param;
        param.sockfd = cli_sock;
        param.addr = cli_addr;
        int ret = pthread_create(&tid, NULL, recvThread, &param);
        if (ret != 0)
        {
            perror("pthread_create");
        }

        pthread_join(tid, &retval); /* 等待线程结束 */
        close(cli_sock); /* 回收文件描述符, 否则大量的连接会导致 accept 报错: too many open files */
    }
}
