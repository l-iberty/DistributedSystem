#ifndef RAFT_RPCSERVER_H
#define RAFT_RPCSERVER_H

#include "RpcProtocol.h"


class RpcServer
{
public:
    RpcServer() = default;

    ~RpcServer() = default;

    void run(void *ptr);
};


#endif /* RAFT_RPCSERVER_H */
