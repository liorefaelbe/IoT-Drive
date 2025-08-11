#ifndef __ILRD_ASYNC_INJECTION_HPP__
#define __ILRD_ASYNC_INJECTION_HPP__

#include <functional>       // std::function
#include <chrono>           // std::chrono::milliseconds
#include <memory>           // std::shard_ptr

#include "scheduler.hpp"    // Scheduler

/**
 * @class   AsyncInjection
 * @brief   A utility class for executing recurring tasks at specific time intervals.
 * 
 * Features:
 *   - Executes a provided function at specified time intervals
 *   - Automatically re-schedules function if it returns true
 *   - Self-destructs when function returns false
 *   - Uses Scheduler for task management
 * 
 * @author  Lior Reafael Berkovits
 * @date    05/14/2025
 */

/************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
class AsyncInjection
{
public:
    AsyncInjection(std::function<bool()>, std::chrono::milliseconds time_interval_ms);

    void PerformAction();

private:
    ~AsyncInjection();

    std::function<bool()> m_func;
    std::chrono::milliseconds m_time_interval_ms;
    std::shared_ptr<Scheduler::ISchedulerTask> m_task;

    class AsyncInjectionTask : public Scheduler::ISchedulerTask
    {
    public:
        AsyncInjectionTask(AsyncInjection& async_injection);

        void Execute();

    private:
        AsyncInjection& m_async_injection;
    }; // AsyncInjectionTask
}; // AsyncInjection
} // namespace ilrd_166_7

#endif // __ILRD_SCHEDULER_HPP__