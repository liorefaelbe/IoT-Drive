#include <iostream>     // std::cerr

#include "reactor.hpp"  // Reactor

using namespace std;
namespace ilrd_166_7
{

/**************************** Implementations *********************************/
/******************************* IListener ************************************/

IListener::~IListener() { /* empty */ }

/******************************** Reactor *************************************/

Reactor::Reactor(IListener* pListener) : m_isRun(false), m_pListener(pListener) { /* empty */}

void Reactor::Run()
{
    m_isRun = true;

    while (m_isRun)
    {
        map_fd_mode triggered_fds = m_pListener->Listen(m_fdMap);
        for (auto fd : triggered_fds)
        {
            fd.second(fd.first.first,fd.first.second);
        }
    }
}

void Reactor::Stop()
{
    m_isRun = false;
}

void Reactor::Subscribe(int fd, MODE mode, std::function<void(int, MODE)> cbFunc)
{
    m_fdMap[make_pair(fd, mode)] = cbFunc;
}

void Reactor::Unsubscribe(int fd, MODE mode)
{
    m_fdMap.erase({fd, mode});
}
}
