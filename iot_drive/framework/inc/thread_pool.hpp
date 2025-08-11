#ifndef __ILRD_THREAD_POOL_HPP__
#define __ILRD_THREAD_POOL_HPP__

#include <condition_variable>   // std::condition_variable
#include <mutex>                // std::mutex
#include <memory>               // std:shared_ptr
#include <thread>               // std::thread
#include <functional>           // std::function
#include <atomic>               // std::atomic_bool
#include <future>               // std::future, std::promise
#include <unordered_map>        // std::unordered_map
#include <list>                 // std::list

#include "w_q.hpp"              // WaitableQueue
#include "pq_adapter.hpp"       // PQAdapter

/**
 * @class  ThreadPool
 * @brief  A priority-based thread pool for concurrent task execution
 * 
 * Features:
 *   - Priority-based execution (LOW, MEDIUM, HIGH, TOP)
 *   - Configurable number of worker threads
 *   - Run/pause/decontamination control
 *   - Thread-safe operation
 * 
 * @author Lior Reafael Berkovits
 * @date   29/04/2025
 */

/************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
class ThreadPool
{
public:
    enum class PRIORITY {LOW, MEDIUM, HIGH};

    typedef typename std::pair<std::thread, std::promise<bool>> thPair;
    typedef typename std::unordered_map<std::thread::id, thPair> thMap;

    explicit ThreadPool(const size_t numOfThreads = std::thread::hardware_concurrency());
    ~ThreadPool() noexcept;

    ThreadPool(const ThreadPool& other) = delete;
    ThreadPool& operator=(const ThreadPool& other) = delete;

    class ITask
    {
    public:
        virtual ~ITask() = 0;
        void virtual Run() = 0;
    }; // ITask

    void AddTask(std::shared_ptr<ITask> task, PRIORITY priority);
    void Run() noexcept; 
    void Pause() noexcept;
    void SetNumOfThreads(size_t numOfThreads);
    
private:
    typedef typename std::pair<PRIORITY, std::shared_ptr<ITask>> taskPair;

    struct CmpTasks
    {
        bool operator()(const taskPair& p1, const taskPair& p2) const { return (p1.first < p2.first); }
    };

    std::atomic_bool m_running;
    std::atomic_size_t m_thKilled;
    size_t m_numOfThreads;

    thMap m_thMap;
    std::list<std::thread::id> m_thTermList;

    std::mutex m_cvLock;
    std::mutex m_termLock;
    std::mutex m_sysLock;
    std::condition_variable m_cvRun;
    std::condition_variable m_cvJoin;

    WaitableQueue<taskPair, PQAdapter<taskPair, CmpTasks>> m_taskQueue;

    void WaitForTasks(std::future<bool> future);
    void CreateThreads(const size_t threadsToAdd);
    void DestroyThreads(const size_t threadsToRemove);
}; // ThreadPool

class FunctionTask : public ThreadPool::ITask
{ 
public:  
    explicit FunctionTask(const std::function<void()>& func);
    ~FunctionTask();
    void Run() override;
    std::function<void()> m_func;
}; // FunctionTask

} // namespace ilrd_166_7

#endif // __ILRD_THREAD_POOL_HPP__