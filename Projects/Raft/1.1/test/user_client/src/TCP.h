#ifndef RAFT_TCP_H
#define RAFT_TCP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>


class TCP
{
public:
    TCP() = default;

    virtual ~TCP() = default;

    bool send(int sockfd, void* buf, size_t len);

    bool recv(int sockfd, void* buf, size_t len);

    bool close(int sockfd);
};


#endif /* RAFT_TCP_H */
