#include <iostream>         // cout
#include <string>           // string

#include "dispatcher.hpp"   // Dispatcher
#include "test_func.hpp"    // CheckValue

using namespace std;
using namespace ilrd_166_7;

/************************* Forward Declaration *******************************/

static int Test(void);
static int TestBasic(void);
static int TestMultipleCallbacks(void);
static int TestUnsubscribe(void);
static int TestDestructor(void);

/******************************* Classes *************************************/

class Observer
{
public:
    Observer() : m_notified(false), m_death_notified(false), m_value(0) {}
    
    void HandleEvent(int value)
    {
        m_notified = true;
        m_value = value;
    }
    
    void HandleDeath()
    {
        m_death_notified = true;
    }
    
    bool WasNotified() const
    {
        return (m_notified);
    }
    
    bool WasDeathNotified() const
    {
        return (m_death_notified);
    }
    
    int GetValue() const
    {
        return (m_value);
    }
    
    void Reset()
    {
        m_notified = false;
        m_death_notified = false;
        m_value = 0;
    }
    
private:
    bool m_notified;
    bool m_death_notified;
    int m_value;
};

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
   
    cout << "\n\033[1m----------- Testing Dispatcher -----------\033[0m\n";
    
    status += TestBasic();
    status += TestMultipleCallbacks();
    status += TestUnsubscribe();
    status += TestDestructor();

    return (status);
}

static int TestBasic(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting basic functionality:\033[0m\n";

    Dispatcher<int> dispatcher;
    Observer observer;

    Callback<int, Observer> callback(observer, 
                                    [](Observer& obs, int event) { obs.HandleEvent(event); },
                                    [](Observer& obs) { obs.HandleDeath(); });

    callback.SetDispatcher(&dispatcher);
    dispatcher.Notify(42);
    status += CheckValue(observer.WasNotified(), true, "Observer notification", __LINE__);
    status += CheckValue(observer.GetValue(), 42, "Observer received value", __LINE__);
    
    return (status);
}

static int TestMultipleCallbacks(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting multiple callbacks:\033[0m\n";

    Dispatcher<string> dispatcher;
    Observer observer1, observer2, observer3;

    Callback<string, Observer> callback1(observer1, 
                                       [](Observer& obs, string event) { obs.HandleEvent(event.length()); },
                                       [](Observer& obs) { obs.HandleDeath(); });
    
    Callback<string, Observer> callback2(observer2, 
                                       [](Observer& obs, string event) { obs.HandleEvent(event.length() * 2); },
                                       [](Observer& obs) { obs.HandleDeath(); });
    
    Callback<string, Observer> callback3(observer3, 
                                       [](Observer& obs, string event) { obs.HandleEvent(event.length() * 3); },
                                       [](Observer& obs) { obs.HandleDeath(); });
    callback1.SetDispatcher(&dispatcher);
    callback2.SetDispatcher(&dispatcher);
    callback3.SetDispatcher(&dispatcher);

    dispatcher.Notify("Test");
    status += CheckValue(observer1.WasNotified(), true, "Observer1 notification", __LINE__);
    status += CheckValue(observer1.GetValue(), 4, "Observer1 correct value", __LINE__);
    
    status += CheckValue(observer2.WasNotified(), true, "Observer2 notification", __LINE__);
    status += CheckValue(observer2.GetValue(), 8, "Observer2 correct value", __LINE__);
    
    status += CheckValue(observer3.WasNotified(), true, "Observer3 notification", __LINE__);
    status += CheckValue(observer3.GetValue(), 12, "Observer3 correct value", __LINE__);
    
    return (status);
}

static int TestUnsubscribe(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting unsubscribe functionality:\033[0m\n";

    Dispatcher<int> dispatcher;
    Observer observer1, observer2;

    Callback<int, Observer>* callback1 = new Callback<int, Observer>(
        observer1,
        [](Observer& obs, int event) { obs.HandleEvent(event); },
        [](Observer& obs) { obs.HandleDeath(); }
    );
    
    Callback<int, Observer> callback2(observer2,
        [](Observer& obs, int event) { obs.HandleEvent(event); },
        [](Observer& obs) { obs.HandleDeath(); }
    );
 
    callback1->SetDispatcher(&dispatcher);
    callback2.SetDispatcher(&dispatcher);

    dispatcher.Notify(42);
    status += CheckValue(observer1.WasNotified(), true, "Observer1 initial notification", __LINE__);
    status += CheckValue(observer2.WasNotified(), true, "Observer2 initial notification", __LINE__);

    observer1.Reset();
    observer2.Reset();

    dispatcher.Unsubscribe(callback1);
    delete callback1;

    dispatcher.Notify(100);
    status += CheckValue(observer1.WasNotified(), false, "Observer1 not notified after unsubscribe", __LINE__);
    status += CheckValue(observer2.WasNotified(), true, "Observer2 still notified after unsubscribe", __LINE__);
    status += CheckValue(observer2.GetValue(), 100, "Observer2 received correct value", __LINE__);
    
    return (status);
}

static int TestDestructor(void)
{
    int status = 0;
    
    cout << "\n\033[35m\033[1mTesting dispatcher destructor:\033[0m\n";
    
    Observer observer1, observer2;

    Callback<double, Observer> callback1(observer1,
        [](Observer& obs, double event) { obs.HandleEvent(static_cast<int>(event)); },
        [](Observer& obs) { obs.HandleDeath(); }
    );
    
    Callback<double, Observer> callback2(observer2,
        [](Observer& obs, double event) { obs.HandleEvent(static_cast<int>(event)); },
        [](Observer& obs) { obs.HandleDeath(); }
    );

    {
        Dispatcher<double> dispatcher;

        callback1.SetDispatcher(&dispatcher);
        callback2.SetDispatcher(&dispatcher);
        
        dispatcher.Notify(3.14);
        status += CheckValue(observer1.WasNotified(), true, "Observer1 notification", __LINE__);
        status += CheckValue(observer2.WasNotified(), true, "Observer2 notification", __LINE__);

        observer1.Reset();
        observer2.Reset();
    }
    
    status += CheckValue(observer1.WasDeathNotified(), true, "Observer1 death notification", __LINE__);
    status += CheckValue(observer2.WasDeathNotified(), true, "Observer2 death notification", __LINE__);
    
    return (status);
}