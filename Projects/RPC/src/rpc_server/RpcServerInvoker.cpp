#include <iostream>
#include "RpcServerInvoker.h"
#include "RpcService.h"
#include "RpcServiceImpl.h"
#include "RpcProtocol.h"
#include "RpcCoderHelper.h"
#include "RpcServiceStub.h"
#include "ServiceStub.h"

RpcServerInvoker::RpcServerInvoker()
{
    mServiceStub.registerService("sayHello", sayHelloStub);
    mServiceStub.registerService("introduce", introduceStub);
}

bool RpcServerInvoker::invoke(RpcMessage *msg)
{
    dumpMessage(msg);

    if (mServiceStub.isRegistered(msg->funcName))
    {
        ServiceStub serviceStub = mServiceStub.getServiceStub(msg->funcName);
        serviceStub(msg);
        return true;
    }

    std::cerr << "unknown funcName" << std::endl;
    return false;
}
