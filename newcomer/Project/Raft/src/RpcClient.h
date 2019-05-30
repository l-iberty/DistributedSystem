#ifndef RAFT_RPCCLIENT_H
#define RAFT_RPCCLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <assert.h>
#include <string>
#include <vector>
#include <map>
#include "RpcProtocol.h"
#include "RaftAlg.h"
#include "StringTokenizer.h"

class RpcClient
{
public:
    RpcClient() = default;

    ~RpcClient() = default;

    bool requestVote(RequestVoteResult &result, uint16_t port,
                     int term,
                     int candidateId,
                     int lastLogIndex,
                     int lastLogTerm);

    bool appendEntries(AppendEntriesResult &result, uint16_t port,
                       int term,
                       int leaderId,
                       int prevLogIndex,
                       int prevLogTerm,
                       int leaderCommit,
                       std::vector<Log> &entries);

private:
    bool connectServer(uint16_t port);

    bool sendToServer(char *buf, size_t len);

    bool recvFromServer(char *buf, size_t len);

    bool closeConnection();

    int mSockfd;
};


#endif /* RAFT_RPCCLIENT_H */
