#ifndef RPCSERVICEIMPL_H
#define RPCSERVICEIMPL_H

#include "RpcService.h"

/**
 * RpcServiceImpl 负责 RpcService 中定义的服务端接口的具体实现
 */

class RpcServiceImpl : public RpcService
{
public:
    std::string sayHello(std::string str) override;
};


#endif /* RPCSERVICEIMPL_H */
