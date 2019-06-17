#ifndef RAFT_RPCPROTOCOL_H
#define RAFT_RPCPROTOCOL_H

/* definition for RpcMessage::Header::opCode */
#define REQUEST_VOTE           100
#define REQUEST_VOTE_REPLY     101
#define APPEND_ENTRIES         102
#define APPEND_ENTRIES_REPLY   103

/* definition for RpcMessage::Header::status */
#define STATUS_SUCCESS 0
#define STATUS_ERROR   1

/* definition for RpcMessage::Header::type */
#define ARGUMENT       1000
#define RETURN_VALUE   1001

struct RpcMessage
{
    struct
    {
        int opCode;
        int type;
        int status;
    } Header;
    int dataSize;
    char data[1];
}__attribute__((packed));

#define MESSAGE_SIZE(pMsg) (sizeof(pMsg->Header)+sizeof(pMsg->dataSize)+pMsg->dataSize)

#endif /* RAFT_RPCPROTOCOL_H */
