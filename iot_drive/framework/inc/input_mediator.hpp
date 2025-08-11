#ifndef __ILRD_INPUT_MEDIATOR_HPP__
#define __ILRD_INPUT_MEDIATOR_HPP__

#include "framework.hpp"        // IInputProxy
#include "thread_pool.hpp"      // ThreadPool
#include "handleton.hpp"        // GetInstance
#include "tp_task.hpp"          // TPTask
#include "input_mediator.hpp"   // InputMediator

/**
 * @class   InputMediator
 * @brief   Mediates between the reactor's file descriptor notifications
 *          and the input proxy system, coordinating the handling of I/O events
 * 
 * Features:
 *   - Connects the reactor's event notifications to the input proxy system
 *   - Handles file descriptor events with appropriate priorities
 *   - Provides callback functionality for the reactor's event dispatching
 *
 * @author  Lior Reafael Berkovits
 * @date    20/05/2025
 */

/************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
struct InputMediator
{
public:
    InputMediator(IInputProxy* inputProxy, Reactor::fd_mode fd, ThreadPool::PRIORITY priority = ThreadPool::PRIORITY::MEDIUM) :
    m_inputProxy(inputProxy), m_fd(fd), m_priority(priority) { /* empty */ }
    
    void operator()(int fd, Reactor::MODE mode)
    {
        (void)fd;
        (void)mode;

        std::shared_ptr<IKeyArgs> key_args = m_inputProxy->GetKeyArgs(m_fd);

        if (key_args.get())
        {
            Handleton<ThreadPool>::GetInstance()->AddTask(std::make_shared<TPTask>(key_args), m_priority);
        }  
    }
private:
    IInputProxy* m_inputProxy;
    Reactor::fd_mode m_fd; 
    ThreadPool::PRIORITY m_priority;
}; // InputMediator
} // namespace ilrd_166_7

#endif // __ILRD_INPUT_MEDIATOR_HPP__