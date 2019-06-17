#ifndef SERVICESTUB_H
#define SERVICESTUB_H

#include "RpcService.h"
#include "RpcServiceImpl.h"
#include "RpcProtocol.h"

void sayHelloStub(RpcMessage *msg);

void introduceStub(RpcMessage *msg);

#endif /* SERVICESTUB_H */
