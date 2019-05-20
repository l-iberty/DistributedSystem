#ifndef RPCSERVICE_H
#define RPCSERVICE_H

#include <string>
#include <stdint.h>

struct ServiceAddr
{
    uint32_t addr;
    uint16_t port;
};

class RpcService
{
public:
    virtual std::string sayHello(std::string str) = 0;

    ServiceAddr getServiceAddr()
    {
        return mServiceAddr;
    }

    void setServiceAddr(uint32_t addr, uint16_t port)
    {
        mServiceAddr.addr = addr;
        mServiceAddr.port = port;
    }

private:
    ServiceAddr mServiceAddr;
};

#endif /* RPCSERVICE_H */
