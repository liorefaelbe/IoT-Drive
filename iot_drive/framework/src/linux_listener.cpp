#include <iostream>     // std::cerr

#include "linux_listener.hpp"  // LinuxListener

using namespace std;
namespace ilrd_166_7
{

/**************************** Implementations *********************************/
/***************************** LinuxListener **********************************/

Reactor::map_fd_mode LinuxListener::Listen(Reactor::map_fd_mode& map)
{
    SetSelect(map);

    Reactor::map_fd_mode triggered_fds;

	for (auto it : map)
	{
        int fd = it.first.first;

		if ((FD_ISSET(fd, &m_r_set) != 0) || (FD_ISSET(fd, &m_w_set) != 0))
		{
			triggered_fds.insert(it);
		}
	}

    return (triggered_fds);
}

void LinuxListener::SetSelect(Reactor::map_fd_mode& map)
{
    FD_ZERO(&m_r_set);
    FD_ZERO(&m_w_set);

    for (auto it : map)
    {
        int fd = it.first.first;
        Reactor::MODE mode = it.first.second;

        switch(mode)
        {
            case Reactor::MODE::READ:
                FD_SET(fd, &m_r_set);
                break;
            case Reactor::MODE::WRITE:
                FD_SET(fd, &m_w_set);
                break;
            case Reactor::MODE::READ_AND_WRITE:
                FD_SET(fd, &m_r_set);
                FD_SET(fd, &m_w_set);
                break;
        }

        m_max_fd = max(m_max_fd, fd);
    }

	timeval timeout = {5,0};
	select(m_max_fd + 1, &m_r_set, &m_w_set, NULL, &timeout);
}
}