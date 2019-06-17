#include <stdio.h>
#include <netinet/in.h>
#include "RpcServer.h"
#include "RpcInvoker.h"


void RpcServer::run(void* ptr)
{
    RaftNode* rfNode = static_cast<RaftNode*>(ptr);
    TCPServer server(rfNode->rpcServerPort());
    char buf[BUFSIZ];

    /* 开始监听 */
    if (!server.listen()) return;

    printf("RPC server listening at %d...\n", rfNode->rpcServerPort());

    for (;;)
    {
        /* 接收客户端的连接请求 */
        if (!server.accept()) return;

        /* 接收客户端的消息 */
        if (server.recvFromClient(buf, sizeof(buf)))
        {
            /* 调用 RPC 服务 */
            RpcMessage* msg = (RpcMessage*) buf;
            RpcInvoker::invoke(rfNode, msg);

            /* 将结果返回给客户端 */
            server.sendToClient(msg, MESSAGE_SIZE(msg));
        }
        server.closeConnection();
    }
}
