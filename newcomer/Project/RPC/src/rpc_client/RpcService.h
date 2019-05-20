#ifndef RPCSERVICE_H
#define RPCSERVICE_H

#include <string>
#include <stdint.h>

struct ServiceAddr
{
    uint32_t addr; /* IPv4地址, 大端序 */
    uint16_t port; /* TCP端口号, 大端序 */
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
