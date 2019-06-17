#ifndef RAFT_TIMER_H
#define RAFT_TIMER_H

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>


typedef void (*timer_thread_routine)(sigval_t);

class Timer
{
public:
    Timer() = default;

    ~Timer() = default;

    void setTimeoutThread(timer_thread_routine timerThreadRoutine, void *param);

    void setTimeout(int timeout);

    bool start(bool auto_restart);

    bool cancel();

    bool isCanceled();

private:
    int mTimeout; /* 超时时间, 单位:ms */
    timer_t mTimerId;
    struct itimerspec mTimerSpec;
};


#endif /* RAFT_TIMER_H */
