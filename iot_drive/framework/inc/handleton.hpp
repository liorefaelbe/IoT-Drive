#ifndef __ILRD_HANDLETON_HPP__
#define __ILRD_HANDLETON_HPP__

#ifdef __HANDLETON_IMPLEMENTOR__

#include <mutex>    // std::mutex
#include <atomic>   // std::atomic_thread_fence
#include <cstdlib>  // std::atexit

/**
 * @class   Handleton
 * @brief   Thread-safe singleton that works across dynamic libraries
 * 
 * Features:
 *   - Template-based to work with any type
 *   - Thread-safe using double-checked locking pattern (DCLP)
 *   - Lazy initialization (only created when first needed)
 *   - Protection against copying
 *   - Guaranteed destruction
 *   - Diagnostics for improper use
 *   - Same instance across dynamic libraries
 * 
 * @tparam  T The type of element to turn into a handleton
 * 
 * @author  Lior Reafael Berkovits (adapted by additional work)
 * @date    04/05/2025
 */

/************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
template <typename T>
class Handleton
{
public:
    static T* GetInstance();
    
    Handleton() = delete;
    Handleton(const Handleton& other) = delete;
    Handleton& operator=(const Handleton& other) = delete;
    ~Handleton() = delete;

private:
    static std::atomic<T*> s_instance;
    static std::mutex s_mutex;
    static void Cleanup();
}; // Handleton

/**************************** Implementations *********************************/

template <typename T>
std::atomic<T*> Handleton<T>::s_instance{nullptr};

template <typename T>
std::mutex Handleton<T>::s_mutex;

template <typename T>
void Handleton<T>::Cleanup()
{
	delete s_instance.load();
    s_instance.store(reinterpret_cast<T*>(0xDEADBEEF));
}

template <typename T>
T* Handleton<T>::GetInstance()
{
    T* tmp = s_instance.load(std::memory_order_relaxed);

    std::atomic_thread_fence(std::memory_order_acquire);

    if (tmp == nullptr)
    {
        std::lock_guard<std::mutex> lock(s_mutex);
        
        tmp = s_instance.load(std::memory_order_relaxed);
        
        if (tmp == nullptr)
        {
            tmp = new T;
            
            std::atexit(Cleanup);
            std::atomic_thread_fence(std::memory_order_release);
            s_instance.store(tmp, std::memory_order_relaxed);
        }
    }

    return (tmp);
}

#define IMPLEMENT_HANDLETON(T) template class Handleton<T>;

}  // namespace ilrd_166_7

#else

#include <mutex>    // std::mutex
#include <atomic>   // std::atomic_thread_fence

namespace ilrd_166_7
{
template <typename T>
class Handleton
{
public:
    static T* GetInstance();
}; // Handleton
} // namespace ilrd_166_7

#endif // __HANDLETON_IMPLEMENTOR__
#endif // __ILRD_HANDLETON_HPP__