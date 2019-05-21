#ifndef RPCSERVICEPROXYIMPL_H
#define RPCSERVICEPROXYIMPL_H

#include <string>
#include "RpcService.h"

/**
 * RpcServiceProxyImpl 负责对服务端接口进行代理实现
 */
class RpcServiceProxyImpl : public RpcService
{
public:
    std::string sayHello(std::string str) override;

    std::map<std::string,std::string> introduce() override;

private:
    bool getServerAddr(uint32_t *addr, uint16_t *port);
};


#endif /* RPCSERVICEPROXYIMPL_H */
