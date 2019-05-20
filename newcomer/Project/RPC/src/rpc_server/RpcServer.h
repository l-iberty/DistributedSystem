#ifndef RPCSERVER_H
#define RPCSERVER_H

#include <stdint.h>

/**
 * RpcServer::run()在服务端的指定端口上监听, 一旦收到客户端的连接请求就创建一个线程T
 * 处理之. 线程T对接收到的客户端消息稍作处理后便将其传递给 RpcInvoker, 由 RpcInvoker
 * 处理服务调用过程. 最后, 线程T将服务的调用结果发送给客户端.
 */

class RpcServer
{
public:
    void run(uint16_t port);
};


#endif /* RPCSERVER_H */
