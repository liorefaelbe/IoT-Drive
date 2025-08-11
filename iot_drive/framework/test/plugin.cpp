#include <iostream>         // cout, endl
#include <memory>           // shared_ptr
#include <dlfcn.h>          // dlopen

#include "singleton.hpp"    // Singleton
#include "handleton.hpp"    // Handleton
#include "factory.hpp"      // Factory

using namespace std;
using namespace ilrd_166_7;

extern "C" 
{
    void test_singleton() 
    {
        auto instance = Singleton<Factory<int, int, int>>::GetInstance();

        cout << "Singleton - Plugin instance address:       " << instance << endl;
    }

    void test_handleton() 
    {
        auto instance = Handleton<Factory<int, int, int>>::GetInstance();

        cout << "Handleton - Plugin instance address:       " << instance << endl;
    }
}