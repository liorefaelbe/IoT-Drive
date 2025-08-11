#include <iostream>             // cout

#include "thread_pool.hpp"      // ThreadPool
#include "test_func.hpp"        // CheckValue

using namespace std;
using namespace ilrd_166_7;

mutex myLock;
atomic<int> counter(0);

class IncrementTask : public ThreadPool::ITask
{
public:
    IncrementTask(atomic<int>& counter) : m_counter(counter) {}
    ~IncrementTask() {}

    void Run() override
    {
        m_counter.fetch_add(1);
        this_thread::sleep_for(chrono::milliseconds(10));
    }

private:
    atomic<int>& m_counter;
};
        
class SlowIncrementTask : public ThreadPool::ITask
{
public:
    SlowIncrementTask(atomic<int>& counter) : m_counter(counter) {}
    ~SlowIncrementTask() {}

    void Run() override
    {
        m_counter.fetch_add(1);
        this_thread::sleep_for(chrono::milliseconds(50));
    }

private:
    atomic<int>& m_counter;
};

/************************* Forward Declaration *******************************/

static int Test(void);
static int TestBasic(void);
static int TestPriority(void);
static int TestPauseResume(void);
static int TestSetNumOfThreads(void);
static int TestEdgeCases(void);
static int TestThreadSafety(void);

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
   
    cout << "\n\033[1m----------- Testing Thread Pool -----------\033[0m\n";
    
    status += TestBasic();
    status += TestPriority();
    status += TestPauseResume();
    status += TestSetNumOfThreads();
    status += TestEdgeCases();
    status += TestThreadSafety();

    return (status);
}

static int TestBasic(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting basic functionality:\033[0m\n";
  
    ThreadPool pool(4);

    const int numTasks = 10;
    for (int i = 0; i < numTasks; ++i)
    {
        pool.AddTask(make_shared<IncrementTask>(ref(counter)), ThreadPool::PRIORITY::MEDIUM);
    }
    
    pool.Run();
    this_thread::sleep_for(chrono::milliseconds(1000));

    {
        lock_guard<mutex> lock(myLock);
        status += CheckValue(numTasks, counter.load(), "Basic task execution", __LINE__);
    }
    
    return (status);
}

static int TestPriority(void)
{ 
    int status = 0;

    cout << "\n\033[35m\033[1mTesting priority-based execution:\033[0m\n";
 
    vector<string> executionOrder;
    mutex orderMutex;

    ThreadPool pool(1);

    shared_ptr<ThreadPool::ITask> task1 = make_shared<FunctionTask>([&executionOrder, &orderMutex]()
    { 
        {
            lock_guard<mutex> lock(myLock);
            cout << "LOW" << endl;
        }
        {
            lock_guard<mutex> lock(orderMutex);
            executionOrder.push_back("LOW");
        }
    });
    
    shared_ptr<ThreadPool::ITask> task2 = make_shared<FunctionTask>([&executionOrder, &orderMutex]()
    { 
        {
            lock_guard<mutex> lock(myLock);
            cout << "MEDIUM" << endl;
        }
        {
            lock_guard<mutex> lock(orderMutex);
            executionOrder.push_back("MEDIUM");
        }
    });
    
    shared_ptr<ThreadPool::ITask> task3 = make_shared<FunctionTask>([&executionOrder, &orderMutex]()
    { 
        {
            lock_guard<mutex> lock(myLock);
            cout << "HIGH" << endl;
        }
        {
            lock_guard<mutex> lock(orderMutex);
            executionOrder.push_back("HIGH");
        }
    });
    
    shared_ptr<ThreadPool::ITask> task4 = make_shared<FunctionTask>([&executionOrder, &orderMutex]()
    { 
        {
            lock_guard<mutex> lock(myLock);
            cout << "TOP" << endl;
        }
        {
            lock_guard<mutex> lock(orderMutex);
            executionOrder.push_back("TOP");
        }
    });

    pool.AddTask(task1, ThreadPool::PRIORITY::LOW);
    pool.AddTask(task2, ThreadPool::PRIORITY::MEDIUM);
    pool.AddTask(task3, ThreadPool::PRIORITY::HIGH);
    
    pool.Run();

    this_thread::sleep_for(chrono::milliseconds(1000));

    vector<string> expectedOrder = {"HIGH", "MEDIUM", "LOW"};
    
    {
        lock_guard<mutex> lock(myLock);
        status += CheckValue(executionOrder.size() == expectedOrder.size(), true, "Execution count mismatch", __LINE__);
    }

    if (executionOrder.size() == expectedOrder.size()) 
    {
        for (size_t i = 0; i < expectedOrder.size(); ++i) 
        {
            {
                lock_guard<mutex> lock(myLock);
                status += CheckValue(executionOrder[i] == expectedOrder[i], true, "Incorrect priority execution at position " 
                                                                                    + to_string(i), __LINE__);
            }
        }
    }
    
    return (status);
}

static int TestPauseResume(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting pause and resume functionality:\033[0m\n";
    
    try
    {
        ThreadPool pool(2);
        atomic<int> counter(0);
        
        pool.Run();
        
        for (int i = 0; i < 10; ++i)
        {
            pool.AddTask(make_shared<IncrementTask>(ref(counter)), ThreadPool::PRIORITY::MEDIUM);
        }
        
        this_thread::sleep_for(chrono::milliseconds(50));
        
        pool.Pause();
        
        int counterAfterPause = counter.load();
        
        for (int i = 0; i < 10; ++i)
        {
            pool.AddTask(make_shared<IncrementTask>(ref(counter)), ThreadPool::PRIORITY::MEDIUM);
        }
        
        this_thread::sleep_for(chrono::milliseconds(1000));
        int counterAfterWait = counter.load();
        
        {
            lock_guard<mutex> lock(myLock);
            status += CheckValue(counterAfterPause, counterAfterWait, "No execution while paused", __LINE__);
        }
        
        pool.Run();
        
        this_thread::sleep_for(chrono::milliseconds(500));
        
        {
            lock_guard<mutex> lock(myLock);
            status += CheckValue(counter.load() == 20, true, "All tasks executed after resume", __LINE__);
        }
    }
    catch (const exception& e)
    {
        cout << "EXCEPTION: " << e.what() << endl;
        status += 1;
    }
    
    return (status);
}

static int TestSetNumOfThreads(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting SetNumOfThreads functionality:\033[0m\n";
    
    try
    {
        ThreadPool pool(2);
        atomic<int> counter(0);
        
        pool.Run();
        
        for (int i = 0; i < 20; ++i)
        {
            pool.AddTask(make_shared<SlowIncrementTask>(ref(counter)), ThreadPool::PRIORITY::MEDIUM);
        }
        
        this_thread::sleep_for(chrono::milliseconds(100));
        pool.SetNumOfThreads(4);

        this_thread::sleep_for(chrono::milliseconds(100));
        pool.SetNumOfThreads(1);
        
        this_thread::sleep_for(chrono::milliseconds(1000));

        {
            lock_guard<mutex> lock(myLock);
            status += CheckValue(counter.load(), 20, "All tasks executed with changing thread count", __LINE__);
        }
    }
    catch (const exception& e)
    {
        cout << "EXCEPTION: " << e.what() << endl;
        status += 1;
    }
    
    return (status);
}

static int TestEdgeCases(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting Edge Cases:\033[0m\n";
    
    try
    {
        ThreadPool pool(0);
        this_thread::sleep_for(chrono::milliseconds(500));
        {
            lock_guard<mutex> lock(myLock);
            cout << "Empty pool test passed" << endl;
        }

        pool.SetNumOfThreads(0);
        this_thread::sleep_for(chrono::milliseconds(500));
        {
            lock_guard<mutex> lock(myLock);
            cout << "Zero thread pool test passed" << endl;
        }
    }
    catch (const exception& e)
    {
        cout << "EXCEPTION: " << e.what() << endl;
        status += 1;
    }
    
    return (status);
}

static int TestThreadSafety(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting Thread Safety:\033[0m\n";
    
    try
    {
        ThreadPool pool;
        atomic<int> counter(0);

        pool.Run();
        
        for (int i = 0; i < 1000; ++i)
        {
            pool.AddTask(make_shared<IncrementTask>(ref(counter)), ThreadPool::PRIORITY::MEDIUM);
        }
        
        this_thread::sleep_for(chrono::milliseconds(2000));
        
        {
            lock_guard<mutex> lock(myLock);
            status += CheckValue(counter.load(), 1000,  "Thread safety test", __LINE__);
        }
    }
    catch (const exception& e)
    {
        cout << "EXCEPTION: " << e.what() << endl;
        status += 1;
    }
    
    return (status);
}
