#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "RaftNode.h"
#include "Random.h"
#include "font_properties.h"

void *serverThread(void *param)
{
    RaftNode *rfNode = (RaftNode *) param;
    rfNode->runRpcServer();
    return NULL;
}

void *clientThread(void *param)
{
    RaftNode *rfNode = (RaftNode *) param;
    rfNode->runRpcClient();
    return NULL;
}


//////////////////////////////// public ////////////////////////////////

RaftNode::RaftNode(int id, std::vector<uint16_t> serverPortsOfAllNodes)
        : mId(id),
          mState(Follower),
          mServerPortsOfAllNodes(serverPortsOfAllNodes),
          mCurrentTerm(INVALID_TERM),
          mVotedFor(INVALID_ID),
          mCommitIndex(INVALID_INDEX),
          mLastApplied(INVALID_INDEX),
          mNextIndex(serverPortsOfAllNodes.size(), 0),
          mMatchIndex(serverPortsOfAllNodes.size(), INVALID_INDEX)
{
    mFollowerTimer.setTimeoutThread(followerTimerThread, this);
    mCandidateTimer.setTimeoutThread(candidateTimerThread, this);
    mLeaderTimer.setTimeoutThread(leaderTimerThread, this);
}

int RaftNode::id()
{
    return mId;
}

NodeState RaftNode::state()
{
    return mState;
}

void RaftNode::setState(NodeState state)
{
    mState = state;
}

std::string RaftNode::stateToString()
{
    switch (mState)
    {
        case Follower:
            return "Follower";
        case Candidate:
            return "Candidate";
        case Leader:
            return "Leader";
    }
    return "???";
}

uint16_t RaftNode::serverPort()
{
    return mServerPortsOfAllNodes[mId];
}

std::vector<uint16_t> RaftNode::serverPortsOfAllNodes()
{
    return mServerPortsOfAllNodes;
}

int RaftNode::currentTerm()
{
    return mCurrentTerm;
}

void RaftNode::increaseCurrentTerm(int increment)
{
    mCurrentTerm += increment;
}

int RaftNode::votedFor()
{
    return mVotedFor;
}

void RaftNode::setVotedFor(int id)
{
    mVotedFor = id;
}

int RaftNode::logTerm(int index)
{
    if (mLoglist.empty() || index >= mLoglist.size())
    {
        return INVALID_TERM;
    }
    return mLoglist[index].term;
}

std::vector<Log> RaftNode::sliceLoglist(int start, int end)
{
    std::vector<Log> slice;
    end = std::min(end, (int) mLoglist.size() - 1);

    if (start >= 0 && !mLoglist.empty())
    {
        for (int i = start; i <= end; i++)
        {
            slice.push_back(mLoglist[i]);
        }
    }
    return slice;
}

std::string RaftNode::loglistToString()
{
    std::string s = "[";

    int i;
    if (!mLoglist.empty())
    {
        for (i = 0; i < mLoglist.size() - 1; i++)
        {
            s += std::to_string(mLoglist[i].term) + ",";
        }
        s += std::to_string(mLoglist[i].term);
    }
    s += "]";

    return s;
}

/**
 * @brief 表示Log数组的字符串只包含`term'一个成员变量, 形如"[1,1,1,2,2,3]"
 */
std::vector<Log> RaftNode::stringToLoglist(std::string &s)
{
    std::vector<Log> logs;

    std::string str = s.substr(1, s.length() - 2); /* 截取[]内的子串 */
    std::vector<std::string> tokens = StringTokenizer::split(str, ",");
    for (int i = 0; i < tokens.size(); i++)
    {
        Log log;
        log.id = i, log.term = atoi(tokens[i].c_str());
        logs.push_back(log);
    }

    return logs;
}

std::vector<Log> RaftNode::readLoglist(const char *filename)
{
    FILE *fp = NULL;
    char buf[BUFSIZ];
    bzero(buf, BUFSIZ);

    if ((fp = fopen(filename, "r")) == NULL)
    {
        perror(STDOUT_L_RED "fopen" STDOUT_NONE);
        return {};
    }

    fgets(buf, BUFSIZ, fp);
    fclose(fp);

    std::string s = buf;
    return stringToLoglist(s);
}

int RaftNode::nextIndex(int index)
{
    if (index < 0 || index >= mNextIndex.size())
    {
        return INVALID_INDEX;
    }
    return mNextIndex[index];
}

void RaftNode::setNextIndex(int index, int value)
{
    if (index >= 0 && index < mNextIndex.size())
    {
        mNextIndex[index] = value;
    }
}

int RaftNode::matchIndex(int index)
{
    if (index < 0 || index >= mMatchIndex.size())
    {
        return INVALID_INDEX;
    }
    return mMatchIndex[index];
}

void RaftNode::setMatchIndex(int index, int value)
{
    if (index >= 0 && index < mMatchIndex.size())
    {
        mMatchIndex[index] = value;
    }
}

void RaftNode::run()
{
    void *retval;
    pthread_t server = createRpcServerThread();
    pthread_t client = createRpcClientThread();

    pthread_join(server, &retval);
    pthread_join(client, &retval);
}

void RaftNode::runRpcServer()
{
    mRpcServer.run(this);
}

void RaftNode::runRpcClient()
{
    switch (mState)
    {
        case Follower:
            runFollower();
            break;
        case Candidate:
            runCandidate();
            break;
        case Leader:
            runLeader();
            break;
    }
}

void RaftNode::runFollower()
{
    assert(mState == Follower);

    cancelCandidateTimer();
    cancelLeaderTimer();

    setFollowerTimeout(Random::rangeRand(ELECTION_TIMEOUT_LOWER, ELECTION_TIMEOUT_UPPER));
    startFollowerTimer(true);
}

void RaftNode::runCandidate()
{
    assert(mState == Candidate);

    cancelFollowerTimer();
    cancelLeaderTimer();

    sigval_t sig;
    sig.sival_ptr = this;
    candidateTimerThread(sig);
}

void RaftNode::runLeader()
{
    assert(mState == Leader);

    cancelFollowerTimer();
    cancelCandidateTimer();

    setLeaderTimeout(HEARTBEAT_TIMEOUT);
    startLeaderTimer(true);

    sigval_t sig;
    sig.sival_ptr = this;
    leaderTimerThread(sig);
}

void RaftNode::stepDown(int term)
{
    printf(STDOUT_L_GREEN "%s[id=%d,currentTerm=%d] discover higher term %d, convert to follower\n" STDOUT_NONE,
           stateToString().c_str(), mId, mCurrentTerm, term);

    mCurrentTerm = term;
    mState = Follower;
    mVotedFor = INVALID_ID;
    runFollower();
}

void RaftNode::comeToPower(int votes)
{
    assert(mState == Candidate); /* only candidate can come to power and become leader */

    printf(STDOUT_L_GREEN "%s[%d] received %d votes, becomes leader with term %d.\n" STDOUT_NONE,
           stateToString().c_str(), mId, votes, mCurrentTerm);

    appendLog("log.txt"); /* 附加新的日志 */
    setState(Leader);
    runLeader();
}

void RaftNode::setFollowerTimeout(int timeout)
{
    mFollowerTimer.setTimeout(timeout);
}

void RaftNode::setCandidateTimeout(int timeout)
{
    mCandidateTimer.setTimeout(timeout);
}

void RaftNode::setLeaderTimeout(int timeout)
{
    mLeaderTimer.setTimeout(timeout);
}

void RaftNode::startFollowerTimer(bool auto_restart)
{
    mFollowerTimer.start(auto_restart);
}

void RaftNode::startCandidateTimer(bool auto_restart)
{
    mCandidateTimer.start(auto_restart);
}

void RaftNode::startLeaderTimer(bool auto_restart)
{
    mLeaderTimer.start(auto_restart);
}

void RaftNode::cancelFollowerTimer()
{
    mFollowerTimer.cancel();
}

void RaftNode::cancelCandidateTimer()
{
    mCandidateTimer.cancel();
}

void RaftNode::cancelLeaderTimer()
{
    mLeaderTimer.cancel();
}

std::vector<RequestVoteResult> RaftNode::doRequestVote()
{
    std::vector<RequestVoteResult> results;

    for (int id = 0; id < mServerPortsOfAllNodes.size(); id++)
    {
        if (id != mId)
        {
            RequestVoteResult res;
            uint16_t port = mServerPortsOfAllNodes[id];
            int lastLogIndex = mLoglist.size() - 1;
            int lastLogTerm = logTerm(lastLogIndex);

            if (mRpcClient.requestVote(res, port, mCurrentTerm, mId, lastLogIndex, lastLogTerm))
            {
                results.push_back(res);
            }
            else
            {
                printf(STDOUT_RED "#ERROR# something wrong with Node[%d](%d)\n" STDOUT_NONE, id, port);
            }
        }
    }

    return results;
}

RequestVoteResult RaftNode::onRequestVoteReply(
        int term, int candidateId, int lastLogIndex, int lastLogTerm)
{
    RequestVoteResult res;
    bool granted = false;

    if (term > mCurrentTerm) stepDown(term);

    /**
     * 任期(term)充当逻辑时钟的作用, 如果 term < currentTerm 则说明发起投票请求的candidate的
     * 信息比自己过时, 所以不应该投票给他; 而如果 term > currentTerm, 说明发现了更高的任期, 应
     * 转换到follower. 总之, 只有满足 term == currentTerm 才有可能投票给他
     */
    if (term == mCurrentTerm &&
        (mVotedFor == INVALID_ID || mVotedFor == candidateId) &&
        lastLogIndex >= mLoglist.size() - 1 && lastLogTerm >= logTerm(mLoglist.size() - 1))
    {
        /**
         * if votedFor is null or candidateId (该条件保证如果有多个candidate, 任意一个
         * candidate都不会得到其他candidate的投票, 因为当follower成为candidate并开始选举
         * 时, 它要先给自己投票), and candidate's log is at least up-to-date as receiver's
         * log, grant vote
         */
        mVotedFor = candidateId;
        granted = true;
    }

    res.from = mId;
    res.to = candidateId;
    res.term = mCurrentTerm;
    res.voteGranted = granted;
    return res;
}

std::vector<AppendEntriesResult> RaftNode::doAppendEntries()
{
    std::vector<AppendEntriesResult> results;

    for (int id = 0; id < mServerPortsOfAllNodes.size(); id++)
    {
        if (id != mId)
        {
            /**
             * 读取文件"block[id]"判断是否要给节点id发送AppendEntries, 如果不发送, 则节点id将因
             * 超时而转换成candidate并开始新一轮选举. 此内容用于用户交互, 与Raft算法逻辑无关. 用户
             * 可以在Raft系统运行时从外部修改相应的block文件, 以此模拟节点状态的改变.
             *
             * 例如:
             * 系统初始运行时, 由启动脚本创建文件block0、block1、block2..., 并向其中写入字符'0',
             * 表示leader发送的AppendEntries可以到达那个节点, 它得以维持follower状态. 如果用户
             * 想模拟节点[1]超时(无法收到leader的AppendEntries), 那么只需向文件"block1"写入字符
             * '1', 就可以将节点[1]与leader的通信阻断一次, 然后`toggle()'会恢复文件"block1"的内
             * 容为'0', 于是leader下一次与节点[1]的通信就会恢复正常. 如果因为本次的通信阻断导致节
             * 点[1]在election timeout时间内没有收到leader的AppendEntries, 它就会认为leader已
             * 经崩溃, 就会转换为candidate开始选举, 那么原本的leader在收到RequestVote时发现了更
             * 高的任期, 它就会转换成follower, 节点[1]通常就能赢得选举成为新的leader.
             */
            std::string filename = "block" + std::to_string(id);
            if (block(filename.c_str()))
            {
                toggle(filename.c_str());
                continue;
            }

            AppendEntriesResult res;
            uint16_t port = mServerPortsOfAllNodes[id];
            int prevLogIndex = mNextIndex[id] - 1;
            int prevLogTerm = logTerm(prevLogIndex);
            std::vector<Log> entries = sliceLoglist(mNextIndex[id], mNextIndex[id] + MAX_ENTRIES - 1);
            /* 如果entries为空, 那么发送的就是heartbeat */

            if (mRpcClient.appendEntries(res, port, mCurrentTerm, mId, prevLogIndex, prevLogTerm, mCommitIndex, entries))
            {
                results.push_back(res);
            }
            else
            {
                printf(STDOUT_RED "#ERROR# something wrong with Node[%d](%d)\n" STDOUT_NONE, id, port);
            }
        }
    }

    return results;
}

/**
 * @brief 如果AppendEntries RPC的接收者是follower, 那么follower的计时器会被重置;
 *        如果接收者是candidate, 那么计时器继续运行; 接收者不可能是leader, 除非满
 *        足 term != currentTerm, 因为同一任期内不可能存在多个leader. 目前仅实现
 *        了日志复制功能.
 */
AppendEntriesResult RaftNode::onAppendEntriesReply(
        int term, int leaderId, int prevLogIndex, int prevLogTerm, int leaderCommit, std::vector<Log> &entries)
{
    AppendEntriesResult res;
    bool success = false;
    int matchIndex = INVALID_INDEX;

    if (term == mCurrentTerm) assert(mState != Leader);

    if (term > mCurrentTerm) stepDown(term);

    if (term == mCurrentTerm)
    {
        mState = Follower;
        runFollower();

        if (prevLogIndex == INVALID_TERM || logTerm(prevLogIndex) == prevLogTerm)
        {
            /* Reply true if follower contains entry matching prevLogIndex and prevLogTerm */
            success = true;
        }

        /**
         * If an existing entry conflicts with a new one(same index but different term),
         * delete the existing entry and all that follow it
         *
         * `mLoglist[prevLogIndex]'后面的日志项如果与`entries'中的任意一项有冲突, 就删除从冲突位置
         * 开始及其后面的所有日志项.
         */
        int index = prevLogIndex;
        for (int i = 0; i < entries.size(); i++)
        {
            index++;
            if (logTerm(index) != entries[i].term)
            {
                while (mLoglist.size() > index)
                {
                    mLoglist.pop_back();
                }
                mLoglist.push_back(entries[i]);
            }
        }
        matchIndex = index;
    }

    printf(STDOUT_GREEN "%s[id=%d,currentTerm=%d,votedFor=%d,log=%s] received AppendEntriesRPC from Leader[id=%d,term=%d]\n" STDOUT_NONE,
           stateToString().c_str(), mId, mCurrentTerm, mVotedFor, loglistToString().c_str(), leaderId, term);

    res.from = mId;
    res.to = leaderId;
    res.term = mCurrentTerm;
    res.matchIndex = matchIndex;
    res.success = success;
    return res;
}

//////////////////////////////// private ////////////////////////////////

pthread_t RaftNode::createRpcServerThread()
{
    pthread_t tid;
    if (pthread_create(&tid, NULL, serverThread, this) != 0)
    {
        perror(STDOUT_RED "#ERROR# pthread_create" STDOUT_NONE);
        exit(EXIT_FAILURE);
    }

    return tid;
}

pthread_t RaftNode::createRpcClientThread()
{
    pthread_t tid;
    if (pthread_create(&tid, NULL, clientThread, this) != 0)
    {
        perror(STDOUT_RED "#ERROR# pthread_create" STDOUT_NONE);
        exit(EXIT_FAILURE);
    }

    return tid;
}

bool RaftNode::block(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    int c;
    if (fp != NULL)
    {
        c = fgetc(fp);
        fclose(fp);
        return c == '1';
    }
    return false;
}

void RaftNode::toggle(const char *filename)
{
    FILE *fp = fopen(filename, "r+");
    int c;
    if (fp != NULL)
    {
        c = fgetc(fp);
        fseek(fp, 0, SEEK_SET);
        fputc((c == '1') ? '0' : '1', fp);
        fclose(fp);
    }
}

void RaftNode::appendLog(std::vector<Log> loglist)
{
    for (Log e : loglist)
    {
        e.term += mCurrentTerm;
        mLoglist.push_back(e);
    }
}

void RaftNode::appendLog(const char *filename)
{
    appendLog(readLoglist(filename));
}

