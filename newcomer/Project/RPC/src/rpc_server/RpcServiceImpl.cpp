#include "RpcServiceImpl.h"

std::string RpcServiceImpl::sayHello(std::string str)
{
    return "hello," + str;
}

std::map<std::string, std::string> RpcServiceImpl::introduce()
{
    std::map<std::string, std::string> m;
    m["name"] = "Bob";
    m["email"] = "Bob@xxx.com";

    return m;
}