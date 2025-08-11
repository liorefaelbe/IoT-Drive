#include "asyncInjection.hpp"   // AsyncInjection
#include "handleton.hpp"        // Hadleton

using namespace std;
namespace ilrd_166_7
{

/**************************** Implementations *********************************/
/**************************** AsyncInjection **********************************/


AsyncInjection::AsyncInjection(std::function<bool()> func, std::chrono::milliseconds time_interval_ms) 
                                    : m_func(func), m_time_interval_ms(time_interval_ms)
{   
    m_task = shared_ptr<AsyncInjectionTask>(new AsyncInjectionTask(*this));
    Handleton<Scheduler>::GetInstance()->AddTask(m_task, m_time_interval_ms);
}

AsyncInjection::~AsyncInjection() { /* empty */ }

void AsyncInjection::PerformAction()
{
    if (m_func())
    {
        Handleton<Scheduler>::GetInstance()->AddTask(m_task, m_time_interval_ms);
    }
    else
    {
        delete this;
    }
}

/*************************** AsyncInjectionTask *******************************/

AsyncInjection::AsyncInjectionTask::AsyncInjectionTask(AsyncInjection& async_injection) 
                                    : m_async_injection(async_injection) { /* empty */ }
        
void AsyncInjection::AsyncInjectionTask::Execute()
{
    m_async_injection.PerformAction();    
}
}