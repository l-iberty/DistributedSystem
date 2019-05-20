#ifndef RPCCLIENTINVOKER_H
#define RPCCLIENTINVOKER_H

#include <netinet/in.h>
#include "RpcInvoker.h"

class RpcClientInvoker : public RpcInvoker
{
public:
    bool invoke(RpcMessage *msg) override;

private:
    bool connectServer(uint32_t addr, uint16_t port);

    bool sendToServer(char *buf, size_t len);

    bool recvFromServer(char *buf, size_t len);

    bool closeConnection();

private:
    int mSockfd;
    struct sockaddr_in mServAddr;
};


#endif /* RPCCLIENTINVOKER_H */
