#include "RpcServiceStub.h"

void RpcServiceStub::registerService(std::string funcName, ServiceStub serviceStub)
{
    mStubEntry[funcName] = serviceStub;
}

bool RpcServiceStub::isRegistered(std::string funcName)
{
    return (mStubEntry.find(funcName) != mStubEntry.end());
}

ServiceStub RpcServiceStub::getServiceStub(std::string funcName)
{
    if (!isRegistered(funcName)) return NULL;

    return mStubEntry[funcName];
}
