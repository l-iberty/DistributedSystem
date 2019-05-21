#include <string.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include "RpcServiceProxyImpl.h"
#include "RpcProtocol.h"
#include "RpcClientInvoker.h"
#include "RpcCoderHelper.h"

std::string RpcServiceProxyImpl::sayHello(std::string str)
{
    std::string res;

    std::vector<std::string> strs;
    std::vector<int> nums;

    char buf[BUFSIZ];
    bzero(buf, BUFSIZ);
    RpcMessage *msg = (RpcMessage *) buf;

    /* 消息编码 */
    strs.push_back(str);
    std::string arg = RpcCoderHelper::encode(strs, nums);
    getServerAddr(&msg->serverAddr, &msg->serverPort);
    strncpy(msg->funcName, "sayHello", MAX_FUNCNAME_LEN);
    msg->u1.argLen = arg.length();
    strcpy(msg->u2.arg, arg.c_str());

    RpcClientInvoker invoker;
    if (invoker.invoke(msg))
    {
        /* 消息解码, 取出返回值 */
        strs.clear();
        std::string s = msg->u2.ret;
        RpcCoderHelper::decode(s, strs, nums);
        res = strs[0];
    }
    else
    {
        std::cerr << "[RpcServiceProxyImpl::sayHello] error" << std::endl;
    }

    return res;
}

std::map<std::string, std::string> RpcServiceProxyImpl::introduce()
{
    std::map<std::string, std::string> res;

    std::vector<std::string> strs;
    std::vector<int> nums;

    char buf[BUFSIZ];
    bzero(buf, BUFSIZ);
    RpcMessage *msg = (RpcMessage *) buf;

    /* 消息编码 */
    getServerAddr(&msg->serverAddr, &msg->serverPort);
    strncpy(msg->funcName, "introduce", MAX_FUNCNAME_LEN);
    msg->u1.argLen = 0;

    RpcClientInvoker invoker;
    if (invoker.invoke(msg))
    {
        /* 消息解码, 取出返回值 */
        std::string s = msg->u2.ret;
        RpcCoderHelper::decode(s, strs, nums);

        for (std::string str:strs)
        {
            std::vector<std::string> tokens = RpcCoderHelper::split(str, ",");
            res[tokens[0]] = tokens[1];
        }
    }
    else
    {
        std::cerr << "[RpcServiceProxyImpl::introduce] error" << std::endl;
    }

    return res;
}

bool RpcServiceProxyImpl::getServerAddr(uint32_t *addr, uint16_t *port)
{
    FILE *fp = fopen("server.cfg", "r");
    if (fp == NULL)
    {
        perror("fopen");
        return false;
    }

    char buf[BUFSIZ];
    bzero(buf, BUFSIZ);
    fgets(buf, BUFSIZ, fp);
    *addr = inet_addr(buf);

    bzero(buf, BUFSIZ);
    fgets(buf, BUFSIZ, fp);
    *port = htons(atoi(buf));

    fclose(fp);
    return true;
}
