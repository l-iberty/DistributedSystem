#ifndef RAFT_RPCINVOKER_H
#define RAFT_RPCINVOKER_H

#include "RpcProtocol.h"
#include "RaftNode.h"


class RpcInvoker
{
public:
    static void invoke(void *ptr, RpcMessage *msg);

    /* encoder and decoder for RequestVote RPC */

    static void encodeRequestVoteArg(RequestVoteArg &arg, RpcMessage *msg);

    static void encodeRequestVoteResult(RequestVoteResult &res, RpcMessage *msg);

    static RequestVoteArg decodeRequestVoteArg(RpcMessage *msg);

    static RequestVoteResult decodeRequestVoteResult(RpcMessage *msg);

    /* encoder and decoder for AppendEntries RPC */

    static std::string encodeLogs(std::vector<Log> &logs);

    static std::vector<Log> decodeLogs(std::string &s);

    static void encodeAppendEntriesArg(AppendEntriesArg &arg, RpcMessage *msg);

    static void encodeAppendEntriesResult(AppendEntriesResult &res, RpcMessage *msg);

    static AppendEntriesResult decodeAppendEntriesResult(RpcMessage *msg);

    static AppendEntriesArg decodeAppendEntriesArg(RpcMessage *msg);
};


#endif /* RAFT_RPCINVOKER_H */
