#include "RpcClient.h"

RpcService *RpcClient::getService(uint32_t addr, uint16_t port)
{
    RpcService *service = new RpcServiceProxyImpl;
    service->setServiceAddr(addr, port);
    return service;
}
