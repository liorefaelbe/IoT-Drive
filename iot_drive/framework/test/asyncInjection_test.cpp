#include <iostream>             // std::cout, std::endl
#include <thread>               // std::this_thread::sleep_for
#include <atomic>               // std::atomic

#include "asyncInjection.hpp"   // AsyncInjection
#include "scheduler.hpp"        // Scheduler
#include "test_func.hpp"        // CheckValue

using namespace std;
using namespace ilrd_166_7;

/************************* Forward Declaration *******************************/

static int Test(void);
static int TestBasic(void);
static int TestAutoReschedule(void);
static int TestSelfDestruct(void);

static std::atomic<int> g_counter(0);
static const int EXPECTED_COUNT = 3;

/****************************** Tasks ****************************************/

auto no_reschedule_func = []() -> bool 
{
    g_counter++;
    return (false); // Never reschedule - should self-destruct
};

static auto reschedule_func = []() -> bool 
{
    g_counter++;
    return (g_counter < EXPECTED_COUNT); // Reschedule until counter reaches EXPECTED_COUNT
};

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
   
    cout << "\n\033[1m----------- Testing AsyncInjection -----------\033[0m\n";
    
    status += TestBasic();
    status += TestAutoReschedule();
    status += TestSelfDestruct();

    return (status);
}

static int TestBasic(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting basic functionality:\033[0m\n";

    g_counter = 0;

    new AsyncInjection(no_reschedule_func, std::chrono::milliseconds(100));
    
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    status += CheckValue(g_counter.load(), 1, "Task function execute", __LINE__);
    
    return (status);
}

static int TestAutoReschedule(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting auto-reschedule functionality:\033[0m\n";
    
    g_counter = 0;
    
    new AsyncInjection(reschedule_func, std::chrono::milliseconds(50));
    
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    
    status += CheckValue(g_counter.load(), EXPECTED_COUNT, "Auto-reschedule functionality", __LINE__);
    
    return (status);
}

static int TestSelfDestruct(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting self-destruct functionality:\033[0m\n";
  
    g_counter = 0;
    const int EXPECTED_FINAL_COUNT = 1;

    new AsyncInjection(no_reschedule_func, std::chrono::milliseconds(50));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    int initial_count = g_counter.load();
   
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
 
    status += CheckValue(g_counter.load(), initial_count, "Self-destruct functionality", __LINE__);

    status += CheckValue(initial_count, EXPECTED_FINAL_COUNT, "Task execute number", __LINE__);
    
    return (status);
}