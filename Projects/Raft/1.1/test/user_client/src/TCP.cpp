#include "TCP.h"

bool TCP::recv(int sockfd, void* buf, size_t len)
{
    bzero(buf, len);
    return ::recv(sockfd, buf, len, 0) > 0;
}

bool TCP::send(int sockfd, void* buf, size_t len)
{
    return ::send(sockfd, buf, len, 0) > 0;
}

bool TCP::close(int sockfd)
{
    return ::close(sockfd) != -1;
}
