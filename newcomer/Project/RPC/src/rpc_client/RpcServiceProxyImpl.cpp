#include <string.h>
#include <iostream>
#include <string>
#include "RpcServiceProxyImpl.h"
#include "RpcProtocol.h"
#include "RpcClientInvoker.h"
#include "RpcCoderHelper.h"

std::string RpcServiceProxyImpl::sayHello(std::string str)
{
    std::string res; /* 返回值 */

    std::vector<std::string> strs;
    std::vector<int> nums;

    char buf[BUFSIZ];
    bzero(buf, BUFSIZ);
    RpcMessage *msg = (RpcMessage *) buf;

    /* 消息编码 */
    strs.push_back(str);
    std::string arg = RpcCoderHelper::encode(strs, nums);
    msg->serverAddr = getServiceAddr().addr;
    msg->serverPort = getServiceAddr().port;
    msg->u1.argLen = arg.length();
    strncpy(msg->funcName, "sayHello", MAX_FUNCNAME_LEN);
    strcpy(msg->u2.arg, arg.c_str());

    RpcInvoker *invoker = new RpcClientInvoker;
    if (invoker->invoke(msg))
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

    delete invoker;
    return res;
}
