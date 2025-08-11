#include <iostream>         // std::cerr
#include <dlfcn.h>          // std::dlopen
#include <unistd.h>         // std::read
#include <sys/inotify.h>    // std::inotify api
#include <stdexcept>        // std::runtime_error
#include <vector>           // std::vector
#include <poll.h>           // std::poll

#include "dir_monitor.hpp"  // DirMonitor

using namespace std;
namespace ilrd_166_7
{

static const size_t BUFFSIZE = 1024 * 10;

/**************************** Implementations *********************************/
/******************************* DirMonitor ***********************************/

DirMonitor::DirMonitor(const string& pathName) : m_pathName(pathName), m_inotifyFD(inotify_init1(IN_NONBLOCK)), m_isRun(false)
{
    if (-1 == m_inotifyFD)
    {
        throw std::runtime_error("inotify_init fail\n");
    }

    if (-1 == inotify_add_watch(m_inotifyFD, pathName.c_str(), IN_DELETE | IN_CLOSE_WRITE | IN_MOVED_TO))
    {
        throw std::runtime_error("inotify_add_watch fail\n");
    }
}

DirMonitor::~DirMonitor()
{
    m_isRun.store(false);
    m_runThread.join();
    close(m_inotifyFD);
}

void DirMonitor::Run()
{
    m_isRun.store(true); 
    m_runThread = thread(&DirMonitor::ThreadWatch, this);
}

void DirMonitor::Subscribe(ACallback<std::string>* cb)
{
    unique_lock<std::mutex> lock(m_lock);
    m_dispatcher.Subscribe(cb);
}

void DirMonitor::Unsubscribe(ACallback<std::string>* cb)
{
    unique_lock<std::mutex> lock(m_lock);
    m_dispatcher.Unsubscribe(cb);
}

void DirMonitor::ThreadWatch()
{
    struct pollfd pfd;
    pfd.fd = m_inotifyFD;
    pfd.events = POLLIN;

    char buffer[BUFFSIZE];

    while (m_isRun.load())
    {
        int ret = poll(&pfd, 1, 100);

        if (ret == -1)
        {
            perror("poll fail");
            continue;
        }

        if (ret == 0)
        {
            continue;
        }

        if (pfd.revents & POLLIN)
        {
            ssize_t len = read(m_inotifyFD, buffer, BUFFSIZE);
            if (len <= 0)
            {
                perror("read fail");
                continue;
            }

            char* ptr = buffer;
            while (ptr < buffer + len)
            {
                struct inotify_event* event = reinterpret_cast<struct inotify_event*>(ptr);

                if (event->mask & (IN_CLOSE_WRITE | IN_MOVED_TO))
                {
                    m_dispatcher.Notify(m_pathName + "/" + event->name);
                }

                if (event->mask & IN_DELETE)
                {
                    /* cout << event->name << " Deleted.\n"; */
                }

                ptr += sizeof(struct inotify_event) + event->len;
            }
        }
    }
}
}