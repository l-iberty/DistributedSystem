#ifndef RPCPROTOCOL_H
#define RPCPROTOCOL_H

#include <stdio.h>
#include <stdint.h>
#include <netdb.h>

#define MAX_FUNCNAME_LEN 16

struct RpcMessage
{
    /* 客户端/服务端的IPv4地址和TCP端口号, 均按照大端序存储 */
    uint32_t clientAddr;
    uint32_t serverAddr;
    uint16_t clientPort;
    uint16_t serverPort;

    /* 函数名 */
    char funcName[MAX_FUNCNAME_LEN];

    /**
     * 参数(返回值)表只包含2种类型: 整形 & 字符串, 参数(返回值)间用"#"分割,
     * 形如: "int:10#string[5]:hello", 表示整数10和长度为5的字符串"hello".
     */
    union
    {
        uint32_t argLen; /* 参数字符串的长度 */
        uint32_t retLen; /* 返回值字符串的长度 */
    } u1;
    union
    {
        char arg[1]; /* 参数字符串 */
        char ret[1]; /* 返回值字符串 */
    } u2;
};

#define MESSAGE_LEN(msg) (sizeof(RpcMessage)-sizeof(RpcMessage::u2)+msg->u1.argLen)

void dumpMessage(RpcMessage *msg);

#endif /* RPCPROTOCOL_H */
