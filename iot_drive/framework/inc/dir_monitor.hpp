#ifndef __ILRD_DIR_MONITOR_HPP__
#define __ILRD_DIR_MONITOR_HPP__

#include <string>               // std::string
#include <thread>               // std::thread
#include <mutex>                // std::mutex
#include <atomic>               // std::atomic_bool
#include <condition_variable>   // std::condition_variable

#include "dispatcher.hpp"       // Dispatcher
#include "dllloader.hpp"        // DllLoader

/**
 * @class  DirMonitor
 * @brief  A directory monitoring system that uses the Dispatcher 
 *         pattern to notify about new library files added to a directory.
 *         The monitor watches a directory and notifies observers when new files appear
 * 
 * Features:
 *   - Non-blocking directory monitoring in a separate thread
 *   - Event-based notification using the Dispatcher pattern when new files are detected
 *   - Dynamic loading of shared libraries (.so/.dll files) at runtime
 *   - Thread-safe subscription and unsubscription mechanisms
 *   - Automatic distribution of file events to all registered callbacks
 * 
 * @author Lior Reafael Berkovits
 * @date   08/05/2025
 */

 /************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
class DirMonitor
{
public:
    explicit DirMonitor(const std::string& pathName);
    ~DirMonitor();

    void Run(); // Non blocking
    
    void Subscribe(ACallback<std::string>* cb);
    void Unsubscribe(ACallback<std::string>* cb);
private:
    std::string m_pathName;
    int m_inotifyFD;
    std::atomic_bool m_isRun;

    std::thread m_runThread;
    std::mutex m_lock;

    Dispatcher<std::string> m_dispatcher;

    void ThreadWatch();
}; // DirMonitor
} // namespace ilrd_166_7

#endif // __ILRD_DIR_MONITOR_HPP__