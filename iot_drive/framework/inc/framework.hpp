#ifndef __ILRD_FRAMEWORK_HPP__
#define __ILRD_FRAMEWORK_HPP__

#include <string>               // std::string
#include <list>                 // std::list
#include <functional>           // std::function
#include <memory>               // std::shared_ptr

#include "reactor.hpp"          // fd_mode
#include "dir_monitor.hpp"      // DirMonitor
#include "dllloader.hpp"        // DllLoader
#include "linux_listener.hpp"    // LinuxListener

/**
 * @class   Framework
 * @brief   A comprehensive application framework that manages event handling,
 *          plugin loading, and task execution through a reactor pattern
 * 
 * Features:
 *   - Manages input proxies for handling various file descriptor events
 *   - Supports dynamic plugin loading from specified path
 *   - Coordinates command creation and execution through a unified interface
 *   - Provides asynchronous task execution capabilities
 *
 * @author  Lior Reafael Berkovits
 * @date    20/05/2025
 */

/************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
using async_injection_functor = std::function<bool()>;

class IKeyArgs
{
public:
    virtual int GetKey() = 0;
}; // IKeyArgs

class IInputProxy
{
public:
    virtual ~IInputProxy() = 0;
    virtual std::shared_ptr<IKeyArgs> GetKeyArgs(Reactor::fd_mode) = 0;
}; // IInputProxy

class ICommand
{
public:
    virtual async_injection_functor Run(std::shared_ptr<IKeyArgs>) = 0;
}; // ICommand

class Framework
{
public:
    using proxy_list_t = std::list<std::pair<Reactor::fd_mode, IInputProxy*>>;
    using createFunc_list_t = std::list<std::pair<int, std::function<std::shared_ptr<ICommand>()>>>;

    Framework(const std::string& pluginsPathName, proxy_list_t pList , createFunc_list_t clist);
    ~Framework();
    void Run(); // Blocking

private:
    Reactor m_reactor;
    LinuxListener m_listener;
    DirMonitor m_monitor;
    DllLoader m_loader;
    Callback<std::string, DllLoader> m_loader_cb;
}; // Framework
} // namespace ilrd_166_7

#endif // __ILRD_FRAMEWORK_HPP__