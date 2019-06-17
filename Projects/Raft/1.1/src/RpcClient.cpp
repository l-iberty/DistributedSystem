#include "RpcClient.h"
#include "RpcInvoker.h"


bool RpcClient::requestVote(RequestVoteResult& result, uint16_t port,
                            int term, int candidateId, int lastLogIndex, int lastLogTerm)
{
    char buf[BUFSIZ];
    RpcMessage* msg = (RpcMessage*) buf;

    RequestVoteArg arg;
    arg.term = term;
    arg.candidateId = candidateId;
    arg.lastLogIndex = lastLogIndex;
    arg.lastLogTerm = lastLogTerm;

    RpcInvoker::encodeRequestVoteArg(arg, msg);

    do
    {
        if (!mTCPClient.connectServer("127.0.0.1", port)) break;

        if (!mTCPClient.sendToServer(buf, MESSAGE_SIZE(msg))) break;

        if (!mTCPClient.recvFromServer(buf, BUFSIZ)) break;

        mTCPClient.closeConnection();
        msg = (RpcMessage*) buf;
        result = RpcInvoker::decodeRequestVoteResult(msg);
        return true;
    } while (1);

    mTCPClient.closeConnection();
    return false;
}

bool RpcClient::appendEntries(AppendEntriesResult& result, uint16_t port,
                              int term, int leaderId, int prevLogIndex,
                              int prevLogTerm, int leaderCommit, std::vector<Log>& entries)
{
    char buf[BUFSIZ];
    RpcMessage* msg = (RpcMessage*) buf;

    AppendEntriesArg arg;
    arg.term = term;
    arg.leaderId = leaderId;
    arg.prevLogIndex = prevLogIndex;
    arg.prevLogTerm = prevLogTerm;
    arg.leaderCommit = leaderCommit;
    arg.entries = entries;

    RpcInvoker::encodeAppendEntriesArg(arg, msg);

    do
    {
        if (!mTCPClient.connectServer("127.0.0.1", port)) break;

        if (!mTCPClient.sendToServer(buf, MESSAGE_SIZE(msg))) break;

        if (!mTCPClient.recvFromServer(buf, BUFSIZ)) break;

        mTCPClient.closeConnection();
        msg = (RpcMessage*) buf;
        result = RpcInvoker::decodeAppendEntriesResult(msg);
        return true;
    } while (1);

    mTCPClient.closeConnection();
    return false;
}
