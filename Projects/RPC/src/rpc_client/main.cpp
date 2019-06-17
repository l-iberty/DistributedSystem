#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <iostream>
#include "RpcService.h"
#include "RpcServiceProxyImpl.h"

#define MAX_THD 65535

void *threadRoutine(void *param)
{
    long n = (long) param;
    RpcServiceProxyImpl service;

    std::cout << "======= TEST " << std::to_string(n) << " =======" << std::endl;

    /* test `sayHello' */
    std::string str = "thread" + std::to_string(n);
    std::string ret = service.sayHello(str);

    /* test `introduce' */
    std::map<std::string, std::string> m = service.introduce();

    std::string s = "hello," + str;
    if (!strcmp(ret.c_str(), s.c_str()) &&
        !strcmp(m["name"].c_str(), "Bob") &&
        !strcmp(m["email"].c_str(), "Bob@xxx.com"))
    {
        std::cout << "retval of sayHello>> " << ret.c_str() << std::endl;
        std::cout << "retval of introduce>> name:" << m["name"].c_str() << ", email:" << m["email"] << std::endl;
        std::cout << "======= TEST " << n << " passed. =======" << std::endl;
    }
    else
    {
        std::cerr << "======= TEST " << n << " failed. =======" << std::endl;
        assert(0);
    }

    return NULL;
}

int main()
{
    pthread_t tid[MAX_THD];
    void *retval;

    /* 创建大量线程, 测试并发性 */
    for (int i = 0; i < MAX_THD; i++)
    {
        if (pthread_create(&tid[i], NULL, threadRoutine, (void *) i) != 0)
        {
            perror("pthread_create");
        }
    }

    for (int i = 0; i < MAX_THD; i++)
    {
        pthread_join(tid[i], &retval);
    }

    return 0;
}
