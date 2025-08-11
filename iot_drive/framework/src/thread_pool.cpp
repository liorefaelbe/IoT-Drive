#include <iostream>         // cerr

#include "thread_pool.hpp" // ThreadPool

using namespace std;
namespace ilrd_166_7
{

const static ThreadPool::PRIORITY TOP = static_cast<ThreadPool::PRIORITY>(3);

/**************************** Implementations *********************************/
/********************************* ITask **************************************/

ThreadPool::ITask::~ITask() { /* empty */ }

/****************************** FunctionTask **********************************/

FunctionTask::FunctionTask(const std::function<void()>& func) : m_func(func) { /* empty */ }

FunctionTask::~FunctionTask() { /* empty */ }

void FunctionTask::Run()
{
    m_func();
}

/******************************* PoisonApple **********************************/

class PoisonApple : public ThreadPool::ITask
{ 
public:  
    PoisonApple(ThreadPool::thMap& map, mutex& lock, atomic_size_t& killed) : m_map(map), m_lock(lock), m_killed(killed) {} 

    void Run() override
    {
        thread::id id = this_thread::get_id();
        lock_guard<mutex> lock(m_lock);
        
        auto it = m_map.find(id);
        if (it != m_map.end())
        {
            it->second.second.set_value(true);
            m_killed.fetch_add(1);
        }
    }

private:
    ThreadPool::thMap& m_map;
    mutex& m_lock;
    atomic_size_t& m_killed;
};

/******************************* ThreadPool ***********************************/
/****************************** Constructors **********************************/

ThreadPool::ThreadPool(size_t numOfThreads) : m_running(false), m_numOfThreads(0)
{
    SetNumOfThreads(numOfThreads);
}

ThreadPool::~ThreadPool() noexcept
{
    try
    {
        SetNumOfThreads(0);
    }
    catch (...)
    {
        cerr << "ThreadPool destructor: SetNumOfThreads() threw an exception\n";
    }
}

/**************************** Member Functions ********************************/

void ThreadPool::SetNumOfThreads(size_t numOfThreads)
{
    if (numOfThreads >= m_numOfThreads)
    {
        CreateThreads(numOfThreads - m_numOfThreads);
    }
    else
    {
        DestroyThreads(m_numOfThreads - numOfThreads);
    }

    m_numOfThreads = numOfThreads;
}

void ThreadPool::AddTask(shared_ptr<ITask> task, PRIORITY priority)
{
    if (!task)
    {
        throw invalid_argument("Null task pointer");
    }
    
    m_taskQueue.Push(make_pair(priority, task));
}

void ThreadPool::Run() noexcept
{
    m_running.store(true);

    lock_guard<mutex> lock(m_cvLock);

    m_cvRun.notify_all();
}

void ThreadPool::Pause() noexcept
{
    m_running.store(false);
}

/**************************** Private Functions *******************************/

void ThreadPool::WaitForTasks(std::future<bool> future)
{
    thread::id myId = this_thread::get_id();

    while (future.wait_for(chrono::milliseconds(0)) != future_status::ready)
    {
        {
            unique_lock<mutex> lock(m_cvLock);
            m_cvRun.wait(lock, [this]() { return (m_running.load()); });
        }

        taskPair taskPair;
        if (m_taskQueue.Pop(taskPair, chrono::milliseconds(50)))
        {
            try
            {
                taskPair.second->Run();
            }
            catch (...)
            {
                cerr << "Thread " << myId << " unknown task exception" << endl;
            }
        }
    }

    lock_guard<mutex> lock(m_sysLock);
    m_thTermList.push_back(myId);
    m_cvJoin.notify_one();
}

void ThreadPool::CreateThreads(size_t threadsToAdd)
{
    for (size_t i = 0; i < threadsToAdd; ++i)
    {
   
        promise<bool> termPromise;
        future<bool> future = termPromise.get_future();
        thread th(&ThreadPool::WaitForTasks, this, move(future));

        lock_guard<mutex> lock(m_sysLock);
        m_thMap.insert({th.get_id(), thPair{move(th), move(termPromise)}});
    }
}

void ThreadPool::DestroyThreads(size_t threadsToRemove)
{
    m_thKilled.store(0);
    
    bool wasRunning = m_running.load();
    if (!wasRunning)
    {
        Run();
    }

    for (size_t i = 0; i < threadsToRemove; ++i)
    {
        lock_guard<mutex> lock(m_termLock);
        AddTask(make_shared<PoisonApple>(m_thMap, m_termLock, m_thKilled), TOP);
    }
    
    {
        unique_lock<mutex> lock(m_sysLock);
        m_cvJoin.wait(lock, [this, threadsToRemove]() { return (m_thKilled.load() == threadsToRemove); });
    }

    for (auto& id : m_thTermList)
    {
        auto it = m_thMap.find(id);
        if (it != m_thMap.end())
        {
            if (it->second.first.joinable())
            {
                it->second.first.join();
            }
            m_thMap.erase(it);
        }
    }
    
    m_thTermList.clear();
    

    if (!wasRunning)
    {
        Pause();
    }
}
}