#ifndef __ILRD_TP_TASK_HPP__
#define __ILRD_TP_TASK_HPP__

#include "framework.hpp"    // IInputProxy
#include "thread_pool.hpp"  // ITask

/**
 * @class   TPTask
 * @brief   A concrete implementation of the ThreadPool's ITask interface
 *          that wraps tasks with their associated key arguments, allowing
 *          them to be executed by the thread pool
 * 
 * Features:
 *   - Encapsulates the task's key arguments for execution
 *   - Implements the ThreadPool's ITask interface for compatibility
 *   - Handles execution of tasks in the thread pool environment
 *
 * @author  Lior Reafael Berkovits
 * @date    20/05/2025
 */

/************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
class TPTask : public ThreadPool::ITask
{
public:
    TPTask(std::shared_ptr<IKeyArgs> args);
    ~TPTask();
    void Run() override;

private:
    std::shared_ptr<IKeyArgs> m_keyArgs;
}; // TPTask
} // namespace ilrd_166_7

#endif // __ILRD_TP_TASK_HPP__