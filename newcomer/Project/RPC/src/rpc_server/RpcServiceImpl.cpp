#include "RpcServiceImpl.h"

std::string RpcServiceImpl::sayHello(std::string str)
{
    return "hello," + str;
}
