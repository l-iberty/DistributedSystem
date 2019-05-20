#ifndef RPCSERVERINVOKER_H
#define RPCSERVERINVOKER_H

#include "RpcInvoker.h"

class RpcServerInvoker : public RpcInvoker
{
public:
    bool invoke(RpcMessage *msg) override;
};


#endif /* RPCSERVERINVOKER_H */
