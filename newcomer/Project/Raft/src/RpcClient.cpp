#include "RpcClient.h"
#include "RpcInvoker.h"
#include "font_properties.h"

void dumpMessage(RpcMessage *msg);

//////////////////////////////// public ////////////////////////////////

bool RpcClient::requestVote(RequestVoteResult &result, uint16_t port,
                            int term, int candidateId, int lastLogIndex, int lastLogTerm)
{
    char buf[BUFSIZ];
    RpcMessage *msg = (RpcMessage *) buf;

    RequestVoteArg arg;
    arg.term = term;
    arg.candidateId = candidateId;
    arg.lastLogIndex = lastLogIndex;
    arg.lastLogTerm = lastLogTerm;

    RpcInvoker::encodeRequestVoteArg(arg, msg);

    do
    {
        if (!connectServer(port)) break;

        if (!sendToServer(buf, MESSAGE_SIZE(msg))) break;

        if (!recvFromServer(buf, BUFSIZ)) break;

        closeConnection();

        msg = (RpcMessage *) buf;
        result = RpcInvoker::decodeRequestVoteResult(msg);
        return true;
    } while (1);

    closeConnection();
    return false;
}

bool RpcClient::appendEntries(AppendEntriesResult &result, uint16_t port,
                              int term, int leaderId, int prevLogIndex,
                              int prevLogTerm, int leaderCommit, std::vector<Log> &entries)
{
    char buf[BUFSIZ];
    RpcMessage *msg = (RpcMessage *) buf;

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
        if (!connectServer(port)) break;

        if (!sendToServer(buf, MESSAGE_SIZE(msg))) break;

        if (!recvFromServer(buf, BUFSIZ)) break;

        closeConnection();

        msg = (RpcMessage *) buf;
        result = RpcInvoker::decodeAppendEntriesResult(msg);
        return true;
    } while (1);

    closeConnection();
    return false;
}

//////////////////////////////// private ////////////////////////////////

bool RpcClient::connectServer(uint16_t port)
{
    struct addrinfo hints, *servaddr;

    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo("127.0.0.1",
                    std::to_string(port).c_str(), /* 将大端序的port还原成小端序后转为字符串 */
                    &hints, &servaddr) != 0)
    {
        perror(STDOUT_RED "#ERROR# getaddrinfo" STDOUT_NONE);
        return false;
    }

    /* 创建与服务端的连接套接字 */
    mSockfd = socket(servaddr->ai_family, servaddr->ai_socktype, servaddr->ai_protocol);
    if (mSockfd < 0)
    {
        perror(STDOUT_RED "#ERROR# socket" STDOUT_NONE);
        return false;
    }

    /* 连接服务端 */
    if (connect(mSockfd, servaddr->ai_addr, servaddr->ai_addrlen) < 0)
    {
        //perror(STDOUT_RED "#ERROR# connect" STDOUT_NONE);
        return false;
    }

    return true;
}

bool RpcClient::sendToServer(char *buf, size_t len)
{
    if (send(mSockfd, buf, len, 0) < 0)
    {
        perror(STDOUT_RED "#ERROR# send" STDOUT_NONE);
        return false;
    }
    return true;
}

bool RpcClient::recvFromServer(char *buf, size_t len)
{
    bzero(buf, len);
    if (recv(mSockfd, buf, len, 0) < 0)
    {
        perror(STDOUT_RED "#ERROR# recv" STDOUT_NONE);
        return false;
    }
    return true;
}

bool RpcClient::closeConnection()
{
    return (close(mSockfd) != -1);
}
