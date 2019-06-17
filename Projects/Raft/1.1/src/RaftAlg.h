#ifndef RAFT_RAFTALG_H
#define RAFT_RAFTALG_H

#include <vector>

#define INVALID_ID             -1
#define INVALID_INDEX          -1
#define INVALID_TERM           0

#define ELECTION_TIMEOUT_LOWER 2000
#define ELECTION_TIMEOUT_UPPER 4000
#define HEARTBEAT_TIMEOUT      1000 /* leader的心跳周期要短于election timeout, 否则无法选出稳定的leader */
#define BATCH_SIZE             5 /* AppendEntries RPC复制日志的时候每次复制多少项? */

typedef enum
{
    Follower, Candidate, Leader
} NodeState;

struct Log
{
    int id;
    int term;

    Log() : id(INVALID_ID), term(INVALID_TERM) {}
};

struct RequestVoteArg
{
    int from, to;

    int term; /* candidate's term */

    int candidateId; /* candidate requesting vote */

    int lastLogIndex; /* index of candidate's last log entry */

    int lastLogTerm; /* term of candidate's last log entry */

    RequestVoteArg() : from(INVALID_ID),
                       to(INVALID_ID),
                       term(INVALID_TERM),
                       candidateId(INVALID_ID),
                       lastLogIndex(INVALID_TERM),
                       lastLogTerm(INVALID_TERM) {}
};

struct RequestVoteResult
{
    int from, to;

    int term; /* currentTerm, for candidate to update itself */

    bool voteGranted; /* true means candidate received vote */

    RequestVoteResult() : from(INVALID_ID),
                          to(INVALID_ID),
                          term(INVALID_TERM),
                          voteGranted(false) {}
};

struct AppendEntriesArg
{
    int from, to;

    int term; /* leader's term */

    int leaderId; /* so follower can redirect clients */

    int prevLogIndex; /* index of log entry immediately preceding new ones */

    int prevLogTerm; /* term of prevLogIndex entry */

    int leaderCommit; /* leader's commitIndex */

    std::vector<Log> entries; /* log entries to store (empty for heartbeat;
                                 may send more than one for efficiency */

    AppendEntriesArg() : from(INVALID_ID),
                         to(INVALID_ID),
                         term(INVALID_TERM),
                         leaderId(INVALID_ID),
                         prevLogIndex(INVALID_INDEX),
                         prevLogTerm(INVALID_TERM),
                         leaderCommit(INVALID_INDEX),
                         entries({}) {}
};

struct AppendEntriesResult
{
    int from, to;

    int term; /* currentTerm, for leader to update itself */

    int matchIndex;

    bool success; /* true if follower contained entry matching prevLogIndex and prevLogTerm */

    AppendEntriesResult() : from(INVALID_ID),
                            to(INVALID_ID),
                            term(INVALID_TERM),
                            matchIndex(INVALID_INDEX),
                            success(false) {}
};

#endif /* RAFT_RAFTALG_H */
