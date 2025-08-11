#include <sys/types.h>      // timer_t
#include <signal.h>         // sigevent
#include <iostream>         // std::cerr

#include "scheduler.hpp"    // Scheduler

using namespace std;

namespace ilrd_166_7
{

/**************************** Implementations *********************************/
/******************************* Scheduler ************************************/

static void ThreadFunc(union sigval timer_data)
{
	Scheduler::ISchedulerTask* task = reinterpret_cast<Scheduler::ISchedulerTask*>(timer_data.sival_ptr);
	task->Execute();
}

void Scheduler::AddTask(std::shared_ptr<ISchedulerTask> task, chrono::milliseconds time_ms)
{
    auto ptask = task.get();

	timer_t timer_id = 0;
	struct sigevent sevent{};
	struct itimerspec duration{};

    duration.it_value.tv_sec = time_ms.count() / 1000;
    duration.it_value.tv_nsec = (time_ms.count() % 1000) * 1000000;
	
	sevent.sigev_notify = SIGEV_THREAD;
	sevent.sigev_notify_function = ThreadFunc;
    sevent.sigev_value.sival_ptr = ptask;
	
	int timer = timer_create(CLOCK_REALTIME, &sevent, &timer_id);
	if (timer == -1)
    {
        cerr << "AddTask: timer_create fail.\n";
    }

	timer = timer_settime(timer_id, 0, &duration, NULL);
    if (timer == -1)
    {
        cerr << "AddTask: timer_settime fail.\n";
    }
}
}
