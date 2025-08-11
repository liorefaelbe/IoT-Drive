#include <iostream>         // cout, endl
#include <memory>           // shared_ptr
#include <dlfcn.h>          // dlopen

#include "singleton.hpp"    // Singleton
#include "handleton.hpp"    // Handleton
#include "factory.hpp"      // Factory
#include "test_func.hpp"    // CheckValue

using namespace std;
using namespace ilrd_166_7;

/************************* Forward Declaration *******************************/

static int Test(void);
static int TestDynamicLoading(void);

/******************************* Main ****************************************/

int main()
{
    int status = 0;

    status += Test();

    (status == 0) ? (cout << "\n\033[0;32m\033[1mAll Good!\033[0m\n\n") :
                    (cout << "\n\033[0;31m\033[1m" << status << " FAILS! \033[0mTotaL\n\n");

    return 0;
}

/***************************** Static Functions ******************************/

static int Test(void)
{
    int status = 0;

    cout << "\n\033[1m----------- Testing Factory Handleton -----------\033[0m\n";

    status += TestDynamicLoading();

    return (status);
}

static int TestDynamicLoading(void)
{
    int status = 0;

    cout << "\n\033[35m\033[1mTest: Dynamic Loading:\033[0m\n";

    auto instance1 = Singleton<Factory<int, int, int>>::GetInstance();
    auto instance2 = Handleton<Factory<int, int, int>>::GetInstance();

    void* plugin = dlopen("./test/plugin.so", RTLD_LAZY);
    if (!plugin) 
    {
        cerr << "Cannot load plugin: " << dlerror() << endl;
        status += 1;
    }
    
    typedef size_t (*TestFunc)();

    TestFunc test1 = (TestFunc)dlsym(plugin, "test_singleton");
    if (!test1) 
    {
        cerr << "Cannot find test function: " << dlerror() << endl;
        dlclose(plugin);
        status += 1;
    }

    TestFunc test2 = (TestFunc)dlsym(plugin, "test_handleton");
    if (!test2) 
    {
        cerr << "Cannot find test function: " << dlerror() << endl;
        dlclose(plugin);
        status += 1;
    }
        
    dlclose(plugin);

    cout << "Singleton - Main process instance address: " << instance1 << endl;
    test1();

    cout << endl;

    cout << "Handleton - Main process instance address: " << instance2 << endl;
    test2();

    return (status);
}