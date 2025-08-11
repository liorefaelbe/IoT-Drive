#ifndef __ILRD_WAITABLE_QUEUE_HPP__
#define __ILRD_WAITABLE_QUEUE_HPP__

#include <queue>                // std::queue
#include <mutex>                // std::mutex
#include <condition_variable>   // std::condition_variable
#include <chrono>               // std::chrono::milliseconds

/**
 * @class  WaitableQueue
 * @brief  A thread-safe generic waitable queue
 * 
 * @tparam T: The type of elements stored in the queue
 *         Requirements for T:
 *              - Must be copy constructible or move constructible
 *              - Must support assignment operation
 *              - Must be destructible
 * 
 * @tparam Container: The underlying container type (default: std::queue<T>)
 *         Requirements for Container:
 *              - Must provide front(), push(), pop(), and empty() methods
 *              - Must maintain FIFO order unless specialized behavior is desired
 * 
 * @author Lior Reafael Berkovits
 * @date   23/04/2025
 */

 /************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
template <typename T, typename CONTAINER = std::queue<T>>
class WaitableQueue
{
public:
    explicit WaitableQueue() = default;
    WaitableQueue(const WaitableQueue& other) = delete;
    WaitableQueue& operator=(const WaitableQueue& other) = delete;
    ~WaitableQueue() = default;
    
    void Pop(T& out_param);
    bool Pop(T& out_param, std::chrono::milliseconds timeout);
    void Push(const T& data);
    bool IsEmpty() const noexcept; // not thread-safe
    
private:
    mutable std::timed_mutex m_lock;
    std::condition_variable_any m_cv;
    CONTAINER m_q;

    static_assert(std::is_same<typename CONTAINER::value_type, T>::value,"Container's value type must match T");
}; // WaitableQueue

/**************************** Implementations *********************************/
/**************************** Member Functions ********************************/

template <typename T, typename CONTAINER>
void WaitableQueue<T, CONTAINER>::Pop(T& out_param)
{
    std::unique_lock<std::timed_mutex> lock(m_lock);
    
    m_cv.wait(lock, [this]{ return !m_q.empty(); });
    
    out_param = m_q.front();

    m_q.pop();
}

template <typename T, typename CONTAINER>
bool WaitableQueue<T, CONTAINER>::Pop(T& out_param, std::chrono::milliseconds timeout)
{
    auto deadline = std::chrono::steady_clock::now() + timeout;

    if (!m_lock.try_lock_until(deadline))
    {
        return (false);
    }
    
    std::unique_lock<std::timed_mutex> lock(m_lock, std::adopt_lock);
    bool is_popped = m_cv.wait_until(lock, deadline, [this]{ return (!m_q.empty()); });
    
    if (!is_popped)
    {
        return (false);
    }
    
    out_param = m_q.front();

    m_q.pop();
    
    return (true);
}

template <typename T, typename CONTAINER>
void WaitableQueue<T, CONTAINER>::Push(const T& data)
{
    {
        std::unique_lock<std::timed_mutex> lock(m_lock);
        m_q.push(data);
    }   
    m_cv.notify_one();
}

template <typename T, typename CONTAINER>
bool WaitableQueue<T, CONTAINER>::IsEmpty() const noexcept
{
    std::unique_lock<std::timed_mutex> lock(m_lock);
    return (m_q.empty());
}
} // namespace ilrd_166_7

#endif // __ILRD_WAITABLE_QUEUE_HPP__