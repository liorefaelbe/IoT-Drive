#ifndef __ILRD_DISPATCHER_HPP__
#define __ILRD_DISPATCHER_HPP__

#include <set>          // std::set
#include <functional>   // std::function

/**
* @class  Dispatcher
* @brief  An implementation of the Observer design pattern that dispatches events 
*         to registered callbacks
*
* Features:
*   - Dynamic registration and unregistration of callbacks
*   - Type-safe event notification
*   - Automatic callback cleanup when dispatcher is destroyed
*   - Support for custom notify functions in observers
*
* @tparam  EVENT The type of events to be dispatched
* 
* @author  Lior Reafael Berkovits
* @date    08/05/2025
*/

/************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
template <typename EVENT>
class Dispatcher;

template <typename EVENT>
class ACallback
{
public:
    explicit ACallback(Dispatcher<EVENT>* dis  = nullptr);
    virtual ~ACallback();

    virtual void Notify(const EVENT&) = 0;
    virtual void NotifyDeath();
    void SetDispatcher(Dispatcher<EVENT>* dis);

private:
    Dispatcher<EVENT>* m_dispatcer;
}; // ACallback

template <typename EVENT, typename OBSERVER>
class Callback : public ACallback<EVENT>
{
public:
    explicit Callback(OBSERVER& obs, std::function<void(OBSERVER&, const EVENT&)> notifyFunc, std::function<void(OBSERVER&)> notifyDeathFunc);
    
    virtual void Notify(const EVENT& event);
    virtual void NotifyDeath();

private:
    OBSERVER& m_observer;
    std::function<void(OBSERVER&, const EVENT&)> m_notifyFunc;
    std::function<void(OBSERVER&)> m_notifyDeathFunc;
}; // Callback

template <typename EVENT>
class Dispatcher
{
public:
    explicit Dispatcher() = default;
    ~Dispatcher();

    Dispatcher(const Dispatcher &other) = delete;
    Dispatcher &operator=(const Dispatcher &other) = delete;

    void Subscribe(ACallback<EVENT>* cb);
    void Unsubscribe(ACallback<EVENT>* cb);
    void Notify(const EVENT& event);

private:
    std::set<ACallback<EVENT>*> m_callbacks;
}; // Dispatcher

/**************************** Implementations *********************************/
/******************************* ACallback ************************************/

template <typename EVENT>
ACallback<EVENT>::ACallback(Dispatcher<EVENT>* dis) : m_dispatcer(dis)
{
    if (m_dispatcer)
    {
        m_dispatcer->Subscribe(this);
    }
}

template <typename EVENT>
ACallback<EVENT>::~ACallback()
{
    if (m_dispatcer)
    {
        m_dispatcer->Unsubscribe(this);
    }
}

template <typename EVENT>
void ACallback<EVENT>::NotifyDeath()
{
    m_dispatcer = nullptr;
}

template <typename EVENT>
void ACallback<EVENT>::SetDispatcher(Dispatcher<EVENT>* dis)
{
    if (m_dispatcer)
    {
        m_dispatcer->Unsubscribe(this);
    }
    
    m_dispatcer = dis;
    
    if (m_dispatcer)
    {
        m_dispatcer->Subscribe(this);
    }
}

/******************************** Callback ************************************/

template <typename EVENT, typename OBSERVER>
Callback<EVENT, OBSERVER>::Callback(OBSERVER& obs, 
        std::function<void(OBSERVER&, const EVENT&)> notifyFunc, 
        std::function<void(OBSERVER&)> notifyDeathFunc)
        : m_observer(obs), m_notifyFunc(notifyFunc), m_notifyDeathFunc(notifyDeathFunc)
{
    // empty ctor
}

template <typename EVENT, typename OBSERVER>
void Callback<EVENT, OBSERVER>::Notify(const EVENT& event)
{
    m_notifyFunc(m_observer, event);
}

template <typename EVENT, typename OBSERVER>
void Callback<EVENT, OBSERVER>::NotifyDeath()
{
    ACallback<EVENT>::NotifyDeath();
    m_notifyDeathFunc(m_observer);
}

/******************************** Callback ************************************/

template <typename EVENT>
Dispatcher<EVENT>::~Dispatcher()
{
    auto callbacks_copy = m_callbacks;

    for (auto cb : callbacks_copy)
    {
        cb->NotifyDeath();
    }

    m_callbacks.clear();
}

template <typename EVENT>
void Dispatcher<EVENT>::Subscribe(ACallback<EVENT>* cb)
{
    m_callbacks.insert(cb);
}

template <typename EVENT>
void Dispatcher<EVENT>::Unsubscribe(ACallback<EVENT>* cb)
{
    m_callbacks.erase(cb);
}

template <typename EVENT>
void Dispatcher<EVENT>::Notify(const EVENT& event)
{
    auto callbacks_copy = m_callbacks;
    
    for (auto cb : callbacks_copy)
    {
        cb->Notify(event);
    }
}
} // namespace ilrd_166_7

#endif // __ILRD_DISPATCHER_HPP__