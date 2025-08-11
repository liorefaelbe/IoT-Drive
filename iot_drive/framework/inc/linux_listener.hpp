#ifndef __ILRD_LINUX_LISTENER_HPP__
#define __ILRD_LINUX_LISTENER_HPP__

#include "reactor.hpp"  // IListener

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
class LinuxListener: public IListener
{
public:
    Reactor::map_fd_mode Listen(Reactor::map_fd_mode& map) override;
private:
    fd_set m_r_set;
    fd_set m_w_set;

    int m_max_fd;

    void SetSelect(Reactor::map_fd_mode& map);
}; // __ILRD_LINUX_LISTENER_HPP__
}

#endif // __ILRD_REACTOR_HPP__