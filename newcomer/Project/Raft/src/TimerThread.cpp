#include "RaftNode.h"
#include "Random.h"
#include "font_properties.h"

/**
 * @brief 通过调用`runFollower()'来启动follower的计时器`mFollowerTimer', 每次启动都会随机设置
 *        定时器的超时时间, 这个超时时间就是"election timeout". `mFollowerTimer'超时后就会调用
 *        `followerTimerThread()', 完成状态转换 follower -> candidate
 *
 * Rules for Followers:
 * - Respond to RPCs from candidates and leaders
 *
 * - If election timeout elapses without receiving AppendEntries RPC from current leader
 *   or granting vote for candidate: convert to candidate
 */
void followerTimerThread(sigval_t sig)
{
    RaftNode *rfNode = (RaftNode *) sig.sival_ptr;

    assert(rfNode->state() == Follower);

    printf(STDOUT_L_GREEN "%s[%d] election timeout, convert to candidate\n" STDOUT_NONE,
           rfNode->stateToString().c_str(), rfNode->id());

    rfNode->setState(Candidate);
    rfNode->runCandidate();
}

/**
 * @brief follower转换为candidate的最后一步是调用`runCandidate()'. `runCandidate()'并不设置
 *        `mCandidateTimer'的超时时间, 也不会启动之, 而是直接调用`candidateTimerThread()'开始
 *        选举, 由该函数负责`mCandidateTimer'的重置. `mCandidateTimer'充当"election timer"的
 *        作用, 如果它超时就说明本次选举失败(没有选出leader), 然后`candidateTimerThread'被调用,
 *        开始新一轮选举.
 *
 * Rules for Candidates:
 * - On conversion to candidate, start election:
 *   - Increment currentTerm
 *   - Vote for self
 *   - Reset election timer
 *   - Send RequestVote RPCs to all other servers
 *
 * - If votes received from majority of servers: become leader
 *
 * - If AppendEntries RPC received from new leader: convert to follower
 *
 * - If election timeout elapses: start new election
 */
void candidateTimerThread(sigval_t sig)
{
    RaftNode *rfNode = (RaftNode *) sig.sival_ptr;

    assert(rfNode->state() == Candidate);

    printf("%s[%d] starts a new election with term %d\n",
           rfNode->stateToString().c_str(), rfNode->id(), rfNode->currentTerm());

    rfNode->increaseCurrentTerm(1); /* increment currentTerm */
    rfNode->setVotedFor(rfNode->id()); /* vote for self */

    /* reset election timer */
    rfNode->setCandidateTimeout(Random::rangeRand(ELECTION_TIMEOUT_LOWER, ELECTION_TIMEOUT_UPPER));
    rfNode->startCandidateTimer(true);

    /* send RequestVote RPCs to all other servers */
    std::vector<RequestVoteResult> results = rfNode->doRequestVote();

    int votes = 1;
    int maxTerm = rfNode->currentTerm();
    for (RequestVoteResult &res : results)
    {
        maxTerm = std::max(maxTerm, res.term);
        if (res.voteGranted) votes++;
    }

    /**
     * 这里涉及到candidate的两种状态转换:
     * - candidate -> follower (在RequestVote RPC的返回结果中发现了更高的任期)
     * - candidate -> leader (得到了大多数节点的投票)
     * 如果上述两个分支均未命中, 则本次选举失败, 直到`mCandidateTimer'超时并开始新一轮选举
     */
    if (maxTerm > rfNode->currentTerm())
    {
        rfNode->stepDown(maxTerm);
    }
    else
    {
        if (votes > rfNode->serverPortsOfAllNodes().size() / 2)
        {
            rfNode->comeToPower(votes);
        }
        else
        {
            printf("%s[%d] received %d votes, ready for a new election.\n",
                   rfNode->stateToString().c_str(), rfNode->id(), votes);
        }
    }
}

/**
 * Rules for Leaders:
 * - Upon election: send initial empty AppendEntries RPCs (heartbeat) to each server; repeat
 *   during idle periods to prevent election timeouts
 *
 * - If command received from client: append entry to local log, respond after applies to state machine
 *
 * - If last log index >= nextIndex for a follower: send AppendEntries RPC with log entries starting
 *   at nextIndex
 *   - If successful: update nextIndex and matchIndex for follower
 *   - If AppendEntries fails because of log inconsistency: decrement nextIndex and retry
 *
 * - If there exists an N such that N > commitIndex, a majority of matchIndex[i] >= N, and
 *   log[N].term == currentTerm: set commitIndex = N
 */
void leaderTimerThread(sigval_t sig)
{
    RaftNode *rfNode = (RaftNode *) sig.sival_ptr;

    assert(rfNode->state() == Leader);

    printf("%s[id=%d,currentTerm=%d,votedFor=%d,log=%s] timeout\n",
           rfNode->stateToString().c_str(), rfNode->id(), rfNode->currentTerm(), rfNode->votedFor(),
           rfNode->loglistToString().c_str());

    int maxTerm = rfNode->currentTerm();
    std::vector<AppendEntriesResult> results = rfNode->doAppendEntries();
    for (AppendEntriesResult &res : results)
    {
        maxTerm = std::max(maxTerm, res.term);
        if (res.success)
        {
            /* matchIndex[res.from] = max(matchIndex[res.from], res.matchIndex */
            /* nextIndex[res.from] = matchIndex[res.from] + 1 */
            rfNode->setMatchIndex(res.from, std::max(rfNode->matchIndex(res.from), res.matchIndex));
            rfNode->setNextIndex(res.from, res.matchIndex + 1);
        }
        else
        {
            /* 将nextIndex[res.from]减1, 但不能小于0 */
            rfNode->setNextIndex(res.from, std::max(0, rfNode->nextIndex(res.from) - 1));
        }
    }

    /**
     * 这里涉及到leader唯一一种状态转换:
     * - leader -> follower (在AppendEntries RPC的返回结果中发现了更高的任期)
     */
    if (maxTerm > rfNode->currentTerm())
    {
        rfNode->stepDown(maxTerm);
    }
}
