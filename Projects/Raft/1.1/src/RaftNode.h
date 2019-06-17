#ifndef RAFT_RAFTNODE_H
#define RAFT_RAFTNODE_H

#include <stdint.h>
#include <pthread.h>
#include <vector>
#include <string>
#include "RaftAlg.h"
#include "RpcServer.h"
#include "RpcClient.h"
#include "RpcProtocol.h"
#include "RpcInvoker.h"
#include "TCPServer.h"
#include "Timer.h"

void* serverThread(void* param);

void* clientThread(void* param);

void* userThread(void* param);

void followerTimerThread(sigval_t sig);

void candidateTimerThread(sigval_t sig);

void leaderTimerThread(sigval_t sig);


class RaftNode
{
public:

    RaftNode(int id, std::vector<uint16_t> userPorts, std::vector<uint16_t> rpcServerPorts);

    ~RaftNode() = default;

    int id();

    NodeState state();

    void setState(NodeState state);

    std::string stateToString();

    uint16_t rpcServerPort();

    std::vector<uint16_t> serverPortsOfAllNodes();

    int currentTerm();

    void increaseCurrentTerm(int increment);

    int votedFor();

    void setVotedFor(int id);

    int logTerm(int index);

    std::vector<Log> sliceLoglist(int start, int end);

    std::string loglistToString();

    std::vector<Log> stringToLoglist(std::string& s);

    int nextIndex(int index);

    void setNextIndex(int index, int value);

    int matchIndex(int index);

    void setMatchIndex(int index, int value);

    void run();

    void runRpcServer();

    void runRpcClient();

    void runUserRequestHandler();

    void runFollower();

    void runCandidate();

    void runLeader();

    void stepDown(int term);

    void comeToPower(int votes);

    void setFollowerTimeout(int timeout);

    void setCandidateTimeout(int timeout);

    void setLeaderTimeout(int timeout);

    void startFollowerTimer(bool auto_restart);

    void startCandidateTimer(bool auto_restart);

    void startLeaderTimer(bool auto_restart);

    void cancelFollowerTimer();

    void cancelCandidateTimer();

    void cancelLeaderTimer();

    std::vector<RequestVoteResult> doRequestVote();

    RequestVoteResult onRequestVoteReply(int term,
                                         int candidateId,
                                         int lastLogIndex,
                                         int lastLogTerm);

    std::vector<AppendEntriesResult> doAppendEntries();

    AppendEntriesResult onAppendEntriesReply(int term,
                                             int leaderId,
                                             int prevLogIndex,
                                             int prevLogTerm,
                                             int leaderCommit,
                                             std::vector<Log>& entries);

private:
    pthread_t createRpcServerThread();

    pthread_t createRpcClientThread();

    pthread_t createUserThread();

    bool block(const char* filename);

    void toggle(const char* filename);

    void appendLog(int n);

    int mId; /* 节点ID, 对应的server进程的监听端口为 mRpcServerPorts[mId] */

    NodeState mState; /* 节点状态: Follower, Candidate, Leader */

    std::vector<uint16_t> mRpcServerPorts; /* 集群中所有节点的rpc server线程的监听端口 */

    std::vector<uint16_t> mUserPorts; /* 集群中所有节点用于相应外界用户请求的监听端口 */

    int mCurrentTerm; /* latest term server has seen */

    int mVotedFor; /* candidateId that received vote in current term (or null if none) */

    std::vector<Log> mLoglist; /* log entries */

    pthread_mutex_t mLoglistMutex; /* 并发线程互斥修改mLoglist */

    int mCommitIndex; /* unused */

    int mLastApplied; /* unused */

    std::vector<int> mNextIndex; /* for each server, index of log entry to send to that server */

    std::vector<int> mMatchIndex; /* for each server, index of highest log entry known to be replicated on server */

    RpcClient mRpcClient;

    RpcServer mRpcServer;

    Timer mFollowerTimer, mCandidateTimer, mLeaderTimer;

    int mCurrentLeaderId;

    TCPClient mTCPClient;

    TCPServer mTCPServer;
};


#endif /* RAFT_RAFTNODE_H */
