#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <iostream>
#include "RpcClient.h"

#define MAX_THD 65535

void *threadRoutine(void *param)
{
    RpcClient client;
    RpcService *service = client.getService(inet_addr("127.0.0.1"), htons(5000));

    long n = (long) param;
    std::string str = "thread" + std::to_string(n);
    std::string ret = service->sayHello(str);
    std::cout << ret.c_str() << std::endl;

    std::string s = "hello," + str;
    if (!strcmp(ret.c_str(), s.c_str()))
    {
        std::cout << "TEST " << n << " passed." << std::endl;
    }
    else
    {
        std::cout << "TEST " << n << " failed." << std::endl;
        assert(0);
    }

    delete service;
    return NULL;
}

int main()
{
    pthread_t tid;
    void *retval;

    /* 创建大量线程, 测试并发性 */
    for (int i = 0; i < MAX_THD; i++)
    {
        int ret = pthread_create(&tid, NULL, threadRoutine, (void *) i);
        if (ret < 0)
        {
            perror("pthread_create");
        }
        pthread_join(tid, &retval);
    }

    return 0;
}