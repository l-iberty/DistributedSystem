#ifndef RPCCLIENT_H
#define RPCCLIENT_H

#include <stdint.h>
#include "RpcService.h"
#include "RpcServiceProxyImpl.h"

class RpcClient
{
public:
    RpcService *getService(uint32_t addr, uint16_t port);
};


#endif /* RPCCLIENT_H */
