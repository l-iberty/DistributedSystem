#include <vector>
#include <string.h>
#include "ServiceStub.h"
#include "RpcCoderHelper.h"
#include "RpcProtocol.h"

void sayHelloStub(RpcMessage *msg)
{
    std::vector<std::string> strs;
    std::vector<int> nums;

    /* 消息解码, 获取参数 */
    std::string s = msg->u2.arg;
    RpcCoderHelper::decode(s, strs, nums);
    std::string arg = strs[0];

    /* 具体的服务调用 */
    RpcServiceImpl service;
    std::string ret = service.sayHello(arg);

    /* 消息编码, 封装返回值 */
    strs.clear();
    strs.push_back(ret);
    s = RpcCoderHelper::encode(strs, nums);
    strcpy(msg->u2.ret, s.c_str());
    msg->u1.retLen = s.length();
}

void introduceStub(RpcMessage *msg)
{
    std::vector<std::string> strs;
    std::vector<int> nums;

    /* 消息编码, 封装返回值 */
    RpcServiceImpl service;
    std::string s;
    std::map<std::string, std::string> ret = service.introduce();
    std::map<std::string, std::string>::iterator it = ret.begin();
    for (; it != ret.end(); it++)
    {
        std::string k = it->first;
        std::string v = it->second;
        s = k + "," + v;
        strs.push_back(s);
    }

    s = RpcCoderHelper::encode(strs, nums);
    strcpy(msg->u2.ret, s.c_str());
    msg->u1.retLen = s.length();
}
