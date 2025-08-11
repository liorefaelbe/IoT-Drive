#ifndef __ILRD_SINGLETON_HPP__
#define __ILRD_SINGLETON_HPP__

#include <mutex>    // std::mutex
#include <atomic>   // std::atomic_thread_fence
#include <cstdlib>  // std::atexit

/**
 * @class   Singleton
 * @brief   Thread-safe Singleton class template implementation
 * 
 * Features:
 *   - Template-based to work with any type
 *   - Thread-safe using double-checked locking pattern (DCLP)
 *   - Lazy initialization (only created when first needed)
 *   - Protection against copying
 *   - Guaranteed destruction
 *   - Diagnostics for use-after-destruction (aborts)
 *   - Portable C++ implementation
 * 
 * @tparam  T The type of element to turn into a singleton
 * 
 * @author  Lior Reafael Berkovits
 * @date    04/05/2025
 */

/************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
template <typename T>
class Singleton
{
public:
    static T* GetInstance();
    
    Singleton() = delete;
    Singleton(const Singleton& other) = delete;
    Singleton& operator=(const Singleton& other) = delete;
    ~Singleton() = delete;

private:
    static std::atomic<T*> s_instance;
    static std::mutex s_mutex;
    static void Cleanup();
}; // Singleton

/**************************** Implementations *********************************/

template <typename T>
std::atomic<T*> Singleton<T>::s_instance;

template <typename T>
std::mutex Singleton<T>::s_mutex;

template <typename T>
void Singleton<T>::Cleanup()
{
	delete s_instance.load();
    s_instance.store(reinterpret_cast<T*>(0xDEADBEEF));
}

template <typename T>
T* Singleton<T>::GetInstance()
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
} // namespace ilrd_166_7

#endif // __ILRD_SINGLETON_HPP__