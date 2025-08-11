#include <iostream>         // cout
#include <string>           // string
#include <thread>           // this_thread::sleep_for
#include <chrono>           // chrono::seconds
#include <fstream>          // ofstream
#include <filesystem>       // filesystem operations
#include <unistd.h>         // access function
#include <cstdlib>          // system function

#include "dllloader.hpp"    // DllLoader
#include "dir_monitor.hpp"  // DirMonitor
#include "test_func.hpp"    // CheckValue

using namespace std;
using namespace ilrd_166_7;

/************************* Forward Declaration ********************************/

static int Test(void);
static int TestBasic(void);

/********************************* Main ***************************************/

int main()
{
    int status = 0;
    
    status += Test();
 
    (status == 0) ? (cout << "\n\033[0;32m\033[1mAll Good!\033[0m\n\n") : 
                    (cout << "\n\033[0;31m\033[1m" << status << " FAILS! \033[0mTotaL\n\n");
    
    return (0);
}

/***************************** Static Functions *******************************/

static int Test(void)
{
    int status = 0;
   
    cout << "\n\033[1m----------- Testing Loader -----------\033[0m\n";
    
    status += TestBasic();

    return (status);
}

static int TestBasic(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting basic functionality:\033[0m\n";
  
    try 
    {
        string dirPath = "./plugins";

        DirMonitor monitor(dirPath);
        DllLoader loader;

        Callback<string, DllLoader> cb(loader,
                                    [](DllLoader& obj, string fileName) { obj.Load(fileName); },
                                    nullptr);

        monitor.Subscribe(&cb);
        
        monitor.Run();
        this_thread::sleep_for(chrono::seconds(30));
    }
    catch(exception& e)
    {
        cout <<e.what() <<endl;
    }

    return (status);
}