#ifndef __ILRD_REACTOR_HPP__
#define __ILRD_REACTOR_HPP__

#include <functional>   // std::function
#include <map>          // std::map
#include <sys/select.h> // std::select

/**
 * @class   Reactor
 * @brief   An event-driven reactor pattern implementation for handling I/O events
 *          on multiple file descriptors using a platform-independent approach.
 * 
 * Features:
 *   - Non-blocking I/O multiplexing using platform-specific mechanisms
 *   - Registration and unregistration of file descriptors with callbacks
 *   - Support for reading and writing events
 *   - Callbacks can modify registered handlers during execution
 * 
 * @author  Lior Reafael Berkovits
 * @date    12/05/2025
 */

/************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
class IListener;

class Reactor
{
public:
    enum class MODE {READ, WRITE, READ_AND_WRITE};
    using fd_mode = std::pair<int, MODE>;
    using map_fd_mode = std::map<fd_mode, std::function<void(int, MODE)>>;

    explicit Reactor(IListener* pListener);
    ~Reactor() = default;

    void Run(); 
    void Stop();

    void Subscribe(int fd, MODE mode, std::function<void(int, MODE)> cbFunc);
    void Unsubscribe(int fd, MODE mode);
private:
    bool m_isRun;
    IListener* m_pListener;
    map_fd_mode m_fdMap;
}; // Reactor

class IListener
{
public:
    virtual ~IListener();

    virtual Reactor::map_fd_mode Listen(Reactor::map_fd_mode& map) = 0;
}; // IListener
} // namespace ilrd_166_7

#endif // __ILRD_REACTOR_HPP__