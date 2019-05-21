#ifndef RPCSERVERINVOKER_H
#define RPCSERVERINVOKER_H

#include "RpcInvoker.h"
#include "RpcServiceStub.h"
#include "RpcServiceStub.h"

class RpcServerInvoker : public RpcInvoker
{
public:
    RpcServerInvoker();

    ~RpcServerInvoker() = default;

    bool invoke(RpcMessage *msg) override;

private:
    RpcServiceStub mServiceStub;
};


#endif /* RPCSERVERINVOKER_H */
