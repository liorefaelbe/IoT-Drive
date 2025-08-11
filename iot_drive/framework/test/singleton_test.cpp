#include <iostream>         // cout, endl
#include <string>           // string
#include <memory>           // shared_ptr
#include <thread>           // thread
#include <vector>           // vector
#include <mutex>            // mutex
#include <dlfcn.h>          // dlopen

#include "singleton.hpp"    // Singleton
#include "handleton.hpp"    // Handleton
#include "factory.hpp"      // Factory
#include "test_func.hpp"    // CheckValue

using namespace std;
using namespace ilrd_166_7;

// Base class for our tests
class Animal 
{
public:
    virtual ~Animal() = default;
    virtual string GetSound() const = 0;
    virtual string GetType() const = 0;
};

// Derived class 1
class Dog : public Animal 
{
public:
    Dog(const string& name) : m_name(name) {}
    
    string GetSound() const override { return "Woof"; }
    string GetType() const override { return "Dog"; }
    string GetName() const { return m_name; }
    
private:
    string m_name;
};

// Derived class 2
class Cat : public Animal 
{
public:
    Cat(const string& name) : m_name(name) {}
    
    string GetSound() const override { return "Meow"; }
    string GetType() const override { return "Cat"; }
    string GetName() const { return m_name; }
    
private:
    string m_name;
};

// Create Functions
shared_ptr<Animal> CreateDog(string name) { return(make_shared<Dog>(name)); }
shared_ptr<Animal> CreateCat(string name) { return(make_shared<Cat>(name)); }

/************************* Forward Declaration *******************************/

static int Test(void);
static int TestFactoryIsSingleton(void);
static int TestFactoryRegistrationAndCreation(void);
static int TestFactoryMultithreadedCreation(void);

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

    cout << "\n\033[1m----------- Testing Factory Singleton -----------\033[0m\n";

    status += TestFactoryIsSingleton();
    status += TestFactoryRegistrationAndCreation();
    status += TestFactoryMultithreadedCreation();

    return (status);
}

static int TestFactoryIsSingleton(void)
{
    int status = 0;

    cout << "\n\033[35m\033[1mTest: Singleton Identity of Factory:\033[0m\n";

    auto factory1 = Singleton<Factory<Animal, string, string>>::GetInstance();
    auto factory2 = Singleton<Factory<Animal, string, string>>::GetInstance();

    status += CheckValue(factory1 == factory2, true, "Different Factory instances returned", __LINE__);

    return (status);
}

static int TestFactoryRegistrationAndCreation(void)
{
    int status = 0;

    cout << "\n\033[35m\033[1mTest: Registration and Creation via Singleton Factory:\033[0m\n";

    auto factory = Singleton<Factory<Animal, string, string>>::GetInstance();

    factory->Add("dog", CreateDog);
    factory->Add("cat", CreateCat);

    shared_ptr<Animal> dog(factory->Create("dog", "a"));
    shared_ptr<Animal> cat(factory->Create("cat", "b"));

    status += CheckValue(dog != nullptr, true, "Failed to create dog", __LINE__);
    status += CheckValue(cat != nullptr, true, "Failed to create cat", __LINE__);
    status += CheckValue(dog->GetSound() == "Woof", true, "Dog Speak mismatch", __LINE__);
    status += CheckValue(cat->GetSound() == "Meow", true, "Cat Speak mismatch", __LINE__);

    return (status);
}

static int TestFactoryMultithreadedCreation(void)
{
    int status = 0;

    cout << "\n\033[35m\033[1mTest: Threaded Creation via Singleton Factory:\033[0m\n";

    constexpr size_t THREADS = 8;
    vector<thread> threads;
    vector<string> results(THREADS);
    mutex mtx;


    for (size_t i = 0; i < THREADS; ++i)
    {
        threads.emplace_back([i, &results, &mtx]() 
        {
            auto factory = Singleton<Factory<Animal, string, string>>::GetInstance();
            string sound;
            shared_ptr<Animal> pet = nullptr;

            if (i % 2 == 0)
            {   
                pet = (factory->Create("dog", "a"));
            }
            else
            {
                pet = (factory->Create("cat", "b"));
            }

            sound = pet ? pet->GetSound() : "null";

            lock_guard<mutex> lock(mtx);
            results[i] = sound;
        });
    }

    for (auto& t : threads) t.join();

    for (size_t i = 0; i < THREADS; ++i)
    {
        string expected = (i % 2 == 0) ? "Woof" : "Meow";

        status += CheckValue(expected, results[i], "Incorrect animal sound", __LINE__);
    }

    return (status);
}