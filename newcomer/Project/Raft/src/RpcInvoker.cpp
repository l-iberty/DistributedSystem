#include "RpcInvoker.h"
#include "font_properties.h"

void dumpMessage(RpcMessage *msg)
{
    printf("opCode: %d\n", msg->Header.opCode);
    printf("type: %d\n", msg->Header.type);
    printf("status: %d\n", msg->Header.status);
    printf("dataSize: %d\n", msg->dataSize);
    printf("data: %s\n", msg->data);
}

//////////////////////////////// public ////////////////////////////////

void RpcInvoker::invoke(void *ptr, RpcMessage *msg)
{
    RaftNode *rfNode = (RaftNode *) ptr;

    switch (msg->Header.opCode)
    {
        case REQUEST_VOTE:
        {
            RequestVoteArg arg = decodeRequestVoteArg(msg);
            RequestVoteResult res =
                    rfNode->onRequestVoteReply(arg.term,
                                               arg.candidateId,
                                               arg.lastLogIndex,
                                               arg.lastLogTerm);
            encodeRequestVoteResult(res, msg);
            break;
        }
        case APPEND_ENTRIES:
        {
            AppendEntriesArg arg = decodeAppendEntriesArg(msg);
            AppendEntriesResult res =
                    rfNode->onAppendEntriesReply(arg.term,
                                                 arg.leaderId,
                                                 arg.prevLogIndex,
                                                 arg.prevLogTerm,
                                                 arg.leaderCommit,
                                                 arg.entries);
            encodeAppendEntriesResult(res, msg);
            break;
        }
        default:
            printf(STDOUT_RED "#ERROR# unknown opCode %d\n" STDOUT_NONE, msg->Header.opCode);
    }
}

void RpcInvoker::encodeRequestVoteArg(RequestVoteArg &arg, RpcMessage *msg)
{
    msg->Header.opCode = REQUEST_VOTE;
    msg->Header.type = ARGUMENT;
    msg->Header.status = STATUS_SUCCESS;

    std::string str = "from:" + std::to_string(arg.from) + "#" +
                      "to:" + std::to_string(arg.to) + "#" +
                      "term:" + std::to_string(arg.term) + "#" +
                      "candidateId:" + std::to_string(arg.candidateId) + "#" +
                      "lastLogIndex:" + std::to_string(arg.lastLogIndex) + "#" +
                      "lastLogTerm:" + std::to_string(arg.lastLogTerm);

    msg->dataSize = str.length() + 1; /* 加上字符串末尾的'\0' */
    strcpy(msg->data, str.c_str());
}

void RpcInvoker::encodeRequestVoteResult(RequestVoteResult &res, RpcMessage *msg)
{
    msg->Header.opCode = REQUEST_VOTE_REPLY;
    msg->Header.type = RETURN_VALUE;
    msg->Header.status = STATUS_SUCCESS;

    std::string str = "from:" + std::to_string(res.from) + "#" +
                      "to:" + std::to_string(res.to) + "#" +
                      "term:" + std::to_string(res.term) + "#" +
                      "voteGranted:" + std::to_string(res.voteGranted);

    msg->dataSize = str.length() + 1; /* 加上字符串末尾的'\0' */
    strcpy(msg->data, str.c_str());
}

RequestVoteArg RpcInvoker::decodeRequestVoteArg(RpcMessage *msg)
{
    RequestVoteArg arg;
    bzero(&arg, sizeof(arg));

    assert(msg->Header.opCode == REQUEST_VOTE);
    assert(msg->Header.type == ARGUMENT);
    assert(msg->Header.status == STATUS_SUCCESS);

    std::string s = msg->data;
    std::vector<std::string> tokens = StringTokenizer::split(s, "#");
    for (std::string &token : tokens)
    {
        std::vector<std::string> toks = StringTokenizer::split(token, ":");
        if (toks[0].compare("from") == 0)
        {
            arg.from = atoi(toks[1].c_str());
        }
        else if (toks[0].compare("to") == 0)
        {
            arg.to = atoi(toks[1].c_str());
        }
        else if (toks[0].compare("term") == 0)
        {
            arg.term = atoi(toks[1].c_str());
        }
        else if (toks[0].compare("candidateId") == 0)
        {
            arg.candidateId = atoi(toks[1].c_str());
        }
        else if (toks[0].compare("lastLogIndex") == 0)
        {
            arg.lastLogIndex = atoi(toks[1].c_str());
        }
        else if (toks[0].compare("lastLogTerm") == 0)
        {
            arg.lastLogTerm = atoi(toks[1].c_str());
        }
    }

    return arg;
}

RequestVoteResult RpcInvoker::decodeRequestVoteResult(RpcMessage *msg)
{
    RequestVoteResult res;
    bzero(&res, sizeof(res));

    assert(msg->Header.opCode == REQUEST_VOTE_REPLY);
    assert(msg->Header.type == RETURN_VALUE);
    assert(msg->Header.status == STATUS_SUCCESS);

    std::string s = msg->data;
    std::vector<std::string> tokens = StringTokenizer::split(s, "#");
    for (std::string &token : tokens)
    {
        std::vector<std::string> toks = StringTokenizer::split(token, ":");
        if (toks[0].compare("from") == 0)
        {
            res.from = atoi(toks[1].c_str());
        }
        else if (toks[0].compare("to") == 0)
        {
            res.to = atoi(toks[1].c_str());
        }
        else if (toks[0].compare("term") == 0)
        {
            res.term = atoi(toks[1].c_str());
        }
        else if (toks[0].compare("voteGranted") == 0)
        {
            res.voteGranted = (bool) atoi(toks[1].c_str());
        }
    }

    return res;
}

/**
 * @brief 将Log数组序列化为以下格式的字符串:
 *        "[id=0,term=1;id=1,term=1;id=1,term=2]"
 *        每个Log结构之间用";"分割, 每个成员变量之间用","分割
 */
std::string RpcInvoker::encodeLogs(std::vector<Log> &logs)
{
    std::string s = "[";

    int i;
    if (!logs.empty())
    {
        for (i = 0; i < logs.size() - 1; i++)
        {
            s += "id=" + std::to_string(logs[i].id) + ",";
            s += "term=" + std::to_string(logs[i].term) + ";";
        }
        s += "id=" + std::to_string(logs[i].id) + ",";
        s += "term=" + std::to_string(logs[i].term);
    }
    s += "]";

    return s;
}

std::vector<Log> RpcInvoker::decodeLogs(std::string &s)
{
    std::vector<Log> logs;
    std::string str = s.substr(1, s.length() - 2); /* 截取[]内的子串 */

    std::vector<std::string> entries = StringTokenizer::split(str, ";");
    for (std::string &entry : entries)
    {
        Log log;
        std::vector<std::string> tokens = StringTokenizer::split(entry, ",");
        for (std::string &token : tokens)
        {
            std::vector<std::string> toks = StringTokenizer::split(token, "=");
            if (toks[0].compare("id") == 0)
            {
                log.id = atoi(toks[1].c_str());
            }
            else if (toks[0].compare("term") == 0)
            {
                log.term = atoi(toks[1].c_str());
            }
        }
        logs.push_back(log);
    }

    return logs;
}

void RpcInvoker::encodeAppendEntriesArg(AppendEntriesArg &arg, RpcMessage *msg)
{
    msg->Header.opCode = APPEND_ENTRIES;
    msg->Header.type = ARGUMENT;
    msg->Header.status = STATUS_SUCCESS;

    std::string str = "from:" + std::to_string(arg.from) + "#" +
                      "to:" + std::to_string(arg.to) + "#" +
                      "term:" + std::to_string(arg.term) + "#" +
                      "leaderId:" + std::to_string(arg.leaderId) + "#" +
                      "prevLogIndex:" + std::to_string(arg.prevLogIndex) + "#" +
                      "prevLogTerm:" + std::to_string(arg.prevLogTerm) + "#" +
                      "leaderCommit:" + std::to_string(arg.leaderCommit) + "#" +
                      "entries:" + encodeLogs(arg.entries);

    msg->dataSize = str.length() + 1; /* 加上字符串末尾的'\0' */
    strcpy(msg->data, str.c_str());
}

void RpcInvoker::encodeAppendEntriesResult(AppendEntriesResult &res, RpcMessage *msg)
{
    msg->Header.opCode = APPEND_ENTRIES_REPLY;
    msg->Header.type = RETURN_VALUE;
    msg->Header.status = STATUS_SUCCESS;

    std::string str = "from:" + std::to_string(res.from) + "#" +
                      "to:" + std::to_string(res.to) + "#" +
                      "term:" + std::to_string(res.term) + "#" +
                      "matchIndex:" + std::to_string(res.matchIndex) + "#" +
                      "success:" + std::to_string(res.success);

    msg->dataSize = str.length() + 1; /* 加上字符串末尾的'\0' */
    strcpy(msg->data, str.c_str());
}

AppendEntriesArg RpcInvoker::decodeAppendEntriesArg(RpcMessage *msg)
{
    AppendEntriesArg arg;
    bzero(&arg, sizeof(arg));

    assert(msg->Header.opCode == APPEND_ENTRIES);
    assert(msg->Header.type == ARGUMENT);
    assert(msg->Header.status == STATUS_SUCCESS);

    std::string s = msg->data;
    std::vector<std::string> tokens = StringTokenizer::split(s, "#");
    for (std::string &token : tokens)
    {
        std::vector<std::string> toks = StringTokenizer::split(token, ":");
        if (toks[0].compare("from") == 0)
        {
            arg.from = atoi(toks[1].c_str());
        }
        else if (toks[0].compare("to") == 0)
        {
            arg.to = atoi(toks[1].c_str());
        }
        else if (toks[0].compare("term") == 0)
        {
            arg.term = atoi(toks[1].c_str());
        }
        else if (toks[0].compare("leaderId") == 0)
        {
            arg.leaderId = atoi(toks[1].c_str());
        }
        else if (toks[0].compare("prevLogIndex") == 0)
        {
            arg.prevLogIndex = atoi(toks[1].c_str());
        }
        else if (toks[0].compare("prevLogTerm") == 0)
        {
            arg.prevLogTerm = atoi(toks[1].c_str());
        }
        else if (toks[0].compare("leaderCommit") == 0)
        {
            arg.leaderCommit = atoi(toks[1].c_str());
        }
        else if (toks[0].compare("entries") == 0)
        {
            std::vector<Log> logs = decodeLogs(toks[1]);
            for (Log &log : logs)
            {
                arg.entries.push_back(log);
            }
        }
    }

    return arg;
}

AppendEntriesResult RpcInvoker::decodeAppendEntriesResult(RpcMessage *msg)
{
    AppendEntriesResult res;
    bzero(&res, sizeof(res));

    assert(msg->Header.opCode == APPEND_ENTRIES_REPLY);
    assert(msg->Header.type == RETURN_VALUE);
    assert(msg->Header.status == STATUS_SUCCESS);

    std::string s = msg->data;
    std::vector<std::string> tokens = StringTokenizer::split(s, "#");
    for (std::string &token : tokens)
    {
        std::vector<std::string> toks = StringTokenizer::split(token, ":");
        if (toks[0].compare("from") == 0)
        {
            res.from = atoi(toks[1].c_str());
        }
        else if (toks[0].compare("to") == 0)
        {
            res.to = atoi(toks[1].c_str());
        }
        else if (toks[0].compare("term") == 0)
        {
            res.term = atoi(toks[1].c_str());
        }
        else if (toks[0].compare("matchIndex") == 0)
        {
            res.matchIndex = atoi(toks[1].c_str());
        }
        else if (toks[0].compare("success") == 0)
        {
            res.success = (bool) atoi(toks[1].c_str());
        }
    }

    return res;
}
