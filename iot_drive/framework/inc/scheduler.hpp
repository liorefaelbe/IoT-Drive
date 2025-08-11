#ifndef __ILRD_SCHEDULER_HPP__
#define __ILRD_SCHEDULER_HPP__

#include <memory>   // std::shared_ptr
#include <chrono>   // std::chrono::milliseconds

/**
 * @class   Scheduler
 * @brief   A task scheduling system that executes tasks after specified time intervals.
 * 
 * Features:
 *   - Schedule tasks to be executed after a specified delay
 *   - Tasks are executed in separate threads
 *   - Uses POSIX timers for scheduling
 *   - Thread-safe task execution
 * 
 * @author  Lior Reafael Berkovits
 * @date    05/14/2025
 */

/************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
class Scheduler
{
public:
    class ISchedulerTask
    {
    public:
        virtual void Execute() = 0;
    };
    
    void AddTask(std::shared_ptr<ISchedulerTask> task, std::chrono::milliseconds time_ms);
}; // Scheduler
} // namespace ilrd_166_7

#endif // __ILRD_SCHEDULER_HPP__