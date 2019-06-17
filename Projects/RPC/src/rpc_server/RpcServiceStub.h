#ifndef RPCSERVICESTUB_H
#define RPCSERVICESTUB_H

#include <map>
#include "RpcService.h"
#include "RpcServiceImpl.h"
#include "RpcProtocol.h"

typedef void (*ServiceStub)(RpcMessage *);

class RpcServiceStub
{
public:
    void registerService(std::string funcName, ServiceStub serverStub);

    bool isRegistered(std::string funcName);

    ServiceStub getServiceStub(std::string funcName);

private:
    std::map<std::string, ServiceStub> mStubEntry;
};


#endif /* RPCSERVICESTUB_H */
