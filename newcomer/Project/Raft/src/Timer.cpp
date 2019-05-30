#include <stdlib.h>
#include "Timer.h"
#include "Random.h"
#include "font_properties.h"

void Timer::setTimeoutThread(timer_thread_routine timerThreadRoutine, void *param)
{
    struct sigevent sev;
    bzero(&sev, sizeof(sev));
    sev.sigev_notify = SIGEV_THREAD;
    sev._sigev_un._sigev_thread._function = timerThreadRoutine;
    sev.sigev_value.sival_ptr = param;

    if (timer_create(CLOCK_REALTIME, &sev, &mTimerId) < 0)
    {
        perror(STDOUT_RED "#ERROR# timer_create" STDOUT_NONE);
        exit(EXIT_FAILURE);
    }
}

void Timer::setTimeout(int timeout)
{
    mTimeout = timeout;
}

bool Timer::start(bool auto_restart)
{
    bzero(&mTimerSpec, sizeof(mTimerSpec));
    mTimerSpec.it_value.tv_sec = mTimeout / 1000;
    mTimerSpec.it_value.tv_nsec = mTimeout % 1000000;

    if (auto_restart)
    {
        /* 超时后it_value被重置为it_interval, 从而实现timer的重启 */
        mTimerSpec.it_interval.tv_sec = mTimerSpec.it_value.tv_sec;
        mTimerSpec.it_interval.tv_nsec = mTimerSpec.it_value.tv_nsec;
    }

    if (timer_settime(mTimerId, 0, &mTimerSpec, NULL) < 0)
    {
        perror(STDOUT_RED "#ERROR# timer_settime" STDOUT_NONE);
        return false;
    }
    return true;
}

bool Timer::cancel()
{
    bzero(&mTimerSpec, sizeof(mTimerSpec));
    if (timer_settime(mTimerId, 0, &mTimerSpec, NULL) < 0)
    {
        perror(STDOUT_RED "#ERROR# timer_settime" STDOUT_NONE);
        return false;
    }
    return true;
}

bool Timer::isCanceled()
{
    return (mTimerSpec.it_value.tv_sec == 0 && mTimerSpec.it_value.tv_nsec == 0 &&
            mTimerSpec.it_interval.tv_sec == 0 && mTimerSpec.it_interval.tv_nsec == 0);
}

