#ifndef RPCINVOKER_H
#define RPCINVOKER_H

#include "RpcProtocol.h"

class RpcInvoker
{
public:
    virtual bool invoke(RpcMessage *msg) = 0;
};

#endif /* RPCINVOKER_H */
