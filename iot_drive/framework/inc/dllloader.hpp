#ifndef __ILRD_DLL_LOADER_HPP__
#define __ILRD_DLL_LOADER_HPP__

#include <string>   // std::string

/**
 * @class  DllLoader
 * @brief  loads dynamic libraries based on notifications from DirMonitor
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
class DllLoader
{
public:
    explicit DllLoader() = default;
    ~DllLoader();
    DllLoader(const DllLoader& other) = delete;
    DllLoader& operator=(const DllLoader& other) = delete;

    void Load(const std::string& fileName);

private:
    void* m_handle;
}; // DllLoader
} // namespace ilrd_166_7

#endif // #define __ILRD_DLL_LOADER_HPP__
