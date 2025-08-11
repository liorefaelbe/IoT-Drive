#include <iostream>             // cout, endl
#include <cstring>              // strlen
#include <thread>               // thread
#include <atomic>               // atomic_bool
#include <unistd.h>             // write
#include <sys/socket.h>         // socketpair 

#include "reactor.hpp"          // Reactor 
#include "linux_listener.hpp"    // LinuxListener
#include "test_func.hpp"        // CheckValue

using namespace std;
using namespace ilrd_166_7;

/************************* Forward Declaration *******************************/

static int Test(void);
static int TestBasic(void);

/******************************* Main ****************************************/

int main()
{
    int status = 0;
    
    status += Test();
 
    (status == 0) ? (cout << "\n\033[0;32m\033[1mAll Good!\033[0m\n\n") : 
                    (cout << "\n\033[0;31m\033[1m" << status << " FAILS! \033[0mTotaL\n\n");
    
    return (0);
}

/***************************** Static Functions ******************************/

static int Test(void)
{
    int status = 0;
   
    cout << "\n\033[1m----------- Testing Reactor -----------\033[0m\n";
    
    status += TestBasic();

    return (status);
}

static int TestBasic(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting basic functionality:\033[0m\n";
    
    // Create socket pair for testing
    int sockfd[2];
    int result = socketpair(AF_UNIX, SOCK_STREAM, 0, sockfd);

    status += CheckValue(result != -1, true, "Socket pair creation", __LINE__);

    atomic_bool callbackTriggered(false);
    LinuxListener listener;
    Reactor reactor(&listener);

    // Subscribe to write event
    reactor.Subscribe(sockfd[1], Reactor::MODE::WRITE, [&](int fd, Reactor::MODE mode) 
    {
        if (!callbackTriggered.load()) 
        {
            callbackTriggered.store(true);
            cout << "Callback triggered for fd: " << fd << " | MODE: " << static_cast<int>(mode) << endl;
        }
    });

    thread reactorThread([&reactor]() 
    {
        reactor.Run();
    });

    // Write some data to trigger write event
    const char* testMessage = "Hello, Reactor!";
    write(sockfd[1], testMessage, strlen(testMessage));

    // Wait a bit longer to ensure reactor processes the event
    this_thread::sleep_for(chrono::milliseconds(10));

    reactor.Stop();
    reactorThread.join();

    status += CheckValue(callbackTriggered.load(), true, "Event callback triggered", __LINE__);

    // Clean up
    close(sockfd[0]);
    close(sockfd[1]);
    
    return (status);
}