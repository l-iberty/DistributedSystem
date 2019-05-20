#include <string.h>
#include <iostream>
#include "RpcServerInvoker.h"
#include "RpcService.h"
#include "RpcServiceImpl.h"
#include "RpcProtocol.h"
#include "RpcCoderHelper.h"

bool RpcServerInvoker::invoke(RpcMessage *msg)
{
    dumpMessage(msg);

    std::vector<std::string> strs;
    std::vector<int> nums;

    /* 调用 RpcServiceImpl 的函数, 将结果通过 msg 传出 */

    if (!strcmp(msg->funcName, "sayHello"))
    {
        RpcService *service = new RpcServiceImpl;

        /* 消息解码, 获取参数 */
        std::string s = msg->u2.arg;
        RpcCoderHelper::decode(s, strs, nums);
        std::string arg = strs[0];

        /* 具体的服务调用 */
        std::string ret = service->sayHello(arg);

        /* 消息编码, 封装返回值 */
        strs.clear();
        strs.push_back(ret);
        s = RpcCoderHelper::encode(strs, nums);
        strcpy(msg->u2.ret, s.c_str());
        msg->u1.retLen = s.length();

        delete service;
        return true;
    }

    std::cerr << "unknown funcName" << std::endl;
    return false;
}
