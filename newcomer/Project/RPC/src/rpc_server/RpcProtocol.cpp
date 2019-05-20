#include <string.h>
#include <arpa/inet.h>
#include "RpcProtocol.h"

void dumpMessage(RpcMessage *msg)
{
    in_addr in;
    char szClientAddr[16], szServerAddr[16];

    in.s_addr = msg->serverAddr;
    strcpy(szClientAddr, inet_ntoa(in));
    in.s_addr = msg->serverAddr;
    strcpy(szServerAddr, inet_ntoa(in));

    printf("# clientAddr: %s, clientPort: %d\n", szClientAddr, htons(msg->clientPort));
    printf("# serverAddr: %s, serverPort: %d\n", szServerAddr, htons(msg->serverPort));
    printf("# funcName: %s\n", msg->funcName);
    printf("# arg or ret: %s\n", msg->u2.arg);
}
