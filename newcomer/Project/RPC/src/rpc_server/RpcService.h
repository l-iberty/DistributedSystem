#ifndef RPCSERVICE_H
#define RPCSERVICE_H

#include <string>
#include <map>

class RpcService
{
public:
    virtual std::string sayHello(std::string str) = 0;

    virtual std::map<std::string,std::string> introduce() = 0;
};

#endif /* RPCSERVICE_H */
