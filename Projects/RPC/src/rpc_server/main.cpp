#include <iostream>
#include <netinet/in.h>
#include "RpcServer.h"

int main()
{
    RpcServer server;
    server.run(htons(5000));
    return 0;
}